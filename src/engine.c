// G engine

#include "../src/include/config.h"

#include "../src/include/g_types.h"

#include "./include/machine.h"
#include "../src/include/tables.h"
#include "../src/include/graphs.h"
#include "../src/include/engine.h"
#include "../src/include/console.h"
#include "../src/include/names.h"

#include "../src/include/all.h"
#undef Debug_engine
// Will be shared Protected
OP operands[OperatorMaximum];
Pointer g_db;
Triple SCRATCH_Triple = {SystemScratch,0,"Scratch"};
int install_sql_script(char * ch,int opid) {
	int status;
	status =
		machine_prepare(g_db,ch,&operands[opid].stmt);
	operands[opid].handler=event_handler;
	operands[opid].maps[0] = 0;
	operands[opid].properties=0;
	return (EV_Ok);
}
int SEND_ONLY = 0;  // Global environment

// install key at installed operand position pointer
int  config_handler(Triple t[]) {
  int status;
  Code stmt = get_ready_stmt;
  const char * ch = t->key;
  int count = t[0].pointer-1; // exclude current triple
  int opid;int param; Triple var;
  Trio * trio;
  param=0;
  while(count) {
    if(EV_Data & (status = machine_step(stmt) )) {
      status = machine_triple(stmt,&var);
      if(var.link != '_') {
        if(param == 0) {
          opid = G_atoi(t->key);
          if((OperatorMaximum < opid) ) 
            return(EV_Incomplete);
        } else  if(param == 1 )  // install user script
          status = install_sql_script(var.key,opid); 
        else { // Install map
          trio = find_name(var.key);
          if(!trio || (trio->type != G_TYPE_MAPPER))
            return(EV_Incomplete);
          else
            operands[opid].maps[count-2]= (Mapper) trio->value;
        }
        param++;
      }
      count--;
    }
  }
  return(status);
}

int sql_handler(Triple *node) {
	int status=EV_Ok;
	install_sql_script((char *) node->key,SystemScratch);
	machine_new_operator((Triple *) &SCRATCH_Triple,0);
	return status;
}
int call_handler(Triple *node) {
	int pointer;
	pointer  =  incr_row(0);
	//set_row(G_atoi(node->key));
	//set_row(pointer);
	return EV_Ok;
}
int exec_handler(Triple *t) {
	int status; Triple v2;
	char *err;
 if(EV_Data &  machine_step(get_ready_stmt()) ) {
        machine_triple(get_ready_stmt(),&v2);
	status = machine_exec(g_db,v2.key,&err);
 } else G_printf("Sys Exec Error\n");
	return status;
}

int swap_handler(Triple *t) {return EV_Ok;}
int exit_handler(Triple *node) {  
	return EV_Done;}
int append_handler(Triple *node) { 
  incr_row(1);
	return set_ready_event(0);}
int unbind_handler(Triple *node) { 
    unbind_triple(operands[node->link].stmt,node+1);
	return EV_Done;
}
int len_handler(Triple *node) {
node[1].key = (char * ) machine_key_len(node[0].key);
return EV_Done;
}
int unbind_triples_handler(Triple *node) { 
  int status;int i=1;
  while(node->pointer) {
    status = machine_step(node->key);
    unbind_triple(operands[node->link].stmt,node+i++);
    if(status != EV_Error) node->pointer--;
    else return status;
  }
	return EV_Done;}
