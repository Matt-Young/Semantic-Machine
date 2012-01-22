
#include "../include/sqlite3.h"
#include "g.h"
#include "graphs.h"
extern OP operands[];
extern M m;
extern TABLE tables[];
PGRAPH schema_graph=0;
extern TABLE tables[];
const TRIPLE NULL_TRIPLE={"_",96,0};
// 
PGRAPH other(PGRAPH  g);
int counter=0;


// Null is alwys operational
FILTER null_filter;
PGRAPH  set_ready_graph(FILTER *f) ;
int init_gfun() {
	null_filter.g[0]=0;
	null_filter.g[1]=0;
	operands['_'].properties= EV_Null;
	set_ready_graph(&null_filter);
	return 0;
}
FILTER *filter_list=&null_filter;
int newfilter=0;
int oldfilter=0;
FILTER * new_filter() {
    FILTER * f = (FILTER *) G_calloc(sizeof(FILTER));
    newfilter++;
    if(filter_list)
        f->prev = filter_list;
      else
        filter_list = f;
    if(f->prev) {
      f->status =  f->prev->status;
      }
    f->status |= G_SELECTED + G_CONTINUE;
    return(f);
  }
FILTER *f;


FILTER *delete_filter(FILTER * f) {
  FILTER * g;
  if(oldfilter >= newfilter) 
    G_error("Filter",G_ERR_FILTER);
  g = (FILTER *) f->prev; 
  oldfilter++; 
  G_free( (void *) f);
  return g;
  }
FILTER * close_filter(FILTER * f) {
  delete_graph((PGRAPH *) f->g[0]->table->list);
  delete_graph((PGRAPH *) f->g[1]->table->list);
  return delete_filter(f);
}



sqlite3_stmt *Statement;
//  **  READY FOR RUNNING ******
typedef struct {
  int count;
  PGRAPH self;
  PGRAPH other;
  PGRAPH result;
  FILTER *filter;
  TRIPLE input_node;
  sqlite3_stmt * stmt;
}  READYSET;
READYSET ready;
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
PGRAPH  set_ready_graph(FILTER *f) {
	G_memset(&ready,0,sizeof(ready));
	ready.filter = f;
  if(f->g[0]) 
	 ready.self = f->g[0];
  if(f->g[1]) 
	 ready.other = f->g[1];
  return ready.self;
}
int dispatch() {
  FILTER * g;
  PGRAPH graph;
//next ready filter
  g = f;
  do {
    if(g->status == G_UNREADY) 
      break;
    g = g->prev;
  }while(g);
  if(g) {
    graph = set_ready_graph(g);

  }
  triple(&graph->table->operators[pop_triple_operator],0);
  return(SQLITE_OK);
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
 if(f->g[0]->end > f->g[0]->row) 
    g_event |= EV_Incomplete;
 if(f->g[1]->end  > f->g[1]->row) 
    g_event |= EV_Incomplete;
 //if((f->properties & EV_Matchable) && key_match(f->
  return (g_event);
}

extern PGRAPH init_parser(char *);
int event_exec(FILTER * f) {
  int g_event =f->properties;
  switch(g_event) {
  case EV_Null:
	  null_filter.g[0] = init_parser("console");
	  set_ready_graph(&null_filter);
      g_event |= parser();
	 //g_event |= set_ready_graph(&null_filter);
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
 
// If the result lands here, the query is done
// go search the filter list for something to do
// otr else
// we are getting a row from some square table.
// then we take the row and run the machine triplet/column mode
// top is the initiationg query, in most cases, the originating triplet.
// Normal action, find an event to call this, then go up the filter chain, 
// srtarting with the calling triplet
int event_handler(TRIPLE t) {
	FILTER *f;
  f = ready_filter();
  f->properties = operands[t.link].properties;
  if(f->properties & EV_Null)
	  event_exec(f);
  else if(f->properties & EV_Square)  {
	if(f->g[0]->table->attribute == G_SQUARE)  do_square(0,f);
	else if(f->g[1]->table->attribute == G_SQUARE) do_square(1,f);
  } else
    event_exec(f);
 
  return 0;
    }
 //int output_filter(TRIPLE t) {
 //return handler(ready.filter);
//}
//G function call backs from inside sql
#define SELF_ROW 0
#define OTHER_ROW 1
#define RESULT_ROW 2
#define GET_NEXT_ROW 3
#define SET_NEXT_ROW 4
#define GET_ROW 5
#define SET_ROW 6
#define SET_LINK 7
void gfunction(sqlite3_context* context,int n,sqlite3_value** v) {

  int op = sqlite3_value_int(v[0]);
  int x = sqlite3_value_int(v[1]);
  //printf("gfun: %d %d\n",x,m.self_row);
  switch(op) {
  case SELF_ROW:
    sqlite3_result_int(context, ready.self->row+1);
    if(x == ready.self->row+1) 
      if(ready.self->row+1 != ready.self->end)
        ready.self->row++;
    break;
  case OTHER_ROW:
    sqlite3_result_int(context, ready.other->row+1);
    if(x == ready.other->row+1) 
      if(ready.other->row+1 != ready.other->end)
        ready.other->row++;
    break;
  case RESULT_ROW:
    sqlite3_result_int(context, ready.result->row+1);
    break;
  case GET_NEXT_ROW:
    sqlite3_result_int(context, ready.result->row);
    break;
  case SET_NEXT_ROW:
    sqlite3_result_int(context, ready.result->row);
    ready.self->end = x;
    break;
  case SET_LINK:
    //current_graph->link = x;
    ready.self->row++;
    //current_graph->opclass = operands[x].properties;
    sqlite3_result_int(context, 1);
    break;
  default:
    G_printf("gfun: %d %d\n",x,ready.self->row);
    sqlite3_result_int(context, 0);
  }
}
