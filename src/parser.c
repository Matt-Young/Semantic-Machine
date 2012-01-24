#include "sqlite_msgs.h"
#include "all.h"
#include <ctype.h>
#define Debug_parser
#define DISCARD 256
typedef char * CharPointer;
char * null_key = "_";
int graph_counter;
// Collects alphnumerics unti the next punctuation mark, does quote '
int key_op(const CharPointer base,CharPointer *current,CharPointer *key,int * op) {
      int i;
	  CharPointer ptr= *current;
	  CharPointer end;
	  if(*ptr == 0) {
		  	*current = base;
			ptr = *current;
			G_line(base);
			if(base[0] == 0)
				return -1;
	  }
	  i = 0; end=0;
	  while(isspace(*ptr)) ptr++;
	  if(*ptr == 39) {
		  ptr++;
		  *key=ptr;
		  while(*ptr != 39) ptr++;
		  end = ptr;ptr++;
	  }
	  else 
		  *key=ptr;
      while(!G_ispunct(*ptr) && (*ptr != 0) ) {i++; ptr++;}
	  if(end==0) {
		 end = ptr;
		if ((*end) != 0)  while(isspace(*(end-1) )) end--;
		}
	  *op = (int) *ptr;
	  i = (int) ptr - (int) *current;
	  *current = ptr+1;
	  *end=0;
      return i;
    }
// apply known attributes
void SetAttribute(Triple * destination,char * attribute) {
	Trio * trio= find_trio(attribute);
	if(trio) {
		if(trio->type == G_TYPE_BIT) 
			destination->link += (int) trio->value;  
		else if(trio->type == G_TYPE_SYSTEM)
			destination->link = (int) trio->value;
	}
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
	
	  nchars =key_op(line,&start,&next.key,&next.link);
      if(nchars < 0)
		break;
      if(next.link == 0) op='.'; // default operator
	  if(!G_strlen(next.key) ) 
			next.key = null_key;  // valid null key
	  next.pointer=(*inner)->row+1;
#ifdef Debug_parser
	  if(1) {
	  G_printf("%s %d  ", next.key,next.link);
	  next.link = DISCARD;
	  } else
#endif
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
#ifdef Debug_parser
  for(;;)process_block(pt);
#else
    return process_block(pt);
#endif
  return(SQLITE_OK);
}

