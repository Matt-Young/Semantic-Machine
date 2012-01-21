

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
PGRAPH other(PGRAPH  g) {
  if(g != tables[2].list)
    return (PGRAPH) tables[G_TABLE_SELF].list;
  else
    return (PGRAPH) tables[G_TABLE_OTHER].list;
}
int counter=0;
typedef struct f { 
  TRIPLE t[2];
  PGRAPH g[2];
  TRIPLE sum;
  TRIPLE caller;
  int status;
  struct f* prev;
} FILTER;

FILTER *filter_list=0;
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

void * start_select() {
FILTER *f = new_filter();
f->g[0] = *LIST(0);
f->status = G_UNREADY;
return (void *) f;
};
FILTER *delete_filter(FILTER * f) {
  FILTER * g;
  if(oldfilter >= newfilter) 
    gerror("Filter",G_ERR_FILTER);
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
// doing the full schema thingie
const char mod_keys[] = "!:*?";
char isin(const char * str,char ch)
{while(*str && ((*str) != ch)) str++; return *str;}
int deliver_output(TRIPLE t) {
  PGRAPH output = *LIST(m.output);
  append_graph(&output,t);
  return SQLITE_OK;
}
// dispatch on sqlite done
int insert_opcode(FILTER * );
PGRAPH  set_ready_graph(FILTER *f); 
// called when sqlite_done asses through to
// the primary caller in triple s
FILTER * f;

sqlite3_stmt *Statement;

typedef struct { 
  char g_event;
  char g_action[3];  
 } OPCODE; 
typedef struct {
  int count;
  PGRAPH self;
  PGRAPH other;
  PGRAPH result;
  FILTER *filter;
  int pc;
  OPCODE opset[4];
  TRIPLE input_node;
}  READYSET;
READYSET ready;
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
//G function call backs from inside sql
#define SELF_ROW 0
#define OTHER_ROW 1
#define RESULT_ROW 2
#define GET_NEXT_ROW 3
#define SET_NEXT_ROW 4
#define GET_ROW 5
#define SET_ROW 6
#define SET_LINK 7

int add_opcode(char a,char b,char c,char d) {
  OPCODE *op;
  op = &ready.opset[ready.pc++];
  op->g_event = a;
  op->g_action[0] = b;
  op->g_action[1] = c;
  return 0;
}

int reset_ready_set() {
  G_memset((void *) &ready,0,sizeof(ready));
  return 0;
}
PGRAPH  set_ready_graph(FILTER *f) {
  if(!f->g[0])
    gerror("Ready",97);
  if(f->g[0]->match_state != G_READY) {
    ready.self = f->g[0];
    ready.other = f->g[1];
  } else if(!f->g[1])
    gerror("Ready",98);
  else { 
    ready.self =f->g[1];
    ready.other =f->g[0];}
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
    do {  
      insert_opcode(g);
      g = g->prev;
    } while(g);
  }
  triple(graph->table->pop_triple,0);
  return(SQLITE_OK);
}
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
    sqlite3_result_int(context, other(ready.other)->row+1);
    if(x == other(ready.other)->row+1) 
      if(other(ready.other)->row+1 != other(ready.other)->end)
        other(ready.other)->row++;
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
// micro ops in opcode, this is a confused mess.
#define OP_Print  0 // match operands 
#define OP_Console 1     
#define OP_Subquery 2
//states
#define STATE_Collect  0// select and repeat?
#define STATE_Unready  G_UNREADY
// events
#define EV_Null 0x01
#define EV_Wild_Triple 0x02
#define EV_Sql_Done EV_Null

int insert_opcode(FILTER *f) {
    switch(f->caller.link) {
      case '_':
        add_opcode(EV_Null,STATE_Unready,OP_Console,OP_Console);
        break;
      default:
        add_opcode(EV_Wild_Triple,STATE_Collect,OP_Print,0);
        break;
    }
    return 0;
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
  int g_event;
 if(f->t[1].pointer > f->g[1]->row) {
    g_event |= EV_Wild_Triple;
  if(f->t[0].link == '*')
    g_event |= EV_Wild_Triple;
  return (g_event);
 }
  return EV_Wild_Triple;
}
int default_sub_query() {
  FILTER *f = new_filter();
  f->g[0] = *LIST(G_TABLE_SELF);
  f->g[1] = *LIST(G_TABLE_OTHER);
  f->status = G_UNREADY;
  return 0;
}
int event_exec(int g_event) {
  int status = SQLITE_OK;
  int index = ready.pc;
  char  *ptr;
  do {
    if(ready.opset[index].g_event & g_event) 
      // we have an opcode for the event
      ptr = ready.opset[index].g_action; 
      while(*ptr) {
        switch(*ptr) {
          case OP_Print:
            print_triple(ready.filter->t[0]);
            print_triple(ready.filter->t[1]);
            break;
          case OP_Console:
            status = parser();
			 set_ready_graph();
            break;
          case OP_Subquery:
            status = default_sub_query();
            break;
        }
     ptr++;
    }
    index--;
  } while(index);
    return(status);
  }
 void reset_G_columns(TABLE *t) { 
   t->info.col_count = 0;
 }

 void sql_column_info(sqlite3_stmt * stmt, COLINFO *cinfo) {
  int i;
  cinfo->col_count = sqlite3_column_count(stmt);
  for(i=0;i< cinfo->col_count;i++) {
    cinfo->name[i] = sqlite3_column_name( stmt, i);
    cinfo->type[i] = sqlite3_column_type( stmt, i);
    if(i == 3)
      cinfo->rowid = sqlite3_column_int(stmt, i);
  }
}  
COLINFO *init_col_info(TABLE * t) {
  if(!t->info.col_count) 
    sql_column_info(Statement,&t->info);
  t->info.index = 0;
  return(&t->info);
}

int column_decoder(COLINFO *c,TRIPLE s) {
      s.key = sqlite3_column_text(Statement,c->index);
      c->index++;
      s.pointer++;
      return(1);
  }
// If the reult lands here, the query is done
// go search the filter list for something to do
// otr else
// we are getting a row from some square table.
// then we take the row and run the machine triplet/column mode
// top is the initiationg query, in most cases, the originating triplet.
int handler(TRIPLE t) {
  COLINFO *c;
  COLINFO *d;
  int g_event=0;
  if(f->g[0]->table->attribute == G_SQUARE) {
    c = init_col_info(f->g[0]->table);
    while(column_decoder(c,f->t[0])) {
      if(f->g[1]->table->attribute == G_SQUARE) { 
        d = init_col_info(f->g[1]->table);
        while(column_decoder(d,f->t[1])) {
           g_event = events(f);
           event_exec(g_event);
          }
      }
      else
        event_exec(g_event);
      }
    }
  else if(f->g[1]->table->attribute == G_SQUARE) {
      d = init_col_info(f->g[1]->table);
        while(column_decoder(d,f->t[1])) {
          g_event = events(f);
          event_exec(g_event);
       }
      } else
        event_exec(g_event);
      return g_event;
  }
 int output_filter(TRIPLE t) {
 return handler(ready.filter);
}

int bind_schema(sqlite3_stmt *stmt,TRIPLE top);
int new_filter_graph(FILTER *f,TRIPLE t) {
  schema_graph->match_state = G_SCHEMA;
  schema_graph = dup_graph(schema_graph,ready.self);
  schema_graph->end = t.pointer;
  schema_graph->start = ready.self->row;
  pass_parent_graph(schema_graph);
  //if(ATTRIBUTE(current_graph->table->atrribute) == G_SQUARE) 
  //  bind_schema(operands[t.link].stmt,t);
  return SQLITE_OK;
}
