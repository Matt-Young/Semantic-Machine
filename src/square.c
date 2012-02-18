#include "../include/sqlite3.h"
#include "all.h"


extern TABLE tables[];
Code  Statement;
// square table adapter

int bind_schema(Code stmt,Triple top);

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

Triple column_decoder(COLINFO *c) {
	Triple s={0,0,0};
      s.key = (void *) sqlite3_column_text(Statement,c->index);
      c->index++;
      s.pointer++;
	  s.link = 0;
      return(s);
  }
int event_exec(Triple *);
// defaut grammar is to descend a row with the default Dot
int do_square(int mode,TABLE *table) {
COLINFO *c,*d;
Triple ct,dt;
    c = init_col_info(table);
    while(c->index < c->col_count) {
		ct = column_decoder(c); 
      if(table->attribute == TABLE_SQUARE) { 
        d = init_col_info(table);
        while(d->index < d->col_count) {
			dt = column_decoder(d); 
           event_exec(&ct);
          }
	  }
	  else 
		  event_exec(&dt);
	  }
	return 0;
      }

