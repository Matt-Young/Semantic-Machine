#include "../include/sqlite3.h"
#include "g.h"
#include "graphs.h"

#define DISCARD 256

int key_op(char * key) {
      int i = 0;
      while(!G_ispunct(key[i]) && (key[i] != 0) ) i++; 
      return i;
    }
char * process_block(PGRAPH *list) {
  TRIPLE ts,next;
  char line[200],*start;
  int nchars,done;
  ts.link = DISCARD;
  done = 0;
  new_graph(list); // starting SET
  for(;;) {
    G_memset(line,0,200);
    start =  line;
    G_gets(line);
    if(line[0] == 0)
      break;
    while(!done ) {
      int i,op;
      i=0;
      nchars = key_op(start);
      op = start[nchars];
      if((nchars == 0) && (op == 0)) done=1;
      if((nchars > 0) && (op == 0)) op='.'; // default operator
	  next.key=start;next.link=op;next.pointer=(*list)->row+1;
      start[nchars]=0;
      start+=nchars+1;
      if(ts.link == ':' )  {  
        new_graph(list);  
        append_graph(list,ts);
        if(next.link == '{')
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
      if(G_strlen(ts.key) > 0)
        append_graph(list,ts); }
    else if(ts.link == ',' ) {
      if(G_strlen(ts.key) > 0)
        append_graph(list,ts);
      close_update_graph(list);
	  //proces_block
      new_graph(list);
      } 
    else if(ts.link == ')') {
      if(G_strlen(ts.key) > 0) {
        next.link = '.';
        append_graph(list,ts);
      }
    close_update_graph(list);
  }
    else if(ts.link != DISCARD ) { // dunno, give it its own segment
      append_graph(list,ts);
      close_update_graph(list);
	  //process block
      new_graph(list);
    }
     ts = next;
  }  
  }
  while((*list)->parent) {
    if(empty_graph(*list))
      delete_graph(list);
     else  
      close_update_graph(list);  
  }

  return(0);
}