#include "../src/include/g_types.h"
#include "./include/machine.h"
#include "../src/include/tables.h"
#include "../src/include/engine.h"
#include "../src/include/console.h"
extern TABLE tables[];
Code  Statement;
// square table adapter

ColInfo *init_col_info(TABLE * t) {
  if(!t->info.col_count) 
    machine_row_info(t->stmt,&t->info);
  t->info.index = 0;
  t->attribute = TABLE_SQUARE;
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

// defaut grammar is to descend a row with the default Dot
int square_handler(Triple *t) {
ColInfo *c;TABLE * table; void * vals[8];
//Triple ct,dt; 
int i;
table = get_ready_table();
table->stmt=get_ready_stmt();

if(table->info.type[0] == G_TYPE_NONE)
    c = init_col_info(table);
G_printf("\n");
machine_unbind_row(table->stmt,&table->info,vals);
for(i=0;i < table->info.col_count;i++)
  if(table->info.type[i] != G_TYPE_INTEGER)
    G_printf("|%s|",(char *) vals[i]);
  else
G_printf("|%d|",(int) vals[i]);
/*
    while(c->index < c->col_count) {
		ct = column_decoder(table,c); 
      if(table->attribute == TABLE_SQUARE) { 
        d = init_col_info(table);
        while(d->index < d->col_count) {
			dt = column_decoder(table,d); 
           event_handler(&ct);
          }
	  }
	  else 
		  event_handler(&dt);
	  }
    */
	return 0;
      }

