

#include "sqlite3.h"
#include "g.h"
#include "graphs.h"
extern OP operands[];
extern M m;
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
#define BIND_DEFAULT 1
#define BIND_GRAPH 2
#define BIND_OTHER 3
#define BIND_SELF 3
#define BIND_TRIPLE 5
#define BIND_SCHEMA 6
#define BIND_GRAPH_ROW 0
#define BIND_GRAPH_START 1
#define BIND_GRAPH_END 2
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

TRIPLE installed[9];
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
    gerror("No parameter",G_ERR_BIND);
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



