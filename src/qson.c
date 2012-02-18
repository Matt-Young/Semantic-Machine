#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/g_types.h"
#include "../src/config.h"
#include "../src/engine.h"
#include "../src/tables.h"
#include "../src/qson.h"

//*******************************
// Qson Switch
//***********************
//
// Move a Qson graph between (table,mem,file,net,console) 
//
int net_to_table(TABLE * table,int * buff) {
  Triple *Qson;Triple v; int i;Code stmt;
    machine_set_operator(&table->operators[append_operator],0);
    stmt = get_ready_stmt();
    Qson = &table->operators[append_data];
    v = *(Triple *) buff;
   for(i=0;i<v.pointer;i++) {
     //len = make_word_from_bytes(Qson[i].key);
      *Qson = *(Triple *) buff;
      machine_reset(stmt);
      bind_code(&table->operators[append_operator],stmt);
      machine_step(stmt);
   }
   return 0;
}
// extract the Qson
Triple * set_output_buff(Triple *t);
int table_to_net(TABLE *t) {
  int len,total; Triple v;int i;int malloc_size;
  Triple *Qson;int * send_packet;
  Code stmt;
  start_table(t,pop_operator);
  stmt = get_ready_stmt();
  total = 0;
  // peek at the header
  i = machine_step_fetch(&v,0); 
  malloc_size = v.pointer*sizeof(Triple); // Twice the size
  send_packet = (int *) malloc(malloc_size);
  Qson = (Triple *) (send_packet+1);

  set_output_buff(Qson);
  Qson[0]=v;
  for(i=0;i<v.pointer;i++) {
    send_packet = (int *) &Qson[i];
    if(i) 
      machine_step_fetch(&Qson[i],0);
    len = machine_key_len(stmt); 
    sprintf( (char *) send_packet,"%4d",Qson[i],len);
  //  make_bytes_from_word(Qson[i].key,len);
    send_packet += (len+3)/4;
    Qson = (Triple *) send_packet;
  }
  return 0;
}
// Simple text file format
int file_to_mem(FILE *fd) {
  int total,len,rows,i=0; char * key_value;
  char link_pointer[4];Triple * Qson;
  total=0;
  fread(link_pointer,1,4,fd);
  sscanf(link_pointer+1,"%3d",&rows);
  Qson = set_output_buff((Triple *) malloc(rows*sizeof(Triple)));
  for(i=0;i<rows;i++) {
    if(i) 
        fread(link_pointer,1,4,fd);
  sscanf(link_pointer,"%c%3d",&Qson->link,&Qson->pointer);
  fscanf(fd,"%4d",&len);
  key_value = (char *)  G_malloc(len+5);
  sprintf(key_value,"%4d",len);
  fread(key_value+4,1,len,fd);  // bytes (from fixed ength key values
  Qson->key = key_value;
  Qson->key[len+4]=0;
  printf("FM%s%c%3d\n",Qson->key,Qson->link,Qson->pointer);
  Qson++;
total += len+4;
  }
return 0;
}
int mem_to_file( FILE * dest,int mode){
  int rows,len,total; int i;Triple *Qson;
  char * key_value;
   Qson = set_output_buff(0);
  rows = Qson[0].pointer; total = 0;
  for(i=0;i<rows;i++) {
    key_value = Qson[i].key;
    sscanf(key_value,"%4d",&len);
    fprintf(dest,"%c%3d",Qson[i].link,Qson[i].pointer);
    fprintf(dest,"%4d",len);
    fwrite(key_value+4,1,len,dest);
    if(mode != AF_CONSOLE)
    printf("MF%s%c%3d%\n",key_value,Qson->link,Qson->pointer);
  }
  if(mode == AF_FILE)
   fclose(dest);
  return 0;
}
int table_to_mem(TABLE *t) {
  int len,total; Triple v;int i;
  Triple *Qson;char * key_value;
  Code stmt;
  start_table(t,pop_operator);
  stmt = get_ready_stmt();
  total = 0;
  // peek at the header
  i = machine_step_fetch(&v,0); 
  Qson = set_output_buff((Triple *) malloc(v.pointer*sizeof(Triple)));
  *Qson = v;
  for(i=0;i<v.pointer;i++) {
    if(i) machine_step_fetch(Qson,0);
  len = machine_key_len(stmt); 
  key_value = (char *)  malloc(len+5);
  sprintf(key_value,"%4d",len);
  memcpy(key_value+4,Qson->key,len);
  Qson->key = key_value;
  Qson->key[len+4]=0;
 
  printf("TM%s%c%3d\n",Qson->key,Qson->link,Qson->pointer);
   Qson++;
total += len+4;
  }
  return total + sizeof(Triple) * v.pointer;
}

