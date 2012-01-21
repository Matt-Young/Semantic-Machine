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
PTABLE  new_table(char * name) {
  PTABLE pt;
  pt = (PTABLE) G_calloc(sizeof(GRAPH));
  pt->name = (char *) G_calloc(G_strlen(name));
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
#define Sql_delete "delete from %s;"
int DELETE_TABLE(TABLE *table) {
  char buff[40];
  TRIPLE t = {buff,G_EXEC,0};
  G_sprintf(buff,Sql_delete,table->name);
  return(triple(t,0));
}
PGRAPH other(PGRAPH  g) {
  if(g != triple_tables[2]->list)
    return (PGRAPH) triple_tables[G_TABLE_SELF]->list;
  else
    return (PGRAPH) triple_tables[G_TABLE_OTHER]->list;
}
PGRAPH self(PGRAPH  g) {
  if(g != triple_tables[3]->list)
    return (PGRAPH) triple_tables[G_TABLE_SELF]->list;
  else
    return (PGRAPH) triple_tables[G_TABLE_OTHER]->list;
}
TABLE * TABLE_POINTER(int i) { return triple_tables[i];}
void set_table_name(char * name,int index) { triple_tables[index]->name =  name;}
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

 TRIPLE make_stmt(int index,int opid,int format) {
  char buff[200];
  int i,status;
  sqlite3_stmt *stmt;
  TRIPLE t = {buff,G_CONFIG,0};
  G_sprintf(buff,"%d",opid);
 
  status = triple(t,0);
  G_sprintf(buff,pre_installed[format].sql, triple_tables[index]->name);
  t.pointer++; 
  //status = triple(t,0);
  status= sqlite3_prepare_v2(m.db,buff,G_strlen(buff)+1, &stmt,0);
  triple_tables[index]->stmt = stmt;
  for(i=0;i < 4;i++) {
    G_sprintf(buff,"%d", pre_installed[format].parm[i]);
    t.pointer++; status = triple(t,0);
    }
  operands[opid].handler = pre_installed[format].handler;
  t.link = opid;t.key=(char *) stmt;t.pointer=0;
  return t;
}
int init_table(int index,char * name) {
  int status = SQLITE_OK;
  //PGRAPH g = new_graph(LIST(index));
  TABLE * table =  new_table(name);
  triple_tables[index] = table;
  //g->table = table;
  table->pop_triple = make_stmt(index,G_POP,0);
  table->insert_triple = make_stmt(index,G_INSERT,1);
  table->update_triple = make_stmt(index,G_UPDATE +,2);  
  table->select_triple = table->pop_triple;  //default until posted 
                        //differenty by an attribute
  return status;
}
void gfunction(sqlite3_context* context,int n,sqlite3_value** v);
// pre insyalled, but these wiull become embedded in indices later
#define NTAB 5
 char * n[NTAB] = {"console","config","self","other","result"};

int init_tables() {
  int status,i;
     status = sqlite3_create_function_v2(m.db,GFUN,2,SQLITE_UTF8 ,0,gfunction,0,0,0);
	 for(i=0; i < NTAB;i++) init_table(i,n[i]);
  install_sql_script("select '_',96,0;",96);
  triple_tables[0]->pop_triple = NULL_TRIPLE;
  triple_tables[0]->select_triple = NULL_TRIPLE;
  return status;
}