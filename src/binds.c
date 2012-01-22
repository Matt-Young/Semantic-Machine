
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


// various binds

int bind_triple(sqlite3_stmt *stmt,TRIPLE top[]) {
  int status; char buff[100];
  PGRAPH g = self_graph();
 
  if(top[1].key) {
	status = sqlite3_bind_text(stmt,1,
	top[1].key,
	G_strlen(top[1].key),0);
  } else // always forcing the default where missing 
  {
	  G_sprintf(buff,"Script: \n%s\n",sqlite3_sql(stmt));
	  status = sqlite3_bind_text(stmt,1,"abc",3,0);
  }
  status = sqlite3_bind_int(stmt,2,top[1].link);
  status = sqlite3_bind_int(stmt,3,top[1].pointer);
  return status;
  }
char buffer[200];
int index = 0;
int local_handler(TRIPLE t) {
  G_sprintf(buffer + G_strlen(buffer)," %s ,",t.key);
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
int bind_graph_var(sqlite3_stmt *stmt,TRIPLE top[],int i,int j) {
  PGRAPH g;
  TABLE * t = get_table_name(top[0].key);
  g = t->list;
  return bind_graph(stmt,g,i,j);
}

// Get bound up for a n sql step
extern sqlite3_stmt * fetch_stmt(TRIPLE top);
extern sqlite3_stmt * set_ready_stmt(TRIPLE top);
 sqlite3_stmt * bind_sql(TRIPLE top[]) {
  int status=SQLITE_OK;
  int i,j;
  sqlite3_stmt * stmt;
  OP * op = &operands[top[0].link];
  // Get and set the stmt
  	if(op->properties & EV_Immediate)
		stmt = (sqlite3_stmt *) top[0].key;
	else if( operands[top[0].link].properties & EV_Operand)
		stmt = operands[top[0].link].stmt; // operand table rules
	else
		return (0);
    j = op->maptype;
    if(j  == BIND_GRAPH)
      bind_graph_var(stmt,top,j & 0x0f,i);
    else if(j == BIND_SELF)
      bind_graph(stmt,self_graph(),j & 0x0f,i);
    else if(j  == BIND_OTHER)
      bind_graph(stmt,other_graph(),j & 0x0f,i);
    else if(j == BIND_TRIPLE)
      bind_triple(stmt,top);
  m.status = SQLITE_OK;
  return stmt;
}