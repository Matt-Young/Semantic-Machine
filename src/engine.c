// G engine

#include "all.h"
#include "filter.h"

Pointer g_db;
#define NVARS 5
OP operands[OperatorMaximum];
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
				if((OperatorMaximum < opid) ) 
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
  else if(status == EV_Done && (top->link & LINKMASK) < SystemMax ) 
      return(status);
  if(status == EV_Done)
	  set_ready_event(EV_Done);
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
	int status;int opid;
	Code stmt; 
	void * key;
	key = 0;stmt=0;
	status = EV_Ok;
	opid = top[0].link;
	op = &operands[ opid & OperatorMask];
	set_ready_event(op->properties);
	if(EV_Overload & op->properties)
		stmt = top[0].key;
	else
		stmt = op->stmt;
	if(!(EV_No_bind & op->properties))
		status = bind_sql(top,&stmt);
  if(status != EV_Ok) 
      G_error("bind \n",G_ERR_BIND);
 if(!stmt)
	 status = ghandler(top,status,handler);
 else {
	 do {
		status = machine_step(stmt );
		status = ghandler(top,status,handler);
		}  while( status == EV_Data );
	machine_reset(stmt);
	}
  return status;
}


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
  {"SystemExec",SystemExec,EV_No_bind,exec_handler},
  {"SystemScript",SystemScript,EV_No_bind,sql_handler},
  {"SystemDecode",SystemDecode,EV_No_bind,script_handler},
  {"SystemConfig",SystemConfig,EV_No_bind,config_handler},
  {"SystemEcho",SystemEcho,EV_No_bind,0}
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
int init_machine() {
	int status;
	G_printf("%s\n",GBASE);
	status = open_machine_layer(GBASE,&g_db);
	if(status != EV_Ok) return status;
	status = install_sql_script("select '_',95,0;",G_TYPE_NULL);
	if(status != EV_Ok) return status;
	G_printf("%s\n",GBASE);
	status = init_handlers(); if(status != EV_Ok) return status;
	status = init_binder(); if(status != EV_Ok) return status;
	status = init_gfun(); if(status != EV_Ok) return status;
	status = init_tables(); if(status != EV_Ok) return status;
	status = init_filters(); if(status != EV_Ok) return status;
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
	for(i=SystemUser;i < OperatorMaximum;i++) {
		operands[i].handler = event_handler;
		operands[i].properties= EV_Null; }
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
