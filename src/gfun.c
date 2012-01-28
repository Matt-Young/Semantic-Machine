
#include "all.h"
#include "filter.h"
const Triple NULL_Triple={"_",'_',0};

Mapper filter_map(Pointer * pointer,int * type) {
*pointer = (void *) &null_filter;
*type = 6;
return 0;
}

//  **  READY FOR RUNNING ******
typedef struct {int start;int row;int end;} RowSequence;
typedef struct {
  int count;
  RowSequence self;
  RowSequence other;
  RowSequence result;
  char * table_name;
  FILTER *filter;
  int  events;
  Code stmt;
  int opid;
}  READYSET;

READYSET ready;
Mapper map_self_row(Pointer * p,int *type) {
	*p = (Pointer) ready.self.row;
	*type = G_TYPE_INTEGER;
	return 0;
	}
Mapper map_self_start(Pointer * p,int *type) {
	*p = (Pointer) ready.self.start;
	*type = G_TYPE_INTEGER;
	return 0;
	}
Mapper map_other_row(Pointer * p,int *type) {
	*p = (Pointer) ready.other.row;
	*type = G_TYPE_INTEGER;
	return 0;
	}
Mapper map_other_start(Pointer * p,int *type) {
	*p = (Pointer) ready.other.start;
	*type = G_TYPE_INTEGER;
	return 0;
	}

	// Selecting an  installed sql statement
// This is called afer it is determined an installed sql
// it is always bound to the table context

FILTER * ready_filter() { return ready.filter;}

// msthods on graph pointers
int stopped_row() {
if(ready.self.row == ready.self.end)
  return 1;
return 0;
}
int incr_row(int delta) {
  ready.self.row+= delta;return(ready.self.row);
}
int _row() {return(ready.self.row);}
int set_row(int ivar) {
  ready.self.row = ivar;
  return(ready.self.row);
}

int reset_ready_set() {
  G_memset((void *) &ready,0,sizeof(ready));
  return 0;
}
void set_row_sequence(RowSequence * rows,PGRAPH f) {
	int rowid = offset_row(f);
	 rows->row += rowid;
	 rows->end += rowid;
	 rows->start += rowid;
}
int set_ready_event(int EV_event) {
	ready.events |= EV_event;
	return ready.events; }
Code set_ready_code(Code code,int opid) {
	ready.stmt = code;
	ready.opid = opid;
	return ready.stmt; }
int  set_ready_graph(FILTER *f) {
	PGRAPH active;
	G_memset(&ready,0,sizeof(ready));
	ready.filter = f;
	active = f->g[1];
	if(f->g[0]) active = f->g[0];
	ready.table_name = active->table->name;
	 set_row_sequence(&ready.self,active);  
  return  EV_Ok;
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
	else  if(f->g[0]->end > f->g[0]->row) 
		f->events |= EV_Incomplete;
	else if(f->g[1] && (f->g[1]->end  > f->g[1]->row)) 
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
	  child->event_triple  = &table->operators[pop_triple_operator];
	  new_table_graph(table);
	  child->g[0] = (PGRAPH) table->list;
	  status = set_ready_graph(child);
	  triple(child->event_triple,0);
	  delete_filter(child);
	  return status;
  }

int init_run_console(FILTER *f) {
	int status; TABLE *table;
	f->g[0] = init_parser();
	status= set_ready_graph(f);
	table =f->g[0]->table;
	f->event_table = table;
	f->event_triple = (Triple *) &G_null_graph;
	status=parser((PGRAPH *) &table->list);
		return status;
}

		/*
	f->event_table =  get_table_context("console");
	f->g[0] = (PGRAPH) new_table_graph(table);
	f->event_triple = &table->operators[pop_triple_operator];
	set_ready_graph(f); // run from console table
	ready.self.end=4;
	status = triple(f->event_triple,event_handler);
	release_table_context(f->event_table );
	*/
int event_exec(FILTER * f) {
	int g_event;
	Triple t;
	g_event = f->events;
	g_event |=  set_ready_event(0);
	if(g_event & EV_Overload) {
		if((ready.opid & OperatorMask) == OperatorConsole) {
			g_event = machine_triple(ready.stmt,&t);
			print_triple(&t);
			g_event = init_run_console(f);
		} else 
			if ((ready.opid & OperatorMask) == 1) {}
	}if(g_event & EV_Null) {
		g_event = machine_triple(ready.stmt,&t);
		print_triple(&t);
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
	f->events = ready.events;
	if(f->events & EV_Null)
	  event_exec(f);
	else if(f->events & EV_Square)  {
		if(f->g[0]->table->attribute == TABLE_SQUARE)  do_square(0,f);
		else if(f->g[1]->table->attribute == TABLE_SQUARE) do_square(1,f);
		} else
    event_exec(f);
 
  return 0;
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
	 machine_result_int(context, ready.self.row);
    if(x == ready.self.row) 
      if(ready.self.row != ready.self.end)
        ready.self.row++;
    break;
  case CallbackOther:
    machine_result_int(context, ready.other.row);
    if(x == ready.other.row) 
      if(ready.other.row != ready.other.end)
        ready.other.row++;
    break;
  case CallbackResult:
    machine_result_int(context, ready.result.row);
    break;
  case CallbackExperiment:
    ready.self.row++;
    machine_result_int(context, 1);
    break;
  default:
    G_printf("gfun: %d %d\n",x,ready.self.row);
    machine_result_int(context, 0);
  }
}
Trio gfun_accessor_list[] = {
	{ "Filter",G_TYPE_MAPPER, (Mapper) filter_map},
	{"BindSelfRow",G_TYPE_MAPPER,(Mapper) map_self_row},
	{"BindSelfStart",G_TYPE_MAPPER,(Mapper) map_self_start}, 
	{"BindOtherRow",G_TYPE_MAPPER,(Mapper) map_other_row},
	{"BindOtherStart",G_TYPE_MAPPER,(Mapper) map_other_start}, 	
	{0,00,} };
int init_gfun() {
	add_trios(gfun_accessor_list);
	ready.filter= &null_filter;
	return 0;
}