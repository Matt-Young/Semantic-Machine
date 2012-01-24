#include "sqlite_msgs.h"
#include "all.h"

#define DISCARD 256
typedef char * CharPointer;
char * null_key = "_";
int graph_counter;
// Collects alphnumerics unti the next punctuation mark
int key_op(const CharPointer base,CharPointer *current,int * op) {
      int i;
	  CharPointer ptr=*current;
	  if(*ptr == 0) {
		  	*current = base;
			ptr = *current;
			G_line(base);
			if(base[0] == 0)
				return -1;
	  }
	  i = 0;
	  while(!G_ispunct(*ptr) && (*ptr != 0) ) {i++; ptr++;}
	  *op = (int) *ptr;
	  *current = ptr;
      return i;
    }
// apply known attributes
void SetAttribute(Triple * destination,char * attribute) {
	Trio * trio= find_trio(attribute);
	if(trio)
		destination->link = (int) trio->value;  
}
// buils a subgraph on inner from user text
int process_block(PGRAPH *inner) {
  Triple prev,current,next;
  char line[200],*start;
  int nchars,named_count;
  prev.link = DISCARD;
  current.link = DISCARD;
  G_memset(line,0,200);
  start = line;
  named_count=0;
  new_graph(inner); // enclose this work in a subgraph
  for(;;) {
      int op;
	  next.key=start;
	  nchars =key_op(line,&start,&op);
      if(nchars < 0)
		break;
      if(op == 0) op='.'; // default operator
	  next.link=op;next.pointer=(*inner)->row+1;
	  if((nchars == 0) || !G_strlen(next.key) || G_strlen(next.key) == 0) 
			next.key = null_key;  // valid null key
      start[0]=0;
      start++;
	  // Handle attributes immediately
	if(current.link == '$') {
		SetAttribute(&current,next.key);
			 current.link = DISCARD;
	}
	// Erase the bracket
	else if(current.link == '{') {
			if(prev.link != ':')
				new_graph(inner);
		 current.link = DISCARD;
	  } else if(current.link == '.') {
	  // Case:  key.
		  if(prev.link != '.')
			  new_graph(inner);
           append_graph(inner,current);
	}  else if(current.link == ',' ) {
// Case: key,X
		if((prev.link != '.') && (prev.link != DISCARD) )
			close_update_graph(inner);
		new_graph(inner);
		append_graph(inner,current);
// Case: key:X
	} else if(current.link == ':' )  {  
		named_count++;
        new_graph(inner);  
        append_graph(inner,current);
	  } else if(current.link == '}' ) 
		close_update_graph(inner);
     prev = current;
	 current = next; 
    }  
  // finish up
  while((*inner)->parent) {
    if(count_graph(*inner))
      delete_graph(inner);
     else  
      close_update_graph(inner);  
  }
  return(count_graph(*inner));
}

PGRAPH init_parser(char * name) {
	int status;
  TABLE * t = get_table_name(name);
  status = new_table_graph(t); 
  return(t->list);
}
int parser() {
  TABLE * t = get_table_name("console");
  PGRAPH * pt = (PGRAPH *) (&t->list);
 // for(;;) {
    return process_block(pt);
  //  G_exit();
  //}
  return(SQLITE_OK);
}

