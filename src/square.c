#include "../src/include/g_types.h"
#include "./include/machine.h"
#include "../src/include/tables.h"

extern TABLE tables[];
Code  Statement;
// square table adapter

ColInfo *init_col_info(TABLE * t) {
  if(!t->info.col_count) 
    machine_name_info(t->stmt,&t->info);
  t->info.index = 0;
  return(&t->info);
}

Triple column_decoder(TABLE *t,ColInfo *c) {
	Triple s={0,0,0};
      s.key =  (char *) machine_column_text(t->stmt,c->index);
      c->index++;
      s.pointer++;
	  s.link = '.';
      return(s);
  }
int event_exec(Triple *);
// defaut grammar is to descend a row with the default Dot
int do_square(int mode,TABLE *table) {
ColInfo *c,*d;
Triple ct,dt;
    c = init_col_info(table);
    while(c->index < c->col_count) {
		ct = column_decoder(table,c); 
      if(table->attribute == TABLE_SQUARE) { 
        d = init_col_info(table);
        while(d->index < d->col_count) {
			dt = column_decoder(table,d); 
           event_exec(&ct);
          }
	  }
	  else 
		  event_exec(&dt);
	  }
	return 0;
      }

