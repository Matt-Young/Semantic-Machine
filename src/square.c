#include "../include/sqlite3.h"
#include "g.h"
#include "graphs.h"
extern OP operands[];
extern M m;
extern TABLE tables[];
sqlite3_stmt * Statement;
// square table adapter

int bind_schema(sqlite3_stmt *stmt,TRIPLE top);
/*
int new_filter_graph(TRIPLE t) {
	PGRAPH schema;
  schema_graph->match_state = G_SCHEMA;
  schema_graph = dup_graph(schema_graph,ready.self);
  schema_graph->end = t.pointer;
  schema_graph->start = ready.self->row;
  pass_parent_graph(schema_graph);
  //if(ATTRIBUTE(current_graph->table->atrribute) == G_SQUARE) 
  //  bind_schema(operands[t.link].stmt,t);
  return SQLITE_OK;
}
*/
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

TRIPLE column_decoder(COLINFO *c) {
	TRIPLE s={0,0,0};
      s.key = (void *) sqlite3_column_text(Statement,c->index);
      c->index++;
      s.pointer++;
	  s.link = 0;
      return(s);
  }
// defaut grammar is to descend a row with the default Dot
int do_square(int mode,FILTER *f) {
COLINFO *c,*d;
TRIPLE ct,dt;
    c = init_col_info(f->g[0]->table);
    while(c->index < c->col_count) {
		ct = column_decoder(c); 
      if(f->g[1]->table->attribute == G_SQUARE) { 
        d = init_col_info(f->g[1]->table);
        while(d->index < d->col_count) {
			dt = column_decoder(d); 
			f->properties = operands[dt.link].properties;
           event_exec(f);
          }
	  }
	  else 
		  event_exec(f);
	  }
	return 0;
      }

