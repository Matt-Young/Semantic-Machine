#include "../include/sqlite3.h"
#include "all.h"


extern OP operands[];
extern M m;
// Table stuff, this will change fast and become part of named graphs
#define NUMBER_TABLES 20
PTABLE triple_tables[NUMBER_TABLES];

int del_table_count=0,new_table_count=0;
// direct sql utilities
#define Sql_create "drop table if exists %s; create table %s (key text,link integer, pointer integer);" 
int del_create_table(TABLE *table) {
  char buff[400];char  *err;int status;
  TRIPLE t = {buff,G_EXEC,0};
  G_sprintf(buff,Sql_create,table->name,table->name);
    status = sqlite3_exec(m.db,buff,0,0,&err);
  return( status);
}
#define Sql_delete_rows "delete from %s;"
int del_table_rows(TABLE *table) {
  char buff[400], *err; int status;
  G_sprintf(buff,Sql_delete_rows,table->name,table->name);
    status = sqlite3_exec(m.db,buff,0,0,&err);
	return(status);
}

PTABLE  new_table_context(char * name) {
  PTABLE pt;int i;
  pt = (PTABLE) G_calloc(sizeof(GRAPH));
  pt->name = (char *) G_calloc(G_strlen(name));
  pt->list = 0;
  pt->attribute = TABLE_NULL;
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
typedef int (*handler)(TRIPLE);
#define null_handler (handler) 0
extern int pop_handler(TRIPLE);

const struct {
	int index;
	int opid;
  char * sql;
  int (*handler)(TRIPLE);
  int maptype;
  int parm[4];
} pre_installed[3] = {
{pop_triple_operator,G_POP,"select key,link,pointer from %s where (gfun(0,rowid) == rowid);",pop_handler,
BIND_DEFAULT,0,0,0,0},
{append_triple_operator,G_APPEND,"insert into %s values( ?, ?, ?) ;",null_handler,
BIND_TRIPLE,0,0,0,0},
{update_triple_operator,G_UPDATE,"update %s set pointer = ? where rowid = (? + 1);",null_handler,
BIND_UPDATE,0,0,0,0},
};
const struct new_install{
	int opindex;
	int opid;
	char * sql;
	char * map_name[4];
} installs[] = {
	{pop_triple_operator,G_POP,"select key,link,pointer from %s where (gfun(0,rowid) == rowid);",0},
	{append_triple_operator,G_APPEND,"insert into %s values( ?, ?, ?) ;",
	  "bind_triple",0},
	{update_triple_operator,G_UPDATE,"update %s set pointer = ? where rowid = (? + 1);",
	"bind_self_row","bind_self_start",0},
	{0,0,0,0}
};
Mapper null_map(void * p,int * i);
 int make_stmt(TABLE * table,int format,char * table_name) {
  char buff[200];
  int i,status=SQLITE_OK;
  sqlite3_stmt *stmt;

  int opid = pre_installed[format].opid;
   int index = pre_installed[format].index;
  TRIPLE *p = &table->operators[index]; 
  // make an sql script
  G_sprintf(buff,pre_installed[format].sql, table_name);
  status= sqlite3_prepare_v2(m.db,buff,G_strlen(buff)+1, &stmt,0);
  p->key = (char *) stmt; 
  p->link = opid;  p->pointer = 0;
  operands[opid].stmt = stmt;  // Look in the table context for stmt
  operands[opid].handler = 0;
  operands[opid].properties = EV_Immediate;
  for(i=0; installs[format].map_name[i];i++) 
	  operands[opid].maps[0]= (Mapper) find_binder(installs[format].map_name[i]);
  return status;
}

int init_table(int index,char * name) {
  int status = SQLITE_OK;
  int i;

  TABLE * table =  new_table_context(name);
  del_create_table(table);
  for(i=0; i < NBUILTINS;i++) {
		make_stmt(table,i,name);

  }
  return status;
}
void gfunction(sqlite3_context* context,int n,sqlite3_value** v);
// pre insyalled, but these wiull become embedded in indices later
#define NTAB 5
 char * n[NTAB] = {"console"};
int init_tables() {
  int status,i;
  char buff[30];
  G_memset(triple_tables,0,sizeof(triple_tables));
  status = sqlite3_create_function_v2(m.db,GFUN,2,SQLITE_UTF8 ,0,gfunction,0,0,0);
  for(i=0; i < 1;i++) 
		 init_table(i,n[i]);
	 G_sprintf(buff,"select '%c',%d,0;",G_NULL,G_NULL);
  install_sql_script(buff,G_NULL);
  return status;
}