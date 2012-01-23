// G engine
#include "../include/sqlite3.h"
#include "all.h"
Pointer g_db;
#define NVARS 5
OP operands[OPERMAX];

// Another cheat to trip up re-entry
Triple triple_var;

const Triple SCRATCH_Triple = {"Scratch",G_SCRATCH,0};
//triple bind, unbind
void print_triple(Triple t) { G_printf(" %s %d %d\n",t.key,t.link,t.pointer);}
void unbind_triple(sqlite3_stmt *stmt,Triple *t) {
   t->key = (void *) sqlite3_column_text( stmt, 0);
  t->link= sqlite3_column_int(stmt, 1);
  t->pointer= sqlite3_column_int(stmt, 2);
}


int newcount=0;
int oldcount=0;
char * newkey(const char * key) {
    int size = G_strlen(key)+1;
    char * p = (char *) G_malloc(size);
    G_strncpy(p,key,size);
    newcount++;
    return(p);
  }
void delkey(const char * key) { 
  if(oldcount < newcount) 
  {oldcount++; G_free( (void *) key);}
}

int output_filter(Triple t);
int install_sql_script(char * ch,int opid) {
  int status;
  status =
    sqlite3_prepare_v2(g_db,ch,G_strlen(ch)+1, 
        &operands[opid].stmt,0);
  operands[opid].handler=event_handler;
        operands[opid].maps[0] = 0;
 operands[opid].properties=0;
        return (SQLITE_OK);
}


// install key at installed operand position pointer
// these are varable cheats for config state
  int variable;

int  config_handler(Triple t) {
    int status=SQLITE_OK;
    const char * ch = t.key; 
    //printf("Configure: \n");
    switch(t.pointer)  {
      case 0: //opcode pointer
        variable = G_atoi(ch);
        if((OPERMAX < variable) ) 
          return(SQLITE_MISUSE);
      break;
      case 1: // install user script
        ch = newkey(t.key);
        status = install_sql_script((char *) ch,variable);
        if(status != SQLITE_OK)
          G_error("Prepare",G_ERR_PREPARE);
        delkey(ch);
        if(status != SQLITE_OK)
          G_error("Prepare",G_ERR_PREPARE);
        break;
      default: // parameters
        break;
    }
    return status;
    }
int sql_handler(Triple node) {
  int status=SQLITE_OK;
  install_sql_script((char *) node.key,G_SCRATCH);
  triple((Triple *) &SCRATCH_Triple,0);
  return status;
}
int call_handler(Triple node) {
  int pointer;
  pointer  =  incr_row(0);
  set_row(G_atoi(node.key));
  set_row(pointer);
  return SQLITE_OK;
}
int exec_handler(Triple t) {
  int status;
  char *err;
  status = sqlite3_exec(g_db,t.key,0,0,&err);
  return status;
}

int swap_handler(Triple t) {return SQLITE_OK;}
int exit_handler(Triple node) {  
return G_DONE;}
int pop_handler(Triple node) {
  int status;
  Triple t;
  //incr_row(1);
  unbind_triple(operands[node.link].stmt,&t);
  status = triple(&t,0);
  if(stopped_row() )
    status = G_DONE;
  return status;
}
int script_handler(Triple node) {  
  int id;
  id = G_atoi(node.key);
  G_printf("Script: \n%s\n",sqlite3_sql(operands[id].stmt));
  return SQLITE_OK;
}
int echo_handler(Triple node) {  
  G_printf("Echo: \n%s %d \n",node.key,node.link);
  return SQLITE_OK;
}
int dup_handler(Triple node){
  int id,i;
  char buff[400];
  int status;
  id = G_atoi(node.key);
  G_strcpy(buff,sqlite3_sql(operands[id].stmt));
  status = install_sql_script(buff,G_SCRATCH);
  if(status != SQLITE_OK) G_error("Dup",G_ERR_DUP);
  for(i=0;operands[id].maps[i];i++) 
    operands[G_SCRATCH].maps[i] = operands[id].maps[i];
  return SQLITE_OK;
}

int ghandler(Triple top,int status,int (*handler)(Triple)) { 

  if( (status != SQLITE_ROW) && (status != SQLITE_DONE) && (status != SQLITE_OK)  )
     G_error("ghandle entry",G_ERR_ENTRY);
  else if(status == SQLITE_DONE && top.link <= G_GRAPH_MAX ) 
      status =  G_DONE;  
  else if(status != SQLITE_ROW && status != SQLITE_OK) 
    G_error("Ghandle ",G_ERR_HANDLER);
  else if(handler)
    handler(top);
  else if(operands[top.link].handler)
      status |= operands[top.link].handler(top);
  return status;
}

#define LINKMASK 0xff
// We get here when a graph produces a triplet that heads a subsequence
// The link then holds the specified graph operator, 
// this sifts through and finds a handler

int triple(Triple top[],int (*handler)(Triple)) {
  OP *op;
  int status= SQLITE_OK;
  Code stmt; 
  void * key;
  key = 0;
  if(top[0].link >= OPERMAX) 
    return SQLITE_MISUSE;

  op = &operands[top[0].link && LINKMASK];
  stmt = op->stmt;
  if(!(op->properties & EV_No_bind))  
	status = bind_sql(top,&stmt);
  if(status != SQLITE_OK) 
      G_error("bind \n",G_ERR_BIND);
  do {
    if(stmt) {
	//if(stmt) {
      status = sqlite3_step(stmt );
      if(status == SQLITE_DONE)
        sqlite3_reset(stmt);
	    //sqlite3_reset(stmt);
      }
    status = ghandler(top[0],status,handler);
    }  while( status == SQLITE_ROW );
  if(key)
    delkey((const char *) key);
  return status;
}


typedef struct {
  int index;
  int (*handler)(Triple);
  } MAP;
#define CALLS 8

const MAP map[CALLS] = {
  {G_EXIT,exit_handler},{G_CALL,call_handler},
  {G_DUP,dup_handler},{G_POP,swap_handler},
  {G_EXEC,exec_handler},{G_SQL,sql_handler},
  {G_SCRIPT,script_handler},{G_CONFIG,config_handler}
};
int init_handlers() {
  int i;
   for(i=0;i < CALLS;i++) {
     operands[map[i].index].handler = map[i].handler;
	 operands[map[i].index].properties = EV_No_bind;
	 operands[map[i].index].stmt=0; 
   }
   return 0;
}
int init_machine() {
  int status,i;
  for(i=G_USERMIN;i < OPERMAX;i++) 
    operands[i].handler = event_handler;
  G_printf("%s\n",GBASE);
   status = sqlite3_open(GBASE,&g_db);
  init_handlers();
  init_gfun();
  init_tables();

  init_console();
  return status;
}
// do the default _
Triple G_null_graph = {"_",'_',0};

Mapper map_debugger(Pointer * p,int *type) {
	*type = G_TYPE_CODE;
	return 0;
	}
NameValue engine_accessor_list[] = { { "debug", (Mapper) map_debugger},{0,0}};
const struct {char * name;int value;} internals[] =
{{"script",G_SCRIPT},{"exec",G_EXEC},
{"config",G_CONFIG},
{"debug", G_DEBUG},{0,0}};
int get_system_call(char * name) {
	int i=0;
	while(G_strcmp(internals[i].name,name)) i++;
	return(internals[i].value);
}
int main(int argc, char * argv[])
{
  int status; 
  //status = init_dll(); 
    init_name_value();
	add_name_value(engine_accessor_list);
  status = init_machine();

  print_name_value();
  for(;;) triple(&G_null_graph,0);
  //for(;;) status = dispatch();
  G_exit(0);
}




