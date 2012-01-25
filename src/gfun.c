
#include "all.h"
#include "filter.h"
const Triple NULL_Triple={"_",96,0};

PGRAPH  set_ready_graph(FILTER *f) ;
Mapper filter_map(Pointer * pointer,int * type) {
*pointer = (void *) &null_filter;
*type = 6;
return 0;
}

//  **  READY FOR RUNNING ******
typedef struct {
  int count;
  PGRAPH self;
  PGRAPH other;
  PGRAPH result;
  FILTER *filter;
  int  events;
  Code stmt;
}  READYSET;

READYSET ready;
Mapper map_self_row(Pointer * p,int *type) {
	if(ready.self)
		*p = (Pointer) ready.self->row;
	*type = G_TYPE_INTEGER;
	return 0;
	}
Mapper map_self_start(Pointer * p,int *type) {
	if(ready.self)
		*p = (Pointer) ready.self->start;
	*type = G_TYPE_INTEGER;
	return 0;
	}
Mapper map_other_row(Pointer * p,int *type) {
	if(ready.other)
		*p = (Pointer) ready.other->row;
	*type = G_TYPE_INTEGER;
	return 0;
	}
Mapper map_other_start(Pointer * p,int *type) {
	if(ready.other)
		*p = (Pointer) ready.other->start;
	*type = G_TYPE_INTEGER;
	return 0;
	}

	// Selecting an  installed sql statement
// This is called afer it is determined an installed sql
// it is always bound to the table context

FILTER * ready_filter() { return ready.filter;}
TABLE * self_table() { return ready.self->table;}
GRAPH * self_graph(){ return (GRAPH *) ready.self->table->list;}
GRAPH * other_graph(){ return (GRAPH *) ready.other->table->list;}
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
int set_row(int ivar) {
  ready.self->row = ivar;
  return(ready.self->row);
}

int reset_ready_set() {
  G_memset((void *) &ready,0,sizeof(ready));
  return 0;
}
int set_ready_event(int EV_event) {
	ready.events |= EV_event;
	return ready.events; }

int  run_ready_graph(FILTER *f) {
	G_memset(&ready,0,sizeof(ready));
	ready.filter = f;
  if(f->g[0]) 
	 ready.self = f->g[0];
  else if(f->g[1]) 
	 ready.other = f->g[1];
  return  triple(f->event_triple,0);
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
  int g_event=0;
  if(!f->g[0])
   g_event |= EV_Null;
  else  if(f->g[0]->end > f->g[0]->row) 
		g_event |= EV_Incomplete;
  else if(f->g[1])
	 if(f->g[1]->end  > f->g[1]->row) 
		g_event |= EV_Incomplete;
  g_event |= set_ready_event(0);
 //if((f->properties & EV_Matchable) && key_match(f->
  return (g_event);
}

// sql table name will cuse events overned by filter paerent->child
 int init_run_table(FILTER * parent,char * name) {
	  TABLE * table; int status;
	  FILTER * child = new_filter(parent);
	  init_table(name,0,&table);
	  child->event_table=table;
	  child->event_triple  = &table->operators[pop_triple_operator];
	  new_table_graph(table);
	  child->g[0] = (PGRAPH) table->list;
	  status = run_ready_graph(child);
	  delete_filter(child);
	  return status;
  }

int init_run_console(FILTER *f) {
	int status; TABLE *table;
	f->g[0] = init_parser();
	table =f->g[0]->table;
	f->event_table = table;
	status=parser((PGRAPH *) &table->list);
	f->event_triple = &table->operators[pop_triple_operator];
	status= run_ready_graph(f); // run from concole table
	return status;
}
int event_exec(FILTER * f) {
  int g_event = f->properties;
  switch(g_event) {
  case EV_Null:
	  g_event = init_run_console(f);
     break;
  case EV_Debug:
	  g_event = init_run_table(f,"config");
	break;
  default:
	  g_event |=events(f);
	  break;
  }
    return(g_event);
  }
 void reset_G_columns(TABLE *t) { 
   t->info.col_count = 0;
 }
 int do_square(int mode,FILTER *f);
 
// Arrive here when some operaors has produced events
int event_handler(Triple * t) {
	FILTER *f;
	f = ready.filter;
	f->properties |= operands[t->link].properties;
	if(f->properties & EV_Null)
	  event_exec(f);
	else if(f->properties & EV_Square)  {
		if(f->g[0]->table->attribute == TABLE_SQUARE)  do_square(0,f);
		else if(f->g[1]->table->attribute == TABLE_SQUARE) do_square(1,f);
		} else
    event_exec(f);
 
  return 0;
    }
 
//G function call backs from inside sql
#define SELF_ROW 0
#define OTHER_ROW 1
#define RESULT_ROW 2
#define GET_NEXT_ROW 3
#define SET_NEXT_ROW 4
#define GET_ROW 5
#define SET_ROW 6
#define SET_LINK 7
void gfunction(Pointer context,int n, Pointer* v) {

  int op = machine_value_int(v[0]);
  int x = machine_value_int(v[1]);
  //printf("gfun: %d %d\n",x,m.self_row);
  switch(op) {
  case SELF_ROW:
	 machine_result_int(context, ready.self->row+1);
    if(x == ready.self->row+1) 
      if(ready.self->row+1 != ready.self->end)
        ready.self->row++;
    break;
  case OTHER_ROW:
    machine_result_int(context, ready.other->row+1);
    if(x == ready.other->row+1) 
      if(ready.other->row+1 != ready.other->end)
        ready.other->row++;
    break;
  case RESULT_ROW:
    machine_result_int(context, ready.result->row+1);
    break;
  case GET_NEXT_ROW:
    machine_result_int(context, ready.result->row);
    break;
  case SET_NEXT_ROW:
    machine_result_int(context, ready.result->row);
    ready.self->end = x;
    break;
  case SET_LINK:
    //current_graph->link = x;
    ready.self->row++;
    //current_graph->opclass = operands[x].properties;
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
	{"BindOtherRow",G_TYPE_MAPPER,(Mapper) map_self_row},
	{"BindOtherStart",G_TYPE_MAPPER,(Mapper) map_self_start}, 	
	{0,00,} };
int init_gfun() {
	add_trios(gfun_accessor_list);
	ready.filter= &null_filter;
	return 0;
}