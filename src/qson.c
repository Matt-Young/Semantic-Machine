#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/include/config.h"
#include "../src/include/g_types.h"
#include "../src/include/names.h"
#include "./include/machine.h"
#include "../src/include/tables.h"
#include "../src/include/engine.h"
#define DebugPrint printf
int parser(char * ,TABLE *);  
//*******************************
// Qson Switch
//***********************
//
// Move a Qson graph form net to Qstore
//
int qson_to_table(TABLE * table,char  * buff,int count) {
  int i;Triple *in;int len;
  machine_set_operator(&table->operators[append_operator],0);
  in = &table->operators[append_data];
  i = 0; while(i < count) {
    //use blob append format
    in->key = (char *) buff;
    sscanf(in->key,"%4d%c%3d",&len,&in->link,&in->pointer);
    machine_reset(table->stmt);
    bind_code(&table->operators[append_operator],table->stmt);
    machine_step(table->stmt);
    i+= (len+8);
    buff+= (len+8);
  }
  return i;
}
// extract the Qson
Triple * set_output_buff(Triple *t);
int sendx(int sockfd, const void *buf, int len, int flags);
int closesocketx(int sock);
int mem_to_net(int fd,int *buff,int protocol) {
  int rows,len,total; int i;Triple *Qson; char  dest[20];
  char * key_value;
  Qson = (Triple *) (buff+2);
  sendx(fd,buff,8,0);
  rows = Qson[0].pointer; total = 0;
  for(i=0;i<rows;i++) {
    key_value = Qson[i].key;
    sscanf(key_value,"%4d",&len);
    sprintf(dest,"%c%3d%4d",Qson[i].link,Qson[i].pointer,len);
    sendx(fd,dest,8,0);
    if(protocol  = Qson_IO) {
      sendx(fd,key_value+4,len/4,0);
      if (total = (len & 0x3))
        sendx(fd,"___",4-total,0);  // keep at four byte boudary
    }
    else
      sendx(fd,key_value+4,len,0);
    DebugPrint("MN%s%c%3d%\n",key_value,Qson->link,Qson->pointer);
  }
  closesocketx(fd);
  return 0;
}
// Simple text file format
int * file_to_mem(FILE *fd) {
  int len,rows,i=0,*buff; char * key_value;
  char link_pointer[12],total[8];Triple * Qson;
 fread(total,1,8,fd);
  fread(link_pointer,1,12,fd);
  sscanf(link_pointer+1,"%3d",&rows);
  buff = (int *)  malloc(rows*sizeof(Triple)+8);
  memcpy(buff,total,8);
  Qson = (Triple *) (buff+2);
  for(i=0;i<rows;i++) {
    if(i) 
      fread(link_pointer,1,12,fd);
    sscanf(link_pointer,"%c%3d%4d",&Qson->link,&Qson->pointer,&len);
    key_value = (char *)  malloc(len+5);
    sprintf(key_value,"%4d",len);
    fread(key_value+4,1,len,fd);  // bytes (from fixed length key values
    Qson->key = key_value;
    //Qson->key[len+4]=0;
    DebugPrint("FM%s%c%3d\n",Qson->key,Qson->link,Qson->pointer);
    Qson++;
  }
  return buff;
}
int mem_to_file( FILE * dest,int * buff,int mode){
  int rows,len,total; int i;Triple *Qson;
  char * key_value;
  Qson = (Triple *)(buff+2);
  fwrite((char *) buff,1,8,dest);  // total count
  rows = Qson[0].pointer; total = 0;
  printf("link again %c\n",Qson[0].link);
  for(i=0;i<rows;i++) {
    key_value = Qson[i].key;
    sscanf(key_value,"%4d",&len);
    printf("Link here %d\n",Qson[i].link);
    fprintf(dest,"%c%3d%4d",Qson[i].link,Qson[i].pointer,len);
    fwrite(key_value,1,len+4,dest);
    if (total = (len & 0x3))
      fwrite("___",1,4-total,dest);  // keep at four byte boudary
    if(mode != AF_CONSOLE)
      DebugPrint("MF%s%c%3d%\n",key_value,Qson->link,Qson->pointer);
  }

  if(mode == AF_FILE)
    fclose(dest);
  return 0;
}
int  * table_to_mem(TABLE *t) {
  int len,long_count; int k,i,rows;int * buff;
  Triple Qin,*Qout;char * key_value;char tmp[9];
  Code stmt;
  start_table(t,pop_operator);
  stmt = get_ready_stmt();
  // peek at the header
  i = machine_step_fetch(&Qin,0); 
  rows = Qin.pointer;

  buff = (int *) malloc(rows*sizeof(Triple)+8);
  long_count=3*rows;  //three fixed  longs per row
  Qout = (Triple *)  (buff+2);
  for(i=0;i<rows;i++) {
    if(i) machine_step_fetch(&Qin,0);
    len = machine_key_len(stmt); 
    k =1+ (len+3) /4;  // count longs, word count and variable long ley fdata
    if(k==0) printf("error\n");
    *Qout = Qin;
    Qout->key = (char *)  malloc(4*k+1);
    long_count += k;
    Qout->key[len+4]=0;
    sprintf(Qout->key,"%4d",len);
    memcpy(Qout->key+4,Qin.key,len);
    printf("Link to file%d\n",Qout->link);
    DebugPrint("TM%s%c%3d\n",Qout->key,Qin.link,Qout->pointer);
    printf("Link another %d\n",Qin.link);
    Qout++;
  }

  sprintf(tmp,"%8d",long_count);
  memcpy(buff,tmp,8);
  return buff;
}

