
#include "all.h"
#include <ctype.h>
#define Debug_parser
#define DISCARD 256
#define Line_size 256
typedef char * CharPointer;

int graph_counter;

// apply known attributes using name substitution
int SetAttribute(Triple * current,Triple * next) {
	Trio * trio;
	if( G_strcmp(current->key,SystemNameSpace) )
		return 0;
	trio= find_trio(next->key);
	if(trio && (int) trio->type == G_TYPE_SYSTEM) {
		next->link =  (int) trio->value;
		return 1;
	}
	return 0;
}
// builds a subgraph on inner from user text

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
	  G_printf("|%10s |%2x-%c | %4d|\n", next.key,next.link,next.link,next.pointer);
#endif
	  // Replace the { ad the : if local name space
	  // Handle local names immediately
	  if((current.link == ':') && SetAttribute(&current,&next) ) {
		  current.link = DISCARD;
		  // Erase the bracket
	  }else if(current.link == '{') { // _{ or key:{   or .{ or ,{ or key{ or !{
		  if(prev.link != ':' || prev.link != '!')
			  new_child_graph(inner);
		  current.link = DISCARD;
	  } 
	// Check grammar
	 else if((current.link == '.') || (current.link == '_')
		|| (current.link == '$')){ // key. key_ key$
		append_graph(inner,current);
	} else if(current.link == ',' ) {  // key,key or  key{key, or key.key, or :key,
//
		if(prev.link != '{')
		close_update_graph(inner);
		new_child_graph(inner);
		append_graph(inner,current);
// Case: key:X
	} else if((current.link == ':') || (current.link == '!'))  {  // key: key!
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
#define Debug_parser
#undef Debug_parser

PGRAPH init_parser() {
  TABLE * t = get_table_name("console");
  new_table_graph(t); 
  return(t->list);
}
int Graph_test(PGRAPH *pt);
void graph_look(PGRAPH *  list);
int parser(PGRAPH *pt) {
#ifdef Debug_parser
  Graph_test(pt);
#else
	process_block(pt);
#endif
	G_buff_counts();
  return(EV_Ok);
}
//const  Triple G_null_graph 
#ifdef Debug_parser 
void graph_look(PGRAPH *  list) {
	PGRAPH  graph = (*list);
	GRAPH g ;
	g = *graph;
	return;
}
int Graph_test(PGRAPH *pt) {

	int i;
	Triple t= {"_",'_',0};
	for(i=0;i < 5;i++ ){
		graph_look(pt);
	append_graph(pt,t);
}
	graph_look(pt);
	while((*pt) && count_graph(*pt)){
		graph_look(pt);
      close_update_graph(pt);  
	  graph_look(pt);
	}
  return(EV_Ok);
}
#endif