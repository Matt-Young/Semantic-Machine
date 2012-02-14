
#include "all.h"
#include "filter.h"
#include "sqlson.h"
const Triple NULL_Triple={"_",'_',0};

Mapper filter_map(Pointer * pointer,int * type) {
*pointer = (void *) &null_filter;
*type = 6;
return 0;
}
//  **  READY FOR RUNNING ******
typedef struct {
  int count;
  RowSequence *self;
  RowSequence *other;
  RowSequence *result;
  char * table_name;
  FILTER *filter;
  int  events;
  Code stmt;
  int opid;
  Webaddr return_addr;
}  READYSET;

READYSET ready;
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
Mapper map_self_start(Pointer * p,int *type) {
	*p = (Pointer) (ready.self->rowoffset);
	*type = G_TYPE_INTEGER;
	return 0;
	}
Mapper map_other_row(Pointer * p,int *type) {
	*p = (Pointer) (ready.other->row+ ready.other->rowoffset);
	*type = G_TYPE_INTEGER;
	return 0;
	}
Mapper map_other_start(Pointer * p,int *type) {
	*p = (Pointer) (ready.other->rowoffset);
	*type = G_TYPE_INTEGER;
	return 0;
	}
Mapper map_return_addr(Pointer * p,int *type) {
	*p = (Pointer) (&ready.return_addr);
	*type = G_TYPE_ADDR;
	return 0;
	}

	// Selecting an  installed sql statement
// This is called afer it is determined an installed sql
// it is always bound to the table context

FILTER * ready_filter() { return ready.filter;}

// msthods on graph pointers
int stopped_row() {
if(ready.self->row == ready.self->end)
  return 1;
return 0;
}
int incr_row(int delta) {
  ready.self->row+= delta;return(ready.self->row);
}
int _row() {return(ready.self->row);}
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

int set_ready_event(int EV_event) {
	ready.events |= EV_event;
	return ready.events; }
int reset_ready_event(int EV_event) {
	ready.events &= ~EV_event;
	return ready.events; }
int set_ready_code(int opid) {
	ready.opid = opid;
	return ready.opid; }
Code set_ready_stmt(Code stmt) {
	ready.stmt = stmt;
	return ready.stmt; }
int  set_ready_graph(FILTER *f) {
	PGRAPH active;
	G_memset(&ready,0,sizeof(ready));
	ready.filter = f;
	active = f->g[1];
	if(f->g[0]) active = f->g[0];
  if(active) {
	  ready.table_name = active->table->name;
    ready.self = &active->rdx;
    } else ready.self=0;
  return  EV_Ok;
}
void set_web_addr(Webaddr *w) {
  ready.return_addr =  *w;
}
Webaddr * get_web_addr(Webaddr *w) {
  return &ready.return_addr;
}
int key_match(const char * k,const char * g) {
  int klen = G_strlen(k);
  int glen = G_strlen(g);
  if((klen == glen) && !G_strcmp(k,g))
    return 1;
  else
    return 0;
}
int events(FILTER * f) {
	if(!f->g[0])
		f->events |= EV_Null;
	else  if(f->g[0]->rdx.end > f->g[0]->rdx.row) 
		f->events |= EV_Incomplete;
	else if(f->g[1] && (f->g[1]->rdx.end  > f->g[1]->rdx.row)) 
			f->events |= EV_Incomplete;
	//if(isnull_filter(f))
	//	f->events |= EV_Null;
	return (f->events);
}

// sql table name will cuse events overned by filter paerent->child
 int init_run_table(FILTER * parent,char * name) {
	  TABLE * table; int status;
	  FILTER * child = new_filter_context(parent);
	  init_table(name,0,&table);
	  child->event_table=table;
	  child->initial_triple  = &table->operators[pop_triple_operator];
	  new_table_graph(table);
	  child->g[0] = (PGRAPH) table->list;
	  status = set_ready_graph(child);
	  triple(child->initial_triple,0);
	  delete_filter(child);
	  return status;
  }

 // The main io graph_changess initialize and run here
 int send_buff(char *buffer,int count,void * ip_addr);
int spew_bson(Triple *t) {
    TABLE * table; char * buff;int count;
  	init_table(t->key,0,&table);
    //machine_triple(&addr)_
     count = Sqlson_to_Bson(table->operators,&buff);
    // send_buff(buff,count,addr.key);
     G_free(buff);
     return EV_Ok;
}
 int spew_json(Triple *t) {
    TABLE * table; char * buff;int count;
  	init_table(t->key,0,&table);
    //machine_triple(&addr)_
     count = Sqlson_to_Json(table->operators,&buff);
    // send_buff(buff,count,addr.key);
     G_free(buff);
     return EV_Ok;
}
 
