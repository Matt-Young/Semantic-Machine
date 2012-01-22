#include "../include/sqlite3.h"
#include "g.h"
#include "graphs.h"

#define DISCARD 256

int key_op(char * key) {
      int i = 0;
      while(!G_ispunct(key[i]) && (key[i] != 0) ) i++; 
      return i;
    }
char * process_block(PGRAPH *inner) {
  TRIPLE ts,next;
  char line[200],*start;
  int nchars,done;
  ts.link = DISCARD;
  done = 0;
  new_graph(inner); // starting SET
  for(;;) {
    G_memset(line,0,200);
    start =  line;
    G_gets(line);
    if(line[0] == 0)
      break;
	done = 0;
    while(!done ) {
      int i,op;
      i=0;
      nchars = key_op(start);
      op = start[nchars];
      if((nchars == 0) && (op == 0)) done=1;
      if((nchars > 0) && (op == 0)) op='.'; // default operator
	  next.key=start;next.link=op;next.pointer=(*inner)->row+1;
      start[nchars]=0;
      start+=nchars+1;
      if(ts.link == ':' )  {  
        new_graph(inner);  
        append_graph(inner,ts);
        if(next.link == '{')
          next.link = DISCARD;
       } else if(next.link == '{') {
         if(ts.link != DISCARD) {
           if(ts.link == ',') {
            append_graph(inner,ts);
            close_update_graph(inner);
            new_graph(inner);
            } else if(ts.link == '.') {
            new_graph(inner);
            append_graph(inner,ts);
          }
        }
      next.link = DISCARD;
    } 
    else if((ts.link == '.') || (ts.link < G_USERMIN) )   {
      if(G_strlen(ts.key) > 0)
        append_graph(inner,ts); }
    else if(ts.link == ',' ) {
      if(G_strlen(ts.key) > 0)
        append_graph(inner,ts);
      close_update_graph(inner);
	  //proces_block
      new_graph(inner);
      } 
	else if(ts.link == '}') {
      if(G_strlen(ts.key) > 0) {
        next.link = '.';
        append_graph(inner,ts);
      }
    close_update_graph(inner);
  }
    else if(ts.link != DISCARD ) { 
      append_graph(inner,ts);
      close_update_graph(inner);
	  //process block
      new_graph(inner);
    }
     ts = next;
  }  
  }
  // finish up
  while((*inner)->parent) {
    if(empty_graph(*inner))
      delete_graph(inner);
     else  
      close_update_graph(inner);  
  }

  return(0);
}

PGRAPH init_parser(char * name) {
	int status;
  TABLE * t = get_table_name(name);
  status = new_table_graph(t); 
  return(t->list);
}
int parser() {
  char * start;
  TABLE * t = get_table_name("console");
  PGRAPH * pt = (PGRAPH *) (&t->list);
  for(;;) {
    G_printf("%c",G_NULL);
    start = process_block(pt);
    G_exit();
  }
  return(0);
}

