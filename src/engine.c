// G engine

#include "all.h"
#include "filter.h"
#undef Debug_engine
// Will be shared Protected
OP operands[OperatorMaximum];
Pointer g_db;
#define NVARS 5

const Triple SCRATCH_Triple = {"Scratch",SystemScratch,0};

//triple bind, unbind
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
	install_sql_script((char *) node->key,SystemScratch);
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
	status = install_sql_script(buff,SystemScratch);
	if(status != EV_Ok) G_error("Dup",G_ERR_DUP);
	for(i=0;operands[id].maps[i];i++) 
		operands[SystemScratch].maps[i] = operands[id].maps[i];
	return EV_Ok;
}

int ghandler(Triple top[],int status,Handler handler) { 
	if( (status != EV_Data) && (status != EV_Done) && (status != EV_Ok)  )
		G_error("ghandle entry",G_ERR_ENTRY);  
	else if(status == EV_Done && (top->link & OperatorMask) < SystemMax ) 
		return(status);
	set_ready_event(status);
	if(handler)
		status = handler(top);
	else if(operands[top->link & OperatorMask].handler)
		status = operands[top->link & OperatorMask].handler(top);
	return status;
}

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

int triple(Triple top[],Handler handler) {
	OP *op;
	int status;int opid,overload,events;
	Code stmt; 
	void * key;
	key = 0;stmt=0;events=0;
	status = EV_Ok;
	opid = top[0].link & OperatorMask;
	overload = top[0].link & OperatorMSB;
	stmt = get_stmt(opid,top);
	op = &operands[ opid ];
	
	if(!handler)
		handler = op->handler;
	events = set_ready_event(op->properties);
	if(events & EV_Debug)
		G_printf("Debug event\n");
	set_ready_code(stmt,opid);
	if(!(EV_No_bind & op->properties))
		status = bind_code(top,stmt);
	if(status != EV_Ok) 
		G_error("bind \n",G_ERR_BIND);
	if(!stmt || overload) {
		if(overload) 
			set_ready_event(EV_Overload);
		if(handler)
			status = ghandler(top,status,handler);
		else
			status = EV_Incomplete;
	}
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
Handler g_debugger(Triple *);
int g_debugger_state;
Trio engine_trios[] = { 
	{ "Debug", G_TYPE_HANDLER, g_debugger},
	{ "Testing", G_TYPE_BIT, (Pointer) EV_SystemEvent},
	{0,0,0}};

	// defult operands
	const  Triple G_null_graph = {"_",'_',0};
	int init_operands() {
		int i; 
		G_memset(operands,0,sizeof(operands));
		for(i=SystemUser;i < OperatorMaximum;i++) {
			operands[i].handler = event_handler;
			if(G_isugly(i) )
				operands[i].properties = EV_Ugly;
			else
				operands[i].properties= 0; 
		}
	i = install_sql_script("select '_',98,0;",SystemNull);
		return(i);
	}

	// Three loops that independent processes can run in
	void netio_loop() {G_printf("No network\n");}
	void debug_loop() {
	//Console c;
	Triple t;
	int status;
	debug_loop();
	for(;;) {
		t=G_null_graph;
		if(set_ready_event(0) & EV_SystemEvent)
			t.link = '@';
		status = triple(&t,event_handler);
	}
}
void console_loop() {
	Console c;
	Triple t;
	int status;
	for(;;) {
		G_console(&c);
		t.link = 
			(OperatorConsole | OperatorMSB); // console overload
		t.key = c.base;
		status = triple(&t,event_handler);
	}
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
    net_start();
		op = operands[GCHAR];
		print_trios();
  }
int main_engine(int argc, char * argv[]) {
		// Main loop
		if(argv[2] && !G_strcmp(argv[2], "-c")) 
			console_loop();
		else if(argv[2] && !G_strcmp(argv[2], "-debug")) // 
			debug_loop();
		 else if(argv[2] && !G_strcmp(argv[2], "-netio"))
			netio_loop();
		return(0);
	}
	void print_triple(Triple *t) { 
		G_printf(" %s %d %d\n",t->key,t->link,t->pointer);}
	// his is a little debgger, and stays ith this file
	Handler g_debugger(Triple *t) {
		print_triple(t);

		return 0;
	}