int consume_bson(Triple *t) {
    TABLE * table;
  	init_table("scratch",0,&table);
     Bson_to_Sqlson(table->operators,t->key);
     return EV_Ok;
}
int echo_handler(Triple *node);
int init_run_json(FILTER *f) {
	int status; RowSequence r; 
  G_printf("Json  \n");
  f->event_table = get_table_name("console");
	status= set_ready_graph(f);
	f->initial_triple = (Triple *) &G_null_graph;
	status=parser(f->event_triple->key,f->event_table);
  G_memset(&r,0,sizeof(r));
  r.rowoffset=1;
  r.end=-1;
  set_row_sequence(&r);
 f->initial_triple  = &f->event_table->operators[pop_triple_operator];
  G_printf("Begin\n");
 //status = machine_exec(g_db,"BEGIN IMMEDIATE;",&err);
 G_printf("\nParse start\n");
 triple(f->initial_triple,pop_handler);
 G_printf("\nParse done\n");


		return status;
}

int event_exec(FILTER * f) {
  int g_event;
  int linkid = f->event_triple->link;
  g_event = f->events; 
  g_event |=  ready.events;
  if(g_event & EV_Done) { // Nothing here but missing code
    G_printf("Unhandled EV_DONE\n");
    reset_ready_event(EV_Done);
    return g_event;
  }
   else if(g_event & EV_Overload) {
    if(ready.opid  == (OperatorJson & OperatorMask))  
      g_event |= init_run_json(f);
    else if (ready.opid  == (OperatorBsonIn & OperatorMask)) 
      g_event |= consume_bson(f->event_triple);
    else if (ready.opid  == (OperatorBsonOut & OperatorMask)) 
      g_event |= spew_bson(f->event_triple);
    reset_ready_event(EV_Overload);
  }
  if(g_event & EV_Null) {
    reset_ready_event(EV_Null);
  }
  return(g_event);
}
void reset_G_columns(TABLE *t) { 
  t->info.col_count = 0;
}
int do_square(int mode,FILTER *f);

// Arrive here when some operators has produced events
int event_handler(Triple * t) {
  FILTER *f;
  f = ready.filter;
  f->event_triple = t;
  f->events = ready.events;
  // G_printf("EX %x \n",f->events);
  if(f->events & EV_Square)  {
    if(f->g[0]->table->attribute == TABLE_SQUARE)  
      return do_square(0,f);
    else if(f->g[1]->table->attribute == TABLE_SQUARE) 
      return do_square(1,f);
  } else
		return event_exec(f);
	 return(f->events);
 }

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
	{ "Filter",G_TYPE_MAPPER, (Mapper) filter_map},
	{"BindSelfRow",G_TYPE_MAPPER,(Mapper) map_self_row},
	{"BindSelfStart",G_TYPE_MAPPER,(Mapper) map_self_start}, 
	{"BindOtherRow",G_TYPE_MAPPER,(Mapper) map_other_row},
	{"BindOtherStart",G_TYPE_MAPPER,(Mapper) map_other_start},
  {"BindRelativeSelfRow",G_TYPE_MAPPER,(Mapper)  map_relative_self_row},
  {"BindReturnAddr",G_TYPE_ADDR,(Mapper) map_return_addr},
	{0,00,} };
int init_gfun() {
	add_trios(gfun_accessor_list);
	ready.filter= &null_filter;
	return 0;
}

// Uglies get their own direct handler

int ugly_handler(Triple *top){
  int linkid;
  Code stmt = ready.stmt;   Triple v1,v2;
        v1= *top; 
        linkid =  v1.link;
        if(linkid == '@'){
         G_printf("Magic %s",v1.key);
        }
        else if(linkid == ':') {
          v1.key = new_string(top->key);
          if(EV_Data &  machine_step(stmt) ) {
            machine_triple(stmt,&v2);
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
          }
        } else if(linkid == '$') {
         if(EV_Data &  machine_step(stmt) ) {
            machine_triple(stmt,&v2);
            top->link = 
              (int) find_trio_value( top->key);
            triple(top,0);
         }
      }
        else {
    print_triple(top);
        }
    reset_ready_event(EV_Ugly);
    return 0;
      }
