// G engine

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

int install_sql_script(char * ch,int opid) {
  int status;
  status =
	  machine_prepare(g_db,ch,&operands[opid].stmt);
  operands[opid].handler=event_handler;
        operands[opid].maps[0] = 0;
 operands[opid].properties=0;
        return (EV_Ok);
}


// install key at installed operand position pointer
// these are varable cheats for config state
  int variable;

int  config_handler(Triple t[]) {
    int status=EV_Ok;
	Code stmt = operands[t[0].link].stmt;
	const char * ch = t->key;
	int count = t[0].pointer;
	int opid;Triple var;
	Trio * trio;

    while(count) {
		status = machine_triple(stmt,&var);
		if(status != EV_Ok)
			G_error("Prepare",G_ERR_PREPARE);
		if(status == EV_Data) {
			if(count == 0 )  // install user script
				status = install_sql_script(var.key,opid);
			if(count ==1) {
				opid = G_atoi(t->key);
				if((OPERMAX < opid) ) 
					return(EV_Incomplete);
			} else  { // Install map
				trio = find_trio(var.key);
				if(!trio || (trio->type != G_TYPE_MAPPER))
					return(EV_Incomplete);
				else
					operands[opid].maps[count-1]= (Mapper) trio->value;
			} 
		}else
			return(status);
		count--;
		}
	return EV_Ok;
	}
int sql_handler(Triple *node) {
  int status=EV_Ok;
  install_sql_script((char *) node->key,G_SCRATCH);
  triple((Triple *) &SCRATCH_Triple,0);
  return status;
}
int call_handler(Triple *node) {
  int pointer;
  pointer  =  incr_row(0);
  set_row(G_atoi(node->key));
  set_row(pointer);
  return EV_Ok;
}
int exec_handler(Triple *t) {
  int status;
  char *err;
  status = machine_exec(g_db,t->key,&err);
  return status;
}

int swap_handler(Triple *t) {return EV_Ok;}
int exit_handler(Triple *node) {  
return EV_Done;}
int pop_handler(Triple *node) {
  int status;
  Triple t;
  unbind_triple(operands[node->link].stmt,&t);
  status = triple(&t,0);
  if(stopped_row() )
    status = EV_Done;
  return status;
}
int script_handler(Triple *node) {  
  int id;
  id = G_atoi(node->key);
  G_printf("Script: \n%s\n",machine_script(operands[id].stmt));
  return EV_Ok;
}
int echo_handler(Triple *node) {  
  G_printf("Echo: \n%s %d \n",node->key,node->link);
  return EV_Ok;
}
int dup_handler(Triple *node){
  int id,i;
  char buff[400];
  int status;
  id = G_atoi(node->key);
  G_strcpy(buff,(const char *) machine_script(operands[id].stmt));
  status = install_sql_script(buff,G_SCRATCH);
  if(status != EV_Ok) G_error("Dup",G_ERR_DUP);
  for(i=0;operands[id].maps[i];i++) 
    operands[G_SCRATCH].maps[i] = operands[id].maps[i];
  return EV_Ok;
}
  
int ghandler(Triple top[],int status,Handler handler) { 
  if( (status != EV_Data) && (status != EV_Done) && (status != EV_Ok)  )
     G_error("ghandle entry",G_ERR_ENTRY);  
  else if(status == EV_Done && (top->link & LINKMASK) < G_SYS_MAX ) 
      return(status);
  if(status == EV_Done)
	  set_ready_event(EV_No_data);
  if(handler)
    status = handler(top);
  else if(operands[top->link & LINKMASK].handler)
    status = operands[top->link & LINKMASK].handler(top);

  return status;
}


// We get here when a graph produces a triplet that heads a subsequence
// The link then holds the specified graph operator, 
// this sifts through and finds a handler

int triple(Triple top[],Handler handler) {
  OP *op;
  int status;
  int events;
  Code stmt; 
  void * key;
  key = 0;events=0;
  status = top[0].link;
  status &= EV_Overload;
  events = set_ready_event(top[0].link & EV_Overload);
  op = &operands[top[0].link && LINKMASK];
  stmt = op->stmt;
  if(!(op->properties & EV_No_bind))  
	status = bind_sql(top,&stmt);
  if(status != EV_Ok) 
      G_error("bind \n",G_ERR_BIND);
 if((!stmt) || (events & EV_Overload))
	 status = ghandler(top,status,handler);
 else {
	 do {
		status = machine_step(stmt );
		status = ghandler(top,status,handler);
		}  while( status == EV_Data );
	machine_reset(stmt);
	}
	
  if(key)
    delkey((const char *) key);
  return status;
}


typedef struct {
  char * name;
  int opid;
  Handler handler;
  } MAP;
#define CALLS 9

const MAP map[CALLS] = {
  {"SystemExit",G_EXIT,exit_handler},{"SystemCall",G_CALL,call_handler},
  {"SystemDup",G_DUP,dup_handler},{"SystemExit",G_POP,swap_handler},
  {"SystemExec",G_EXEC,exec_handler},{"SystemScript",G_SQL,sql_handler},
  {"SystemDecode",G_SCRIPT,script_handler},{"SystemConfig",G_CONFIG,config_handler},
  {"SystemEcho",G_ECHO,echo_handler}
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
	int status;
	G_printf("%s\n",GBASE);
	status = open_machine_layer(GBASE,&g_db);
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

// defult operands
const Triple G_null_graph = {"_",'_',0};
int init_operands() {
	int i; 
	G_memset(operands,0,sizeof(operands));
	for(i=G_USERMIN;i < OPERMAX;i++) {
		operands[i].handler = event_handler;
		operands[i].properties= EV_Null; }
	install_sql_script("select '_',95,0;",G_TYPE_NULL);
	G_printf("%s %d\n",GBASE,i);
	return(i);
	}
int main(int argc, char * argv[])
{
	int status; 
	Triple test;
	g_debugger_state=0;
	init_operands();
	init_trios();
	add_trios(engine_trios);
	status = init_machine();
	print_trios();
	G_memcpy(&test,&G_null_graph,sizeof(Triple));
#ifdef Debug_engine
   test.link |= EV_Overload;
#endif
  for(;;) triple(&test,0);
  G_exit(0);
}
void print_triple(Triple t) { G_printf(" %s %d %d\n",t.key,t.link,t.pointer);}
// his is a little debgger, and stays ith this file
Handler g_debugger(Triple t) {print_triple(t);return 0;}