int pop_handler(Triple *node) {
	int status; Code stmt;
	Triple t;
  stmt = get_ready_stmt();
  status = set_ready_event(0);
  do {
    unbind_triple(stmt,&t);
    status = machine_new_operator(&t,0);
    status = machine_step(stmt );
  }while( status & EV_Data);
	  return status;
}
int script_handler(Triple *node) {  
	int id;
	id = G_atoi(node->key);
	G_printf("Script: \n%s\n",machine_script(operands[id].stmt));
	return EV_Ok;
}
int echo_handler(Triple *node) {
  G_printf("\nEcho  ");
  print_triple(node);
	return set_ready_event(0);
}
int dup_handler(Triple *node){
	int id,i;
	char buff[400];
	int status;
	id = G_atoi(node->key);
	G_strcpy(buff,(const char *) machine_script(operands[id].stmt));
	status = install_sql_script(buff,SystemScratch);
	if(status != EV_Ok) G_printf("Dup",EV_Error);
	for(i=0;operands[id].maps[i];i++) 
		operands[SystemScratch].maps[i] = operands[id].maps[i];
	return EV_Ok;
}
Handler get_ghandler(Triple top[],Handler handler) { 
	if(handler)
		return handler;
	else if(operands[top->link & OperatorMask].handler)
		return operands[top->link & OperatorMask].handler;
  else return pop_handler; // Try and execute the thing
}
#define DebugPrint
// Get the stmt
Code get_stmt(int opid,Triple * top) {
	if(operands[opid].properties & EV_Overload)
		return (Code) top[0].key;
	else if(operands[opid].properties & EV_Ugly)
		return 0;
	else 
		return operands[opid].stmt; // operand table rules
}
// We get here when a graph produces a triplet that heads a subsequence
// The link then holds the specified graph operator, 
// this sifts through and finds a handler

int  machine_set_operator(Triple top[],Handler handler) {
	int opid,events,status;
	Code stmt; 
	void * key;
	key = 0;stmt=0;events=0;
	status = EV_Ok;
	opid = top[0].link & OperatorMask;
  events = reset_ready_event(EV_No_bind | EV_Overload );
  events =operands[ opid ].properties; 
  if(top[0].link & OperatorMSB) 
      events |= EV_Overload;
	set_ready_stmt(get_stmt(opid,top));
	if(events & EV_Debug)
		G_printf("Debug event ");
	set_ready_code(opid);
  events = set_ready_event(events);
  DebugPrint("New Operator %s %d %d\n",top[0].key,top[0].link,top[0].pointer);
  return events;
}
int  machine_new_operator(Triple top[],Handler handler) {
  int events,status; Code stmt;
  events = machine_set_operator(top,handler);
  status = EV_Ok;
  stmt = get_ready_stmt();
	if(!(EV_No_bind & events))
		status = bind_code(top,stmt);
	if(status != EV_Ok) 
		G_printf("bind %x ",status);
   handler = get_ghandler(top,handler);
	if(!stmt) 
	  handler(top);
	else 
    machine_loop(top,handler);
	return 0;
}
//#define EV_Forms 0xe0000  

const struct {
	char * name;
	int opid;
	int properties;
	Handler handler;
}  map[SystemMax +1] = {
	{"SystemExit",SystemExit,EV_No_bind,exit_handler},
	{"SystemCall",SystemCall,EV_No_bind,call_handler},
	{"SystemDup",SystemDup,EV_No_bind,dup_handler},
	{"SystemPop",SystemPop,EV_No_bind,pop_handler},
	{"SystemExec",SystemExec,EV_No_bind+EV_FormZero,exec_handler},
	{"SystemScript",SystemScript,EV_No_bind+EV_FormZero,sql_handler},
	{"SystemDecode",SystemDecode,EV_No_bind+EV_FormZero,script_handler},
	{"SystemConfig",SystemConfig,EV_No_bind+EV_FormOne,config_handler},
	{"SystemEcho",SystemEcho,EV_No_bind+EV_FormOne,echo_handler}
};
int init_handlers() {
	int i;
	i=0;
	while( (i < SystemMax +1) && (map[i].name != 0)) {
		operands[map[i].opid].handler = map[i].handler;
		operands[map[i].opid].properties = map[i].properties;
		operands[map[i].opid].stmt=0; 
		add_trio(map[i].name,G_TYPE_SYSTEM,(Pointer) map[i].opid);
		i++;
	}
	return 0;
}


Mapper map_debugger(Pointer * p,int *type) {
	*type = G_TYPE_CODE;
	return 0;
}
Handler g_debugger(Triple *);
int g_debugger_state;
Trio engine_trios[] = { 
	{ "Debug", G_TYPE_HANDLER, g_debugger},
	{ "Testing", G_TYPE_BIT, (Pointer) EV_Debug},
  { "UnbindTriple", G_TYPE_HANDLER, (Handler) unbind_handler},
  { "AppendHandler", G_TYPE_HANDLER, (Handler) append_handler},
  { "ExitHandler", G_TYPE_HANDLER, (Handler) exit_handler},
   { "return", G_TYPE_ADDR, 0},
	{0,0,0}};

	// defult operands
