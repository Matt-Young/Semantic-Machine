#include "sqlite_msgs.h"
#include "all.h"
#include "filter.h"
// Table stuff, this will change fast and become part of named graphs
#define NUMBER_TABLES 20
PTABLE triple_tables[NUMBER_TABLES];

int del_table_count=0,new_table_count=0;
int init_table_context() {
 G_memset(triple_tables,0,sizeof(triple_tables));
 del_table_count=0,new_table_count=0;
  return 0;}
// direct sql utilities
#define Sql_create "drop table if exists %s; create table %s (key text,link integer, pointer integer);" 
int del_create_table(TABLE *table) {
  char buff[400];char  *err;int status;
  Triple t = {buff,G_EXEC,0};
  G_sprintf(buff,Sql_create,table->name,table->name);
    status = machine_exec(g_db,buff,&err);
  return( status);
}
#define Sql_delete_rows "delete from %s;"
int del_table_rows(TABLE *table) {
  char buff[400], *err; int status;
  G_sprintf(buff,Sql_delete_rows,table->name,table->name);
    status = machine_exec(g_db,buff,&err);
	return(status);
}

PTABLE  new_table_context(char * name) {
  PTABLE pt;int i;
  pt = (PTABLE) G_calloc(sizeof(GRAPH));
  pt->name = (char *) G_calloc(G_strlen(name));
  pt->list = 0;
  pt->attribute = G_TYPE_TABLE;
  G_strcpy(pt->name,name);
  for(i=0;i< NUMBER_TABLES;i++) {if(triple_tables[i]==0) break;}
  pt->index = i;
  triple_tables[i] = pt;
  new_table_count++;
  return pt;
}
void free_table_context(PTABLE pt) {
   if(del_table_count >= new_table_count)
		G_error("Bad table",G_ERR_GRAPH);
   triple_tables[pt->index]=0;
    G_free((void *) pt->name);
	G_free((void *) pt);
	del_table_count++;
}
PGRAPH new_table_graph(TABLE * table) {
 table->list = new_graph_context(); 
	table->list->table = table;
table->list->rowid=1;
  return table->list;
}
char * NAME(TABLE *table) {
  return table->name;
}
int ATTRIBUTE(TABLE *table) {
  return table->attribute;
}

TABLE * TABLE_POINTER(int i) { return triple_tables[i];}
void set_table_name(char * name,int index) { triple_tables[index]->name =  name;}
PGRAPH get_table_graph(int i){return triple_tables[i]->list;}

TABLE * get_table_name(const char * name) { 
	int i=0;
	while(triple_tables[i]) {
			if(!G_strcmp(triple_tables[i]->name,name))
				return (triple_tables[i]);
			i++;
		}
	return (0);
}


const struct new_install{
	int opindex;
	int opid;
	char * sql;
	char * map_name[4];
} installs[] = {
	{pop_triple_operator,G_POP,"select key,link,pointer from %s where (gfun(0,rowid) == rowid);",0},
	{append_triple_operator,G_APPEND,"insert into %s values( ?, ?, ?) ;",
	  "BindTriple",0},
	{update_triple_operator,G_UPDATE,"update %s set pointer = ? where rowid = (? + 1);",
	"BindSelfRow","BindSelfStart",0},
	{0,0,0,0}
};
Mapper null_map(void * p,int * i);
 int make_stmt(TABLE * table,int format,char * table_name) {
  char buff[200];
  int i,status=SQLITE_OK;
  Code stmt;
  int opid = installs[format].opid;
  int index = installs[format].opindex;
  Triple *p = &table->operators[index]; 
  // make an sql script
  G_sprintf(buff,installs[format].sql, table_name);
  status= machine_prepare(g_db,buff, &stmt);
  p->key = (char *) stmt; 
  p->link = opid;  p->pointer = 0;
  operands[opid].stmt = stmt;  // Look in the table context for stmt
  operands[opid].handler = pop_handler;
  operands[opid].properties = EV_Immediate;
  for(i=0; installs[format].map_name[i];i++) 
	  operands[opid].maps[0]= (Mapper) find_trio_value(installs[format].map_name[i]);
  return status;
}

int init_table(char * name,int options,TABLE **table) {
  int status = SQLITE_OK;
  int i;
  *table =  new_table_context(name);
  if(options)
	del_create_table(*table);
  for(i=0; i < NBUILTINS;i++) {
		make_stmt(*table,i,name);
  }
  return status;
}
void gfunction(Pointer context,int n,Pointer * v);

 Trio table_trios[] = {{"TablesInit",0,0},{0,0,0}};
int init_tables() {
	int status;
	add_trios(table_trios);
	init_table_context();
	status = machine_install_callback(g_db,GFUN,2,gfunction);
	return status;
}