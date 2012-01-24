// G engine
#include "sqlite_msgs.h"
#include "all.h"
#include "filter.h"
Pointer g_db;
#define NVARS 5
OP operands[OPERMAX];
#define LINKMASK 0x7f
// Another cheat to trip up re-entry
Triple triple_var;

const Triple SCRATCH_Triple = {"Scratch",G_SCRATCH,0};
//triple bind, unbind

void unbind_triple(Code stmt,Triple *t) {
   t->key = (char  *) machine_column_text(stmt,0); 
  t->link= machine_column_int(stmt, 1);
  t->pointer= machine_column_int(stmt, 2);
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
	  machine_prepare(g_db,ch,&operands[opid].stmt);
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
  status = machine_exec(g_db,t.key,&err);
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
  G_printf("Script: \n%s\n",machine_script(operands[id].stmt));
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
  G_strcpy(buff,(const char *) machine_script(operands[id].stmt));
  status = install_sql_script(buff,G_SCRATCH);
  if(status != SQLITE_OK) G_error("Dup",G_ERR_DUP);
  for(i=0;operands[id].maps[i];i++) 
    operands[G_SCRATCH].maps[i] = operands[id].maps[i];
  return SQLITE_OK;
}
  
int ghandler(Triple top,int status,int (*handler)(Triple)) { 
  if( (status != SQLITE_ROW) && (status != SQLITE_DONE) && (status != SQLITE_OK)  )
     G_error("ghandle entry",G_ERR_ENTRY);  
  else if(status == SQLITE_DONE && (top.link & LINKMASK) <= G_GRAPH_MAX ) 
      return(status);
  if(status == SQLITE_DONE)
	  set_ready_event(EV_No_data);
  if(handler)
    status = handler(top);
  else if(operands[top.link & LINKMASK].handler)
    status = operands[top.link & LINKMASK].handler(top);

  return status;
}


// We get here when a graph produces a triplet that heads a subsequence
// The link then holds the specified graph operator, 
// this sifts through and finds a handler

int triple(Triple top[],int (*handler)(Triple)) {
  OP *op;
  int status;
  int events;
  Code stmt; 
  void * key;
  key = 0;
  events = set_ready_event(top[0].link & EV_Overload);
  op = &operands[top[0].link && LINKMASK];
  stmt = op->stmt;
  if(!(op->properties & EV_No_bind))  
	status = bind_sql(top,&stmt);
  if(status != SQLITE_OK) 
      G_error("bind \n",G_ERR_BIND);
 if((!stmt) || (events & EV_Overload))
	 status = ghandler(top[0],status,handler);
 else {
	 do {
		status = machine_step(stmt );
		status = ghandler(top[0],status,handler);
		}  while( status == SQLITE_ROW );
	machine_reset(stmt);
	}
	
  if(key)
    delkey((const char *) key);
  return status;
}


typedef struct {
  char * name;
  int opid;
  int (*handler)(Triple);
  } MAP;
#define CALLS 8

const MAP map[CALLS] = {
  {"SystemExit",G_EXIT,exit_handler},{"SystemCall",G_CALL,call_handler},
  {"SystemDup",G_DUP,dup_handler},{"SystemExit",G_POP,swap_handler},
  {"SystemExec",G_EXEC,exec_handler},{"SystemScript",G_SQL,sql_handler},
  {"SystemDecode",G_SCRIPT,script_handler},{"SystemConfig",G_CONFIG,config_handler}
};
int init_handlers() {
  int i;
   for(i=0;i < CALLS;i++) {
     operands[map[i].opid].handler = map[i].handler;
	 operands[map[i].opid].properties = EV_No_bind;
	 operands[map[i].opid].stmt=0; 
	 add_trio(map[i].name,G_TYPE_SYSTEM,(Pointer) map[i].opid);
   }
   return 0;
}
int init_machine() {
  int status,i;
  for(i=G_USERMIN;i < OPERMAX;i++) 
    operands[i].handler = event_handler;
  G_printf("%s\n",GBASE);
status =   open_machine_layer(GBASE,&g_db);

  status = init_handlers();
  status = init_binder();
  status = init_gfun();
  status = init_tables();
  status = init_filters();
  init_console();
  return status;
}

Mapper map_debugger(Pointer * p,int *type) {
	*type = G_TYPE_CODE;
	return 0;
	}
Handler g_debugger(Triple );
int g_debugger_state;
Trio engine_trios[] = { 
	{ "Debug", G_TYPE_HANDLER, g_debugger},
	{ "Testing", G_TYPE_BIT, (Pointer) EV_Overload},
	{0,0,0}};
// do the default _
Triple G_null_graph = {"_",'_',0};
int main(int argc, char * argv[])
{
  int status; 
  Triple test;
 
  g_debugger_state=0;
    init_trios();
	add_trios(engine_trios);
  status = init_machine();

  print_trios();
   G_memcpy(&test,&G_null_graph,sizeof(Triple));
   test.link |= EV_Overload;
  for(;;) triple(&test,0);
  //for(;;) status = dispatch();
  G_exit(0);
}
void print_triple(Triple t) { G_printf(" %s %d %d\n",t.key,t.link,t.pointer);}
// his is a little debgger, and stays ith this file
Handler g_debugger(Triple t) {print_triple(t);return 0;}