extern Triple _null_graph;
  int ugly_handler(Triple *top);
	int init_operands() {
		int i; 
		G_memset(operands,0,sizeof(operands));
		for(i=SystemUser;i < OperatorMaximum;i++) {
			operands[i].handler = ugly_handler;
			if(G_isugly(i) )
				operands[i].properties = EV_Ugly |EV_No_bind;
			else
				operands[i].properties= 0; 
		}
	//i = install_sql_script("select '_',98,0;",SystemNull);
		return(i);
	}

	// Three loops that independent processes can run in
	void netio_loop() {G_printf("No network\n");}
	void debug_loop() {
	//Console c;
	//Triple t;
	//int status;
	for(;;) {}  // no debugger!!
}
  void set_return(Webaddr *w);

#define TestAddr  "2001:db8:8714:3a90::12"
  int debug_json_string(Webaddr * w);
int test_qson() ;
void console_loop(){
	Webaddr from,to;
  from.sa_family = AF_CONSOLE;
  to.sa_family = AF_TABLE;
  from.fd = Json_IO;
  G_strcpy((char *) to.addr,"console"); 
  G_printf("Console loop\n");
	for(;;) {
		//G_console(&from);
    debug_json_string(&from);
    system_copy_qson(&from,&to);
    G_free(from.buff);
    test_qson();
    flush_users();
    sort_names();
    G_buff_counts();
	}
}
int init_machine() {
	int status;
	status = init_handlers(); if(status != EV_Ok) return status;
	status = init_binder(); if(status != EV_Ok) return status;
	status = init_gfun(); if(status != EV_Ok) return status;
	status = init_tables(); if(status != EV_Ok) return status;
  sort_names(); 
  if(status != EV_Ok) return status;
	init_console();
	return status;
}

void engine_init() {
		int status; 
		OP op;
		status = open_machine_layer(GBASE,&g_db);
		if(status != EV_Ok) G_exit();
		G_printf("%s\n",GBASE);
		g_debugger_state=0;
		init_operands();
		op = operands[GCHAR];
		init_trios();
		add_trios(engine_trios);
		status = init_machine();
		op = operands[GCHAR];

    sort_names();

  }
#ifndef TEST_ADDR
char * TEST_ADDR = "127.0.0.1";
#endif
int port = TEST_PORT;

int main(int argc, char * argv[]) {
  int i;
  for(i=1; i < argc;i++) {
     G_printf("Arg: %s\n",argv[i]);
    if(!G_strcmp(argv[i], "-V")) {
      G_printf("You are using %s.\n",VERSION);
      G_exit();
    } else if( !G_strcmp(argv[i], "--help")  || !G_strcmp(argv[i], "-h") ) {
      G_printf("Usage: graphs\n");
      G_printf("\n");
      G_printf("Please see https://github.com/Matt-Young/Semantic-Machine/wiki .\n");
    } else if(!G_strcmp(argv[i], "-port") )
  {  G_printf("Port changed %s\n",argv[i+1]);port = G_strtol(argv[i+1]); i++;}
   else if(!G_strcmp(argv[i], "-file") ) {
  Webaddr c;
  c.buff = (int *) G_malloc(400);
  G_InitConsole(&c);
  console_file(&c,argv[i+1]); 
  G_free(c.buff);
    return(0);
   }

      G_printf("Port %d\n",port);
     engine_init();
		// Main loop

     print_trios();
    net_start((void *) port);
   G_printf("Main engine\n");
	  console_loop();
   }
		return(0);
	}
	void print_triple(Triple *t) { 
		G_printf(" %10s %c %4d  ",t->key,t->link,t->pointer);}
	// his is a little debgger, and stays ith this file
	Handler g_debugger(Triple *t) {
		print_triple(t);
		return 0;
	}
  
