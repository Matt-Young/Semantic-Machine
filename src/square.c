#include "../src/include/config.h"
#include "../src/include/g_types.h"
#include "../src/include/machine.h"
#include "../src/include/tables.h"
#include "../src/include/qson.h"
#include "../src/include/engine.h"
#include "../src/include/console.h"
extern TABLE tables[];
Code  Statement;
// square table adapter

ColInfo *init_col_info(TABLE * t) {
  if(!t->info.count) 
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
  ColInfo *c;TABLE * table; void * vals[8]; int len;
  Triple triple;IO_Structure * to; char num_buff[8];char * key;
  ColInfo *info;int i;int opid;int count;
  to =  get_io_stuct();
  table = get_ready_table();
  table->stmt=get_ready_stmt();
  info=&table->info;
  if(!table->attribute){
    c = init_col_info(table);
    init_json_stream(to);
  }
  G_printf("\n");
  if(table->attribute == EV_Square) {
    machine_unbind_row(table->stmt,&table->info,vals);
    for(i=0;i < info->count;i++){
      if(info->type[i] == G_TYPE_INTEGER) {
        G_sprintf(num_buff,"d",(int)vals[i]);
        key = num_buff;len=G_strlen(num_buff);}
      else{
        key = (char *) vals[i]; len = machine_value_len(table->stmt,i);}
      if(!i) {opid = ',';count=info->count; }else{opid = '.'; count=1;}
      stream_json(opid,count,len,key,to);
    }}
  else{
    machine_triple(table->stmt,&triple);
    len = machine_key_len(table->stmt);
    stream_json(triple.link,triple.pointer,len,triple.key,to);
  }
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

