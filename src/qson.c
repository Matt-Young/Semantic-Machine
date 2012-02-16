#include <stdio.h>
#include "../src/g_types.h"
#include "../src/config.h"
#include "../src/engine.h"
#include "../src/tables.h"
#include "../src/console.h"
#include "../src/qson.h"

int  make_word_from_bytes(char * b) {
  int i;
     i =  *b++;
    i += *b++  << 8;
    i += *b++  << 16;
    i += *b++  << 24;
    return i;}

char *  make_bytes_from_word(char * b,int i) {
    *b++ = i & 0xff;
    *b++ = (i >> 8) & 0xff;
    *b++ = (i >> 16) & 0xff;
    *b++ = i >> 24;
  return b;}

// Qson bundled in and out
//
// extract the Qson
Triple * set_output_buff(Triple *t);

int get_qson_file(TABLE *t,FILE *fd) {
  int len,i=0; char * key_value;Triple v;Triple *Qson;
        fread(&v,1,sizeof(Triple),fd);
      len = make_word_from_bytes(Qson[i].key);
      key_value = (char *)  G_malloc(len+5);
       Qson[0]=v;
  for(i=0;i<v.pointer;i++) {
    if(i)
      fread(&v,1,sizeof(Triple),fd);
              key_value = (char *)  G_malloc(len+5);
      fread(key_value+4, len, 1, fd);
  }
return 0;
}
int get_qson_graph(TABLE *t) {
  int len,total; Triple v;int i;
  Triple *Qson;char * key_value;
  Code stmt;
  stmt = get_ready_stmt();
  total = 0;
  // peek at the header
  i = machine_step_fetch(&v,0); 
  Qson = (Triple *) G_malloc(v.pointer*sizeof(Triple));
  len = machine_key_len(stmt); 
  set_output_buff(Qson);
  Qson[0]=v;
  for(i=0;i<v.pointer;i++) {
    if(i) 
    len = machine_step_fetch(&Qson[i],0);
  len = machine_key_len(stmt); 
  key_value = (char *)  G_malloc(len+5);
  G_memcpy(key_value+4,Qson[i].key,len);
make_bytes_from_word(key_value,len);
Qson[i].key=key_value;
Qson[i].key[len+4]=0;
total += len+5;
  }
  return total + sizeof(Triple) * v.pointer;
}

// extract the Qson
Triple * set_output_buff(Triple *t);
int put_qson_graph_mode(TABLE *table,int mode,FILE *fd) {
  int rows,len,total; int i;Triple *data;
  Triple * Qson;
  Code stmt;
  start_table(table,append_operator);
  stmt = get_ready_stmt();
  data = &table->operators[append_data];
  Qson = set_output_buff(0);
  rows = Qson[0].pointer; total = 0;
  for(i=0;i<rows;i++) {
    *data = Qson[i];
    len = make_word_from_bytes(data->key);
    // Json IO
    if(mode == AF_CONSOLE)
      G_printf("Qson: %d %s \n",len,data->key+4);
    else if (mode == AF_TABLE) {
      machine_reset(stmt);
      bind_code(&table->operators[append_operator],stmt);
      machine_step(stmt);
    }else if (mode == AF_FILE) {
      fwrite(data,1,sizeof(Triple),fd);
      fwrite(data->key+4, len, 1, fd);
    }
    G_free(Qson[i].key);
  }
  G_free(Qson);
  return total + sizeof(Triple) * rows;
}
int put_qson_graph(TABLE *table) {
  put_qson_graph_mode(table,AF_TABLE,0);
  return 0;
}
int print_qson_graph(TABLE *table) {
  put_qson_graph_mode(table,AF_CONSOLE,0);
  return 0;
}
int write_qson_graph(TABLE *table) {
  FILE *fd;
  fd= fopen("test", "w+");
  put_qson_graph_mode(table,AF_FILE,fd);
  return 0;
}
