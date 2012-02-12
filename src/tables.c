//#include "sqlite_msgs.h"
#include "all.h"
#include "filter.h"
// Table stuff, this will change fast and become part of named graphs
#define NUMBER_TABLES 20
// Shared memory
PTABLE triple_tables[NUMBER_TABLES];

extern int del_table_count,new_table_count;
PTABLE  new_table_context() {
  PTABLE pt;
  pt = (PTABLE) G_calloc(sizeof(TABLE));
  new_table_count++;
return(pt);}

void free_table_context(PTABLE pt) {
   if(del_table_count >= new_table_count)
		G_error("Bad table",G_ERR_GRAPH);
	G_free((void *) pt);
	del_table_count++;
}

PTABLE  get_table_context(char * name) {
  PTABLE pt; int i;
  pt = new_table_context();
  pt->name = (char *) G_calloc(G_strlen(name));
  G_strcpy(pt->name,name);
  pt->list = 0;
  pt->attribute = G_TYPE_TABLE;

  for(i=0;i< NUMBER_TABLES;i++) {if(triple_tables[i]==0) break;}
  pt->index = i;
  triple_tables[i] = pt;
  return pt;
}
void release_table_context(PTABLE pt) {
   triple_tables[pt->index]=0;
    G_free((void *) pt->name);
	G_free((void *) pt);
	del_table_count++;
}

// direct sql utilities
#define Sql_create "drop table if exists %s; create table %s (key text,link integer, pointer integer);" 
int del_create_table(TABLE *table) {
  char buff[400];char  *err;int status;
  Triple t = {buff,SystemExec,0};
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

TABLE * TABLE_POINTER(int i) { return triple_tables[i];}
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
	{pop_triple_operator,SystemMax+1,
	"select key,link,pointer from %s where (gfun(0,rowid) == rowid);",
  "UnbindTriple",0,0,0},
	{append_triple_operator,SystemMax+2,"insert into %s values( ?, ?, ?);",
	  "BindTriple","AppendHandler",0,0},
	{update_triple_operator,SystemMax+3,"update %s set pointer = (?) where rowid = ?;",
	"BindRelativeSelfRow","BindSelfStart","ExitHandler",0},
	{0,0,0,0,0,0,0}
};
Mapper null_map(void * p,int * i);
 int make_stmt(TABLE * table,int format,char * table_name) {
  char buff[200];
  int i,status=EV_Ok;
  Code stmt;Trio  * local_symbol;
  int opid = installs[format].opid;
  int index = installs[format].opindex;
  Triple *p = &table->operators[index]; 
  // make an sql script
  G_sprintf(buff,installs[format].sql, table_name);
  status= machine_prepare(g_db,buff, &stmt);
  if(status != EV_Ok) return status;
  p->key = (char *) stmt; 
  p->link = opid;  p->pointer = 0;
  operands[opid].stmt = stmt;  // Look in the table context for stmt
  operands[opid].handler = 0;  // May be over ridden
  operands[opid].properties = EV_Overload;
  for(i=0; installs[format].map_name[i];i++) {
	  local_symbol = find_name(installs[format].map_name[i]);
  if(local_symbol->type == G_TYPE_MAPPER)
  	  operands[opid].maps[i]= (Mapper) local_symbol->value;
  else if(local_symbol->type == G_TYPE_BIT)
    operands[opid].properties |= (int) local_symbol->value;
    else if(local_symbol->type == G_TYPE_HANDLER)
    operands[opid].handler = (Handler) local_symbol->value;
  }
  return status;
}

int init_table(char * name,int options,TABLE **table) {
	 int status = EV_Ok;
	 int i;
	 *table =  get_table_context(name);
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
	G_memset(triple_tables,0,sizeof(triple_tables));
	del_table_count=0,new_table_count=0;
	status = machine_install_callback(g_db,GFUN,2,gfunction);
	return status;
}