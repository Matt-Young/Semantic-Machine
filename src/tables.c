#include "../include/sqlite3.h"
#include "g.h"
#include "graphs.h"
#include "binds.h"
extern OP operands[];
extern M m;
// Table stuff
TABLE tables[] =
  { {"console",0,0},{"config",0,0},{"self",0,0},{"other",0,0},{"result",0,0}};

PGRAPH *LIST(int i) {
  return (PGRAPH *) &tables[i].list;
}
char * NAME(TABLE *table) {
  return tables->name;
}
int ATTRIBUTE(TABLE *table) {
  return tables->attribute;
}
#define Sql_delete "delete from %s;"
int DELETE_TABLE(TABLE *table) {
  char buff[40];
  TRIPLE t = {buff,G_EXEC,0};
  G_sprintf(buff,Sql_delete,tables->name);
  return(triple(t,0));
}

TABLE * TABLE_POINTER(int i) { return &tables[i];}
void set_table_name(char * name,int index) { tables[index].name =  name;}
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

TRIPLE install(int index,int opid,int format) {
  char buff[200];
  int i,status;
  TRIPLE t = {buff,G_CONFIG,0};
  G_sprintf(buff,"%d",opid);
 
  status = triple(t,0);
  G_sprintf(buff,pre_installed[format].sql, tables[index].name);
  t.pointer++; 
  status = triple(t,0);
  for(i=0;i < 4;i++) {
    G_sprintf(buff,"%d", pre_installed[format].parm[i]);
    t.pointer++; status = triple(t,0);
    }
  operands[opid].handler = pre_installed[format].handler;
  t.link = opid;t.key=tables[index].name;t.pointer=0;
  return t;
}
int init_table(int index) {
  int status = SQLITE_OK;
  PGRAPH g = new_graph(LIST(index));
  TABLE * table = TABLE_POINTER(index);
  g->table = table;
  table->pop_triple = install(index,G_POP + index-1,0);
  table->insert_triple = install(index,G_INSERT + index-1,1);
  table->update_triple = install(index,G_UPDATE + index-1,2);  
  table->select_triple = table->pop_triple;  //default until posted 
                        //differenty by an attribute
  return status;
}
void gfunction(sqlite3_context* context,int n,sqlite3_value** v);
int init_gfun() {
  int status;
     status = sqlite3_create_function_v2(m.db,GFUN,2,SQLITE_UTF8 ,0,gfunction,0,0,0);
  init_table(1);  init_table(2); init_table(3); init_table(4); // skipping schema,console
  install_sql_script("select '_',96,0;",96);
  tables[0].pop_triple = NULL_TRIPLE;
  tables[0].select_triple = NULL_TRIPLE;
  return status;
}