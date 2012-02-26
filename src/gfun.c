
//#include "../src/include/config.h"
#include "../src/include/g_types.h"
#include "../src/include/config.h"
#include "../src/include/names.h"
#include "./include/machine.h"
#include "../src/include/tables.h"
#include "../src/include/graphs.h"
#include "../src/include/engine.h"
#include "../src/include/console.h"
#include "./include/qson.h"
Triple _null_graph={'_',0,"_"};
       int print_triple(Triple *);
       int parser(char *,TABLE *);
//  **  READY FOR RUNNING ******
typedef struct ReadySet {
  int count;
  RowSequence *self;
  RowSequence *other;
  RowSequence *result;
  int  events;
  Code stmt;
  int opid;
  Triple *buff; // an output
  IO_Structure *return_addr;
  TABLE *table;
  char  *any_name;
}  Readyset;

Readyset ready;
Readyset * ready_stack[8];int ready_index=0;
int push_ready(){
  ready_stack[ready_index] = (Readyset *) G_malloc(sizeof(Readyset));
  *ready_stack[ready_index] = ready;
  ready_index ++; ready_index &= 0x07; 
  return 0;
}
int pop_ready(){
  ready_stack[ready_index] = (Readyset *) G_malloc(sizeof(Readyset));
  ready_index --; ready_index &= 0x07; 
  //ready = *ready_stack[ready_index];
  G_free(ready_stack[ready_index]);
  return 0;
}
Mapper map_self_row(Pointer * p,int *type) {
	*p = (Pointer) (ready.self->row + ready.self->rowoffset);
	*type = G_TYPE_INTEGER;
	return 0;
	}
Mapper map_relative_self_row(Pointer * p,int *type) {
	*p = (Pointer) (ready.self->row);
	*type = G_TYPE_INTEGER;
	return 0;
	}
Mapper map_self_offset(Pointer * p,int *type) {
	*p = (Pointer) (ready.self->rowoffset);
	*type = G_TYPE_INTEGER;
	return 0;
	}
Mapper map_other_row(Pointer * p,int *type) {
	*p = (Pointer) (ready.other->row+ ready.other->rowoffset);
	*type = G_TYPE_INTEGER;
	return 0;
	}
Mapper map_other_offset(Pointer * p,int *type) {
	*p = (Pointer) (ready.other->rowoffset);
	*type = G_TYPE_INTEGER;
	return 0;
	}
Mapper map_return_addr(Pointer * p,int *type) {
	*p = (Pointer) ready.return_addr;
	*type = G_TYPE_ADDR;
	return 0;
	}

	// Methods on the ready set
int stopped_row() {
if(ready.self->row == ready.self->end)
  return 1;
return 0;
}
int incr_row(int delta) {
  ready.self->row+= delta;return(ready.self->row);
}
int _row() {return(ready.self->row);}
Triple * set_output_buff(Triple *t) {
  if(t)
  ready.buff = t;
  return(ready.buff);
}
int set_row_sequence(RowSequence *r) {
  ready.self = r;
  return(ready.self->row);
}

int reset_ready_set() {
  G_memset((void *) &ready,0,sizeof(ready));
  return 0;
}
Code get_ready_stmt() {
  return ready.stmt;
}
TABLE * get_ready_table() {
  return ready.table;
}
int set_ready_event(int EV_event) {
	ready.events |= EV_event;
	return ready.events; }
int reset_ready_event(int EV_event) {
  int events=ready.events;
	ready.events &= ~EV_event;
	return events; }
int set_ready_code(int opid) {
	ready.opid = opid;
	return ready.opid; }
Code set_ready_stmt(Code stmt) {
	ready.stmt = stmt;
	return ready.stmt; }
int  set_ready_graph(TABLE * t,IO_Structure * to) {
  G_memset(&ready,0,sizeof(ready));
  ready.table=t;
  ready.return_addr = to;
  // ready.return_addr->sa_family = AF_CONSOLE;
  // ready.return_addr->format=Json_IO;
  if(t) {
    ready.self = &((PGRAPH) t->list)->rdx;
    ready.self->end = -1;
  }
  return  EV_Ok;
}
void * set_io_struct(IO_Structure *w) {
  ready.return_addr=w;
  return ready.return_addr;
}
IO_Structure * get_io_stuct() {
  return ready.return_addr;
}
int key_match(const char * k,const char * g) {
  int klen = G_strlen(k);
  int glen = G_strlen(g);
  if((klen == glen) && !G_strcmp(k,g))
    return 1;
  else
    return 0;
}


 // The main io graph_changess initialize and run here
 int send_buff(char *buffer,int count,void * ip_addr);
 int put_qson_graph(TABLE *table,Triple *Qson) ;
 int spew_qson(Triple *t) {
   //put_qson_graph(ready.table,t);
   return 0;
 }