// extract the Qson
Triple * set_output_buff(Triple *t);
int mem_to_table(void * dest,int mode) {
  int rows,total; int i;Triple *data;
  Triple * Qson;
  Code stmt;
  start_table((TABLE *) dest,append_old_operator);
  stmt = get_ready_stmt();
  data = &((TABLE *) dest)->operators[append_old_data];
  Qson = set_output_buff(0);
  rows = Qson[0].pointer; total = 0;
  for(i=0;i<rows;i++) {
    *data = Qson[i];
      printf("MT%s%c%3d\n",Qson[i].key,Qson[i].link,Qson[i].pointer);
    //len = (int) Qson.key; // for blob bind
      machine_reset(stmt);
      bind_code(&((TABLE *)dest)->operators[append_old_operator],stmt);
      machine_step(stmt);

    free(Qson[i].key);
  }
  free(Qson);
  return total + sizeof(Triple) * rows;
}

int system_copy_qson(Webaddr *from,Webaddr *to ) {
  machine_reset(get_ready_stmt());  // big deal, start fresh
  if(from->sa_family ==  AF_MEMORY) {
    if(to->sa_family== AF_FILE) {
      FILE *fd;
      fd= fopen((char *)to->data, "w+");
      printf("File out %s\n",(char *)  to->data);
      mem_to_file(fd,AF_FILE);
      fflush(fd);
      fclose(fd);
    }
    if(to->sa_family== AF_CONSOLE) {
      mem_to_file(stdout,AF_CONSOLE);
    }
    if(to->sa_family== AF_TABLE){
      TABLE * table;
      init_table((char *) to->data,1,&table);
      mem_to_table(table,AF_TABLE);
    } 
  }
  else if(from->sa_family== AF_TABLE && to->sa_family== AF_INET){
    TABLE * table;
    init_table((char *) to->data,0,&table);
    table_to_net(table);
  }
  else if(from->sa_family== AF_INET && to->sa_family== AF_TABLE){
    TABLE * table;
    init_table((char *) to->data,1,&table);
    net_to_table(table,(int *) from->data);
    // else this might be or the network
  }
  else  if(from->sa_family== AF_TABLE && to->sa_family== AF_MEMORY){
    TABLE * table;
    init_table((char *) from->data,0,&table);
    table_to_mem(table);
    // else this might be or the network
  }
    else  if(from->sa_family== AF_FILE && to->sa_family== AF_MEMORY){
      FILE *fd;
     printf("File in %s\n",(char *)  from->data);
      fd= fopen((char *) from->data, "r");
    file_to_mem(fd);
    fclose(fd);
    // else this might be or the network
  }
  return 0;
}

int test_qson() {
	Webaddr w1,w2;
  printf("Test qson\n");
  strcpy((char *) w1.data,"test");
  w1.sa_family = AF_TABLE;
  w2.sa_family = AF_MEMORY;
  system_copy_qson(&w1,&w2);// table to mem
 w1.sa_family = AF_CONSOLE;
  system_copy_qson(&w2,&w1); // mem to console
   strcpy((char *) w1.data,"c:\\soft\\result");
   w1.sa_family = AF_FILE;
  system_copy_qson(&w2,&w1); // mem to file

   strcpy((char *) w1.data,"testagain");
    w1.sa_family = AF_TABLE;
    system_copy_qson(&w2,&w1);  //mem to table
  strcpy((char *) w1.data,"c:\\soft\\result");
    w1.sa_family = AF_FILE;
    system_copy_qson(&w1,&w2);  //file to mem
    return 0;
}
