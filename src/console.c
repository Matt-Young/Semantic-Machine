// Version 2.0 of the comand line for the G machine
// 

#include <ctype.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <stdarg.h>
typedef struct sqlite3_stmt sqlite3_stmt;
typedef struct sqlite3 sqlite3;
#include "g.h"
#include "graphs.h"
void gerror(char * c,int i) {printf("error %d\n",c); exit(i);}
int init_gbase();


void * context;
int main(int argc, char * argv[])
{
  int status; 
  //status = init_dll(); 
  status = init_gbase();
  context = start_select();
  for(;;) {parser();  }
  printf("Done\n");
  exit(0);
}

char line[200];
char * process_block(char * start);
int parser() {
  char * start;
  TRIPLE t = {"Start",'_',0};
  for(;;) {
    printf("%s","P: ");
    memset(line,0,200);
    gets_s(line,200);
    if(line[0] == 0)
      break;
    del_table_graph(LIST(2));
    start = process_block(line);
    reset_graphs(2);
    reset_graphs(3);
    triple(TABLE_POINTER(2)->pop_triple,0);
  }
  return(0);
}
int key_op(char * key) {
      int i = 0;
      while(!ispunct(key[i]) && (key[i] != 0) ) i++; 
      return i;
    }
// operators by weight
#define DISCARD 256
#define TABLENAME 257
char * gchar(char * start,TRIPLE *t);
char gbuffer[256];
char tablename[20];
char * process_block(char * start) {
  TRIPLE ts,next;
  int nchars,done;
  PGRAPH *list = LIST(G_TABLE_SELF);
  ts.link = DISCARD;
  done = 0;
  new_graph(list); // starting SET
  while(!done ) {
    //graph_counts();
    int i,op;
    i=0;
    nchars = key_op(start);
    op = start[nchars];
    if((nchars == 0) && (op == 0)) done=1;
    if((nchars > 0) && (op == 0)) op='.'; // default operator
    if(ts.link == TABLENAME) {
      if(op !=  GCHAR)  {
        strcpy_s(tablename,20,next.key);
        set_table_name(tablename,G_TABLE_SELF);
        init_table(G_TABLE_SELF);
        next.link = DISCARD;
      }
      ts.link = DISCARD;
    }
    if(op==GCHAR) { // GCHAR 'key'
      start = gchar(start,&next);}
    else{next.key=start;next.link=op;next.pointer=(*list)->row+1;
      start[nchars]=0;
      start+=nchars+1;
      }


    if(ts.link == ':' )  {  
      new_graph(list);  
      append_graph(list,ts);
      if(next.link == '(')
        next.link = DISCARD;
    } else if(next.link == '(') {
      if(ts.link != DISCARD) {
        if(ts.link == ',') {
          append_graph(list,ts);
          close_update_graph(list);
          new_graph(list);
        } else if(ts.link == '.') {
          new_graph(list);
          append_graph(list,ts);
        }
      }
      next.link = DISCARD;
    } 
    else if((ts.link == '.') || (ts.link < G_USERMIN) )   {
      if(strlen(ts.key) > 0)
        append_graph(list,ts);  }
    else if(ts.link == ',' ) {
      if(strlen(ts.key) > 0)
        append_graph(list,ts);
      close_update_graph(list);
      new_graph(list);
      } 
    else if(ts.link == ')') {
      if(strlen(ts.key) > 0) {
        next.link = '.';
        append_graph(list,ts);
      }
    close_update_graph(list);
  }
    else if(ts.link != DISCARD ) { // dunno, give it its own segment
      append_graph(list,ts);
      close_update_graph(list);
      new_graph(list);
    }
     ts = next;
  }  
  while((*list)->parent) {
    if(empty_graph(*list))
      delete_graph(list);
     else  
      close_update_graph(list);  
  }
  return(0);
}
#define sql_null_event "select '_',96,0;"
//Taking the ackward approach on accessing G internals with grammar
char * gchar(char * start,TRIPLE *tp) { 
  int nchars;
  start++;
  if(*start == '.')
    return start;
  if(*start == 'q')
    exit(0);
  nchars=0;
  while(start[nchars] != ';') nchars++;
  nchars++;
  if(strncmp(start,"op",2)) {
    tp->link = G_SQL;
    tp->key = start;
    start+=nchars;
    return(start);
  } else {
  start+=2;
  if(isdigit(*start) ) {
    int i = atoi(start);
    int j = atoi(start+3);
    sprintf_s(gbuffer,4,"%d",j);
    tp->key = gbuffer;
    tp->link = i;
    return(start+6);
    }
  }
  return(start+1);
}

void G_printf( const char *fmt, ...) 
{    
 va_list argptr;
 va_start(argptr,fmt);
 vprintf(fmt, argptr);
 va_end(argptr);
}
void G_sprintf(char *s, const char *fmt, ...) 
{  
 va_list argptr;
 va_start(argptr,fmt);
 vsprintf(s,fmt, argptr);
 va_end(argptr);
}
void* G_malloc(int size){return malloc(size);}
void* G_calloc(int size){return calloc(size,1);}
void G_free(void* p){free(p);}
char* G_strncpy(char* s, const char* ct, int n){return strncpy(s,ct,n);}
char * G_strcpy(char* s, const char* ct){return strcpy(s, ct);}
int G_strcmp(const char* cs, const char* ct){return strcmp( cs, ct);}
int G_strlen(const char* cs){return strlen(cs);}
int G_atoi(const char* s){ return atoi(s);}
void G_memset(void* s, int c, int n) {memset(s,c,n);}
