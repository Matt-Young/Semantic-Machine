#include "../include/sqlite3.h"
#include "all.h"

#define DISCARD 256
typedef char * CharPointer;
// Collects alphnumerics unti the next punctuation mark
int key_op(const CharPointer base,CharPointer *current,int * op) {
      int i = 0;
	  if(**current == 0) {
		  	*current = base;
			G_line(base);
			if(base[0] == 0)
				return -1;
	  }
	  while(!G_ispunct(*(*current)) && (*(*current) != 0) ) {i++; (*current)++;}
	  *op = (int) *(*current);
      return i;
    }
// buils a subgraph on inner from user text
char * process_block(PGRAPH *inner) {
  TRIPLE prev,next;
  char line[200],*start;
  int nchars,done;
  prev.link = DISCARD;
  done = 0;
  G_memset(line,0,200);
  start = line;
  new_graph(inner); // enclose this work in a subgraph
  for(;;) {
	done = 0;
    while(!done ) {
      int i,op;
      i=0;
	  nchars =key_op(line,&start,&op);
      if(nchars < 0)
		break;
      if(op == 0) op='.'; // default operator
	  next.key=start;next.link=op;next.pointer=(*inner)->row+1;
      start[nchars]=0;
      start+=nchars+1;
      if(prev.link == ':' )  {  // This one starts a sub graph
        new_graph(inner);  
        append_graph(inner,prev);
        if(next.link == '{') // Making this a duplicate
          next.link = DISCARD;
        }else if(next.link == '{') {
          if(prev.link != DISCARD) {
            if(prev.link == ',') {
              append_graph(inner,prev);
              close_update_graph(inner);
              new_graph(inner);
            } else if(prev.link == '.') {
               new_graph(inner);
               append_graph(inner,prev);
              }
           }
         next.link = DISCARD;
        }   
      else if((prev.link == '.') || (prev.link < G_USERMIN) )   { // Append triplet
        if(G_strlen(prev.key) > 0)
           append_graph(inner,prev); 
	  } else if(prev.link == ',' ) {
           if(G_strlen(prev.key) > 0)
             append_graph(inner,prev);
          close_update_graph(inner);
	  //proces_block
        new_graph(inner);
      } else if(prev.link == '}') {
        if(G_strlen(prev.key) > 0) {
           next.link = '.';
          append_graph(inner,prev);
         }
       close_update_graph(inner);
     } else if(prev.link != DISCARD ) { 
        append_graph(inner,prev);
        close_update_graph(inner);
	    //process block
        new_graph(inner);
      }
     prev = next;
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
 // for(;;) {
    G_printf("%c",G_NULL);
    start = process_block(pt);
  //  G_exit();
  //}
  return(SQLITE_OK);
}

