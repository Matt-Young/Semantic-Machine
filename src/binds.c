
/* This file contains all the routines to bind prepared 
and installed statements, and it prepares the table
for insert, update and selectg.  
Table stuff really doesn belong here, as table are going to be an
an index system built on the sqlite_master
*/
#include "../include/sqlite3.h"
#include "g.h"
#include "graphs.h"
#include "binds.h"
extern OP operands[];
extern M m;

struct {
  void * p;int t; } default_pointers[10];
int bind_default(sqlite3_stmt *stmt,TRIPLE top,int j,int i) {
  int t,status=SQLITE_OK;
  void * p;
  t = default_pointers[j].t;
  p = default_pointers[j].p;
  if(t ==  SQLITE_INTEGER)
      status = sqlite3_bind_int(stmt,i+1,*(int *)p);
  else if(t ==  SQLITE_TEXT)
      status = sqlite3_bind_text(stmt,i+1,(char *) p,G_strlen((char *) p),0);
  else
    G_error("No parameter",G_ERR_BIND);
  return status;
}
// various binds

int bind_triple(sqlite3_stmt *stmt,TRIPLE top) {
  int status,index;
  PGRAPH g;
  index = G_atoi(top.key);
  g = *LIST(index);
  status = sqlite3_bind_text(stmt,1,
    g->pending_triple.key,
    G_strlen(g->pending_triple.key),0);
  status = sqlite3_bind_int(stmt,2,g->pending_triple.link);
  status = sqlite3_bind_int(stmt,3,g->pending_triple.pointer);
  return status;
  }
char buffer[200];
int index = 0;
int local_handler(TRIPLE t) {
  G_sprintf(buffer + G_strlen(buffer)," %s ,",t.key);
  return SQLITE_OK;
}
extern PGRAPH schema_graph;
int bind_schema(sqlite3_stmt *stmt,TRIPLE top) {
  PGRAPH g = schema_graph;
  TABLE * table = g->table;
  g->row = g->start;
  index = 0;
  G_sprintf(buffer,"select ");
  triple(table->pop_triple,local_handler);
  G_sprintf(buffer,"from %s where (gfun(0,rowid) == rowid);",
    NAME(g->table));
  install_sql_script(buffer,G_SCRATCH);
  return SQLITE_OK;
}
int bind_graph(sqlite3_stmt *stmt,PGRAPH g,int i,int j) {
  int status;
  if( j== BIND_GRAPH_ROW)
    status = sqlite3_bind_int(stmt,i+1,g->row);
  else if(j == BIND_GRAPH_START)
    status = sqlite3_bind_int(stmt,i+1,g->start);
  else if(j == BIND_GRAPH_END)
    status = sqlite3_bind_int(stmt,i+1,g->end);
  return status;
}
int bind_graph_var(sqlite3_stmt *stmt,TRIPLE top,int i,int j) {
  int index;
  PGRAPH g;
  index = G_atoi(top.key);
  g = *LIST(index);
  return bind_graph(stmt,g,i,j);
}

// Get bound up for a n sql step

int bind_sql(OP *op,TRIPLE top) {
  int status=SQLITE_OK;
  int i,j;
  sqlite3_stmt *stmt;
  stmt = operands[top.link].stmt;
  for(i=0; op->vp[i]; i++ ) {
    j = op->vp[i];
    if(j) {
    if((j >> 4) == BIND_DEFAULT)
      bind_default(stmt,top,j & 0x0f,i);
    if((j >> 4) == BIND_GRAPH)
      bind_graph_var(stmt,top,j & 0x0f,i);
    if((j >> 4) == BIND_SELF)
      bind_graph(stmt,*LIST(2),j & 0x0f,i);
    if((j >> 4) == BIND_OTHER)
      bind_graph(stmt,*LIST(3),j & 0x0f,i);
    if((j >> 4) == BIND_TRIPLE)
      bind_triple(stmt,top);
    if((j >> 4) == BIND_SCHEMA)
      bind_schema(stmt,top);
    }
    }
  return status;
}



