
#include "all.h"
#include <ctype.h>
#define Debug_parser
#define DISCARD 256
const char  *uglies = "\"._,{}$!:@";
char * null_key = "_";
enum {QuoteSyntax,DotSyntax,NullSyntax,CommaSyntax,LeftSyntax,RightSyntax};
#define ESC 33
int isin(char c,const char *str);
int  G_isugly(char ch) { 
	return isin(ch,uglies);
}

// apply known attribute when detected in the input stream
int SetAttribute(Triple * current,Triple * next) {
	Trio * trio;
	if( G_strcmp(current->key,SystemNameSpace) )
		return 0;
	trio= find_trio(next->key);
	if(trio) { 
		if( (int) trio->type == G_TYPE_BIT) 
			next->link =  (int) trio->value | OperatorMSB;
		else if( (int) trio->type == G_TYPE_BIT)
			set_ready_event(EV_SystemEvent);
		else return 0;
		return 1;
		} else
		return 0;
	}
// Front end key word from text and json operators
int G_keyop(char * *Json,Triple *t) {
	enum { quote = 1,numeric =2};
	char * ptr = *Json;
	char *  start= ptr;
	int i=0;
	int events=0;
	while(isspace(*ptr)) ptr++;
	if(G_isdigit(*ptr) )
		events += numeric;
	if(*ptr == uglies[QuoteSyntax]) {// Quote char?
		ptr++;
		t->key=ptr;
		if(*ptr  != uglies[QuoteSyntax]) {
			events += quote;
			ptr++;
		}
	} else t->key=ptr;
	while(1) {
		if(G_isugly(*ptr) || (*ptr == 0) ) {
			if((events & numeric) && *ptr == uglies[DotSyntax])
				ptr++;
			else if(!(events & quote) && (*ptr == uglies[QuoteSyntax]) ) {
				*ptr = 0;ptr++; 
			} else { 
				char * tmp;
				tmp = ptr; 
				while((tmp != start) && isspace(*(tmp-1))) tmp--;
				if(isspace(*tmp)) *tmp = 0;
				break;
			}	
		}else {
			if(*ptr == 0)
				return -1;
			ptr++;
		}
	}
	if((*ptr == 0) || !G_isugly(*ptr) )
		t->link = uglies[NullSyntax];
	else
		t->link = *ptr;
	i =   (int) ptr;
	i -= (int) start ; // char count
	*ptr = 0;
	*Json = ptr+1;
	if(!(t->key[0]))
		t->key = null_key;  // valid null key
	return i;
}

// builds a subgraph on inner from user text
int parser(char * Json, PGRAPH *inner) {
	Triple prev,current,next;
	int nchars;
	append_graph(inner, G_null_graph);
	prev.link = DISCARD;
	current.link = DISCARD;
	nchars=0;
	new_child_graph(inner); // enclose this work in a subgraph
	while(*Json) {
			//G_memset(&next,0,sizeof(next));
		nchars = G_keyop(&Json,&next);
		//look_graph = *(*inner);
		next.pointer=(*inner)->row+1;
#ifdef Debug_parser
		G_printf("|%10s| |%c| |%4d|\n", next.key,next.link,next.pointer);
#endif
		// Replace the { ad the : if local name space
		// Handle local names immediately
		if(current.link == ':') {
			if( SetAttribute(&current,&next) ) 
				current.link = DISCARD;
			// Erase the bracket
		}else if(current.link == '{') { // _{ or key:{   or .{ or ,{ or key{ or !{
			if(isin(prev.link,"!:@"))
				new_child_graph(inner);
			current.link = DISCARD;
		} 
		// Check grammar
		else if(isin(current.link,"._@") || (current.link & OperatorMSB)){ // key. key_ key$ key.overload
			append_graph(inner,current);
		} else if(current.link == ',' ) {  // key,key or  key{key, or key.key, or :key,
			//
			if(prev.link != '{')
				close_update_graph(inner);
			new_child_graph(inner);
			append_graph(inner,current);
			// Case: key:X
		} else if(isin(current.link,":!@"))  {  // key: key!
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

PGRAPH * init_parser() {
  TABLE * t = get_table_name("console");
  new_table_graph(t); 
  return((PGRAPH *) &(t->list));
}

void graph_look(PGRAPH *  list);

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