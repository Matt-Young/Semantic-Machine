// Console, set up
// 
#include "../include/sqlite3.h"
#include "g.h"
#include "graphs.h"
void gerror(char * c,int i) {printf("error %d\n",c); exit(i);}
char * process_block(PGRAPH *list)
int parser() {
  char * start;
  TRIPLE t = {"Start",'_',0};
  for(;;) {
    printf("%s","P: ");
    del_table_graph(LIST(2));
    start = process_block(LIST(2));
    reset_graphs(2);
    reset_graphs(3);

  }
  return(0);
}
#ifdef DEBUG
//
//
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
#endif

// These are here just to keep the std lib includes in one spot
#include <ctype.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <stdarg.h>
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
void G_exit() { exit(0);}
int G_ispunct(int c){return ispunct(c);}
char * G_gets(char * line) { return gets(line);};