int get_qson_graph(Triple *t) ; 
int consume_qson(Triple *t) {
    TABLE * table;
  	init_table("scratch",0,&table);
    // get_qson_graph(t);
     return EV_Ok;
}
int echo_handler(Triple *);
int init_run_json(Triple *triple) {
	int status;TABLE * table;
  G_printf("Json  \n");
 init_table("console",0,&table);
	status= set_ready_graph(table,0);
  G_printf("Begin Json\n");
	status=parser(triple->key,table);
//  run_table(table,0);
 G_printf("\nJson done\n");
		return status;
}
// These are realy over loads, not part of the engine proper, 
// nor part of the Ugly Set.  
int event_handler(Triple * t) {
  int g_event;
  int linkid = t->link;
  g_event =  ready.events;
  if(g_event & EV_Done) { // Nothing here but missing code
    G_printf("Unhandled EV_DONE\n");
    reset_ready_event(EV_Done);
    return g_event;
  }
   else if(g_event & EV_Overload) {
    if(ready.opid  == (OperatorJson & OperatorMask))  
      g_event |= init_run_json( t);
    else if (ready.opid  == (OperatorBsonIn & OperatorMask)) 
      g_event |= consume_qson(t);
    else if (ready.opid  == (OperatorBsonOut & OperatorMask)) 
     // g_event |= spew_qson(f->event_triple);
    reset_ready_event(EV_Overload);
  }
  if(g_event & EV_Null) {
    reset_ready_event(EV_Null);
  }
  return(g_event);
}

int do_square(int mode,TABLE *);


//G function call backs from inside sql
enum {CallbackSelf,CallbackOther,CallbackResult,CallbackExperiment};
void gfunction(Pointer context,int n, Pointer v[]) {
	// graph pointers already mapped to machine rows
  int op = machine_value_int(v[0]);
  int x = machine_value_int(v[1]);
  //printf("gfun: %d %d\n",x,m.self_row);
  switch(op) {
  case CallbackSelf:
	 machine_result_int(context, ready.self->row + ready.self->rowoffset);
    if(x == ready.self->row + ready.self->rowoffset) 
      if(ready.self->row != ready.self->end)
        ready.self->row++;
    break;
  case CallbackOther:
    machine_result_int(context, ready.other->row + ready.other->rowoffset);
    if(x == ready.other->row + ready.other->rowoffset) 
      if(ready.other->row != ready.other->end)
        ready.other->row++;
    break;
  case CallbackResult:
    machine_result_int(context, ready.result->row);
    break;
  case CallbackExperiment:
    ready.self->row++;
    machine_result_int(context, 1);
    break;
  default:
    G_printf("gfun: %d %d\n",x,ready.self->row);
    machine_result_int(context, 0);
  }
}
Trio gfun_accessor_list[] = {
	{"BindSelfRow",G_TYPE_MAPPER,(Mapper) map_self_row},
	{"BindSelfOffset",G_TYPE_MAPPER,(Mapper) map_self_offset}, 
	{"BindOtherRow",G_TYPE_MAPPER,(Mapper) map_other_row},
	{"BindOtherOffset",G_TYPE_MAPPER,(Mapper) map_other_offset},
  {"BindRelativeSelfRow",G_TYPE_MAPPER,(Mapper)  map_relative_self_row},
  {"BindReturnAddr",G_TYPE_ADDR,(Mapper) map_return_addr},
	{0,00,} };
int init_gfun() {
	add_trios(gfun_accessor_list);
	return 0;
}
// Uglies get their own direct handler
//Trio * find_name(char * key) 
Handler get_handler(int opid,Handler handler) ;
int ugly_handler(Triple *top){
  int linkid;Trio *s1;
  Triple v0,v1,v2;
  Code stmt = ready.stmt;   
  machine_triple(stmt, &v0);
  linkid =  v0.link;
  if(linkid == '@'){
   // V0@V1,V2
 
ready.any_name=new_string(v0.key);

		 s1 = 
        find_name( v0.key);
		if(s1->type == G_TYPE_SYSTEM)
        return get_handler((int) s1->value,0)(top);  // Call handler acquies V2
		else { // Dunno quite yet
    }

  }  else if(linkid == ':') {
      v1.key = new_string(v0.key);
      if(EV_Data &  machine_step(stmt) ) 
        machine_triple(stmt,&v2);
      else
        return 0;
        if(G_isdigit(v2.key[0])) 
          add_trio( 
          v1.key ,
          G_TYPE_USER,
          (void *) G_strtol(v2.key));
        else 
          add_trio( 
          new_string(top->key),
          G_TYPE_USER,
          find_name(v2.key));
      } else if(linkid == '$') {
        if(EV_Data &  machine_step(stmt) ) 
          machine_triple(stmt,&v2);
          top->link = 
            (int) find_trio_value( top->key);
          machine_new_operator(top,0);
      }  else 
        print_triple(&v0);
      reset_ready_event(EV_Ugly);
      return 0;
    }
