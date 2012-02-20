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
  int total,len,rows,i=0,*buff; char * key_value;
  char link_pointer[12];Triple * Qson;
  total=0;
  fread(link_pointer,1,12,fd);
  sscanf(link_pointer+1,"%3d",&rows);
  buff = (int *)  malloc(rows*sizeof(Triple)+8);
  Qson = (Triple *) (buff+2);
  for(i=0;i<rows;i++) {
    if(i) 
        fread(link_pointer,1,12,fd);
  sscanf(link_pointer,"%c%3d4d",&Qson->link,&Qson->pointer,&len);
  key_value = (char *)  malloc(4*len+5);
  sprintf(key_value,"%4d",len);
  fread(key_value+4,4,len,fd);  // bytes (from fixed length key values
  Qson->key = key_value;
  Qson->key[len+4]=0;
  DebugPrint("FM%s%c%3d\n",Qson->key,Qson->link,Qson->pointer);
  Qson++;
total += len+4;
  }
  total += sizeof(Triple) * rows;
  sprintf((char *) set_output_buff(0),"%8d",total);
return buff;
}
int mem_to_file( FILE * dest,int * buff,int mode){
  int rows,len,total; int i;Triple *Qson;
  char * key_value;
   Qson = (Triple *)(buff+2);
   fwrite((char *) buff,1,8,dest);  // total count
  rows = Qson[0].pointer; total = 0;
  for(i=0;i<rows;i++) {
    key_value = Qson[i].key;
    sscanf(key_value,"%4d",&len);
    fprintf(dest,"%c%3d",Qson[i].link,Qson[i].pointer);
    fprintf(dest,"%4d",len);
    fwrite(key_value+4,4,len,dest);
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
  int len,total; int k,i,rows;int * buff;
  Triple Qin,*Qout;char * key_value;
  Code stmt;
  start_table(t,pop_operator);
  stmt = get_ready_stmt();
  total = 0;
  // peek at the header
  i = machine_step_fetch(&Qin,0); 
  rows = Qin.pointer;
  
    buff = (int *) malloc(rows*sizeof(Triple)+8);
  Qout = (Triple *)  (buff+2);
  for(i=0;i<rows;i++) {
    if(i) machine_step_fetch(&Qin,0);
  len = machine_key_len(stmt); 
  *Qout = Qin;
  key_value = (char *)  malloc(4*len+1);
  sprintf(key_value,"%4d",len);
  memcpy(key_value+4,Qin.key,len*4);
  if (k = (len & 0x3))
      memcpy(key_value+4+len*4,"___",4-k);  // keep at four byte boudary
  key_value[len+4]=0;

  DebugPrint("TM%s%c%3d\n",Qout->key,Qout->link,Qout->pointer);
   Qout++;
total += len+4;
  }
  total += sizeof(Triple) * rows;
  sprintf((char *) buff,"%8d",(total+3)/4);
  return buff;
}

// extract the Qson
Triple * set_output_buff(Triple *t);
int mem_to_table(TABLE* table,int * buff,int mode) {
  int rows,total; int i;Triple *data;
  Triple * Qson;
  Code stmt;
  start_table((TABLE *) table,append_old_operator);

  data = &(table)->operators[append_old_data];
  Qson = (Triple *) (buff+2);
  rows = Qson[0].pointer; total = 0;
  for(i=0;i<rows;i++) {
    *data = Qson[i];
      DebugPrint("MT%s%c%3d\n",Qson[i].key,Qson[i].link,Qson[i].pointer);
    //len = (int) Qson.key; // for blob bind
      machine_reset(table->stmt);
      bind_code(&(table)->operators[append_old_operator],table->stmt);
      machine_step(table->stmt);

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
      fd= fopen((char *)to->addr, "w+");
      printf("File out %s\n",(char *)  to->addr);
      mem_to_file(fd,from->buff,AF_FILE);
      fflush(fd);
      fclose(fd);
    }
    if(to->sa_family== AF_CONSOLE) {
      mem_to_file(stdout,from->buff,AF_CONSOLE);
    }
    if(to->sa_family== AF_TABLE){
      TABLE * table;
      init_table((char *) to->addr,1,&table);
      mem_to_table(table,to->buff,AF_TABLE);
    } 
    if(to->sa_family== AF_INET){
      mem_to_net(to->fd,from->buff,Qson_IO);
    } 
  }
  else if(from->sa_family== AF_TABLE && to->sa_family== AF_INET){
    TABLE * table;
    init_table((char *) to->addr,0,&table);
    //table_to_net(to->fd,(int *) from->buff);
  }
    else if(from->sa_family== AF_TABLE && to->sa_family== AF_TABLE)
      dup_table((char *) from->addr,(char *) to->addr);
  else if(( to->sa_family== AF_TABLE) 
     && ((from->sa_family== AF_INET) || (from->sa_family== AF_CONSOLE))) {
      TABLE * table;
      printf("New Table  %s\n",(char *)  to->addr);
      init_table((char *) to->addr,1,&table);
      if(from->fd == Json_IO) 
        parser((char *) from->buff,table);    // Json from the net
      else if (from->fd == Qson_IO)
        qson_to_table(table,(char *) to->buff,to->count);
  }
  else  if(from->sa_family== AF_TABLE && to->sa_family== AF_MEMORY){
    TABLE * table;
    printf("Table from %s\n",(char *)  from->addr);
    init_table((char *) from->addr,0,&table);
    to->buff = table_to_mem(table);
    // else this might be or the network
  }
    else  if(from->sa_family== AF_FILE && to->sa_family== AF_MEMORY){
      FILE *fd;
     printf("File in %s\n",(char *)  from->buff);
      fd= fopen((char *) from->addr, "r");
    to->buff = file_to_mem(fd);
    fclose(fd);
    // else this might be or the network
  }


  return 0;
}

int test_qson() {
	Webaddr w1,w2;
  printf("Test qson\n");
  strcpy((char *) w1.addr,"test");
  w1.sa_family = AF_TABLE;
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
