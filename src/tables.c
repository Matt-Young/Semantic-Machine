#include "../include/sqlite3.h"
#include "g.h"
#include "graphs.h"
#include "binds.h"
extern OP operands[];
extern M m;
// Table stuff, this will change fast and become part of named graphs
// right now it assumes pre installed triplet tables for test
// not well linked in
PTABLE triple_tables[20];

int del_table_count=0,new_table_count=0;
// direct sql utilities
#define Sql_create "drop table if exists %s; create table %s (key text,link integer, pointer integer);" 
int CREATE_TABLE(TABLE *table) {
  char buff[400];char  *err;int status;
  TRIPLE t = {buff,G_EXEC,0};
  G_sprintf(buff,Sql_create,table->name,table->name);
    status = sqlite3_exec(m.db,buff,0,0,&err);
  return( status);
}
#define Sql_delete "delete from %s;"
int DELETE_TABLE(TABLE *table) {
  char buff[400], *err; int status;
  G_sprintf(buff,Sql_delete,table->name,table->name);
    status = sqlite3_exec(m.db,buff,0,0,&err);
	return(status);
}

PTABLE  new_table(char * name) {
  PTABLE pt;
  pt = (PTABLE) G_calloc(sizeof(GRAPH));
  pt->name = (char *) G_calloc(G_strlen(name));
  pt->list = 0;
  G_strcpy(pt->name,name);
  new_table_count++;
  return pt;
}
void free_table(PTABLE pt) {
   if(del_table_count >= new_table_count)
		G_error("Bad table",G_ERR_GRAPH);
    G_free((void *) pt->name);
	G_free((void *) pt);
	del_table_count++;
}
PGRAPH *LIST(int i) {
  return (PGRAPH *) triple_tables[i]->list;
}
char * NAME(TABLE *table) {
  return table->name;
}
int ATTRIBUTE(TABLE *table) {
  return table->attribute;
}

TABLE * TABLE_POINTER(int i) { return triple_tables[i];}
void set_table_name(char * name,int index) { triple_tables[index]->name =  name;}
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
  char * sql;
  int (*handler)(TRIPLE);
  int parm[4];
} pre_installed[3] = {
{"select key,link,pointer from %s where (gfun(0,rowid) == rowid);",pop_handler,
0,0,0,0},
{"insert into %s values( ?, ?, ?) ;",null_handler,
(BIND_TRIPLE << 4),0,0,0},
{"update %s set pointer = ? where rowid = (? + 1);",null_handler,
(BIND_GRAPH << 4) + BIND_GRAPH_ROW, (BIND_GRAPH << 4) + BIND_GRAPH_START,0,0},
};

 sqlite3_stmt * make_stmt(int index,int opid,int format,char * table_name) {
  char buff[200];
  int i,status;
  sqlite3_stmt *stmt;
  // make an sql script
  G_sprintf(buff,pre_installed[format].sql, table_name);
  status= sqlite3_prepare_v2(m.db,buff,G_strlen(buff)+1, &stmt,0);
//  triple_tables[index]->stmt = stmt;
  operands[opid].stmt = 0;  // Look in the table context for stmt
  for(i=0;i < 4;i++) {  // set bindings
	operands[opid].vp[i] = pre_installed[index].parm[i];
    }
  operands[opid].handler = pre_installed[index].handler;
  operands[opid].properties = index;
  return stmt;
}

int init_table(int index,char * name) {
  int status = SQLITE_OK;
  TRIPLE t;
  int i;
  //PGRAPH g = new_graph(LIST(index));
  TABLE * table =  new_table(name);
  CREATE_TABLE(table);
  triple_tables[index] = table;
  //g->table = table;
  // Turn operators into convenient triples
  t.pointer =0; 
  for(i=0; i < NBUILTINS;i++) {
		t.key = (const char *) make_stmt(i,G_POP+i,0,name);
  t.link = G_POP+i; table->operators[i] = t; 
  }
  return status;
}
void gfunction(sqlite3_context* context,int n,sqlite3_value** v);
// pre insyalled, but these wiull become embedded in indices later
#define NTAB 5
 char * n[NTAB] = {"console","config","self","other","result"};

int init_tables() {
  int status,i;
  char buff[30];
     status = sqlite3_create_function_v2(m.db,GFUN,2,SQLITE_UTF8 ,0,gfunction,0,0,0);
	 for(i=0; i < 1;i++) init_table(i,n[i]);
	 G_sprintf(buff,"select '%c',%d,0;",G_NULL,G_NULL);
  install_sql_script(buff,G_NULL);
  return status;
}