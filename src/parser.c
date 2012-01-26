
#include "all.h"
#include <ctype.h>
#define Debug_parser
#define DISCARD 256
#define Line_size 256
typedef char * CharPointer;

int graph_counter;
// apply known attributes using name substitution
void SetAttribute(Triple * destination,Triple * attribute) {
	Trio * trio;
	if(G_strcmp(attribute->key,"local") && (attribute->key[0] != '_'))
		return;
	trio= find_trio(destination->key);
	if(trio && (int) trio->type == G_TYPE_SYSTEM) {
		destination->link =  (int) trio->value;
		attribute->link = DISCARD;
	}
}
// buils a subgraph on inner from user text

int process_block(PGRAPH *inner) {
  Triple prev,current,next;
  Console console;
  int nchars,named_count;
  char line[Line_size];
  G_InitConsole(&console,line,Line_size);
  prev.link = DISCARD;
  current.link = DISCARD;
  G_memset(line,0,Line_size);
  named_count=0;nchars=0;
  new_child_graph(inner); // enclose this work in a subgraph
  while(nchars >= 0) {
	  nchars =G_keyop(&console,&next.key,&next.link);
	  next.pointer=(*inner)->row+1;
#ifdef Debug_parser
	 //if(1) {
		  G_printf("<%10s %c  %4d>\n", next.key,next.link,next.pointer);
	  //next.link = DISCARD;
	 // } else
#endif
	  // Handle attributes immediately
	if(current.link == '$') { // key$
		SetAttribute(&current,&next);
			 current.link = DISCARD;
	}
	// Erase the bracket
	else if(current.link == '{') { //{ key:{   or .} or ,{
			if(prev.link != ':')
				new_child_graph(inner);
		 current.link = DISCARD;
	} else if((current.link == '.') || (current.link == '_')){ // key. key_
//
		append_graph(inner,current);
	} else if(current.link == ',' ) {  // key,
//
		close_update_graph(inner);
		new_child_graph(inner);
		append_graph(inner,current);
// Case: key:X
	} else if(current.link == ':' )  {  // key:
		named_count++;
        new_child_graph(inner);  
        append_graph(inner,current);
	  } else if(current.link == '}' ) { //  | key} |
		append_graph(inner,current);
		close_update_graph(inner);
	}
     prev = current;
	 current = next; 
    }  
  // finish up
  while((*inner) && count_graph(*inner))
      close_update_graph(inner);  
  return(EV_Ok);
}
#undef Debug_parser
PGRAPH init_parser() {
  TABLE * t = get_table_name("console");
  new_table_graph(t); 
  return(t->list);
}

int parser(PGRAPH *pt) {
#ifdef Debug_parser
  for(;;)
#endif
	process_block(pt);
  return(EV_Ok);
}