// extract the Qson
Triple * set_output_buff(Triple *t);
int mem_to_table(TABLE* table,int * buff,int mode) {
  int rows,total; int i;Triple *data;
  Triple * Qson;
  Code stmt;
  start_table((TABLE *) table,append_operator);

  data = &(table)->operators[append_data];
  Qson = (Triple *) (buff+2);
  rows = Qson[0].pointer; total = 0;
  for(i=0;i<rows;i++) {
    *data = Qson[i];
    DebugPrint("MT%s%c%3d\n",Qson[i].key,Qson[i].link,Qson[i].pointer);
    //len = (int) Qson.key; // for blob bind
    machine_reset(table->stmt);
    bind_code(&(table)->operators[append_operator],table->stmt);
    machine_step(table->stmt);
  }
  return  rows;
}

int system_copy_qson(Webaddr *from,Webaddr *to ) {
  machine_reset(get_ready_stmt());  // big deal, start fresh
  // if the source is memory format
  if(from->sa_family ==  AF_MEMORY) {
    if(to->sa_family== AF_FILE) {
      FILE *fd;
      fd= fopen((char *)to->addr, "w+");
      printf("File out %s\n",(char *)  to->addr);
      mem_to_file(fd,from->buff,AF_FILE);
      fflush(fd);
      fclose(fd);
    }  else if(to->sa_family== AF_CONSOLE) {
      mem_to_file(stdout,from->buff,AF_CONSOLE);
    } else if(to->sa_family== AF_TABLE){
      TABLE * table;
      init_table((char *) to->addr,1,&table);
      mem_to_table(table,from->buff,AF_TABLE);
    } else if(to->sa_family== AF_INET)
      mem_to_net(to->fd,from->buff,Qson_IO);
    // If the source is a table
  }else if(from->sa_family== AF_TABLE ) {
    if( to->sa_family== AF_INET){
      TABLE * table;
      start_table((char *) to->addr,0,&table);
      //table_to_net(to->fd,(int *) from->buff);
    } else if(to->sa_family== AF_TABLE)
      dup_table((char *) from->addr,(char *) to->addr);

    else if(to->sa_family== AF_MEMORY){
      TABLE * table;
      printf("Table from %s\n",(char *)  from->addr);
      init_table((char *) from->addr,0,&table);
      to->buff = table_to_mem(table);

    }
  // else this might be from the network
  } else if ((from->sa_family== AF_INET) || (from->sa_family== AF_CONSOLE)) {
    if( to->sa_family== AF_TABLE) { 
      TABLE * table;
      printf("New Table  %s\n",(char *)  to->addr);
      init_table((char *) to->addr,1,&table);
      if(from->fd == Json_IO) 
        parser((char *) from->buff,table);    // Json from the net
      else if (from->fd == Qson_IO)
        qson_to_table(table,(char *) to->buff,to->count);
    }
   // source is file
  }  else if(from->sa_family== AF_FILE) {
    if(to->sa_family== AF_MEMORY){
      FILE *fd;
      printf("File in %s\n",(char *)  from->addr);
      fd= fopen((char *) from->addr, "r");
      to->buff = file_to_mem(fd);
      fclose(fd);

    }
  }
  return 0;
}

int test_qson() {
  Webaddr w1,w2;
  printf("Test qson\n");
  strcpy((char *) w1.addr,"test");
  strcpy((char *) w2.addr,"console");
  w1.sa_family = AF_TABLE;
  w2.sa_family = AF_TABLE;
  system_copy_qson(&w2,&w1);// table to table
  w2.sa_family = AF_MEMORY;
  system_copy_qson(&w1,&w2);// table to mem
  w1.sa_family = AF_CONSOLE;
  system_copy_qson(&w2,&w1); // mem to console
  strcpy((char *) w1.addr,"c:\\soft\\result");
  w1.sa_family = AF_FILE;
  system_copy_qson(&w2,&w1); // mem to file
  strcpy((char *) w1.addr,"testagain");
  w1.sa_family = AF_TABLE;
  system_copy_qson(&w2,&w1);  //mem to table
  strcpy((char *) w1.addr,"c:\\soft\\result");
  w1.sa_family = AF_FILE;
  system_copy_qson(&w1,&w2);  //file to mem
  return 0;
}
