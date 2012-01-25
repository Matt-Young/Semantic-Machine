#include "sqlite_msgs.h"
#include "all.h"
#include <ctype.h>
#define Debug_parser
#define DISCARD 256
#define Line_size 256
typedef char * CharPointer;
char * null_key = "_";
int graph_counter;
// Is it a character known to the syntax? '
const char  *uglies = "'._,{}$"; // minuse the quote
#define Dot uglies[1]
#define Quote uglies[0]
char  isugly(char ch) { 
	const char * ptr = uglies;
	while((*ptr) && ((*ptr) != ch)) ptr++;
		return *ptr;}

// Front end key word from text and json operators
int key_op(Console * console,CharPointer *key,int * op) {
	enum { quote = 1,numeric =2};
      int i;
	  CharPointer ptr= console->current;
	  int events;
	  if(*ptr == 0) {
		  	console->current = console->base;
			ptr = console->current;
			G_console(console);  // get a line of data
			if(console->base[0] == 0)
				return -1;
	  }
	  i = 0; events=0;
	  while(isspace(*ptr)) ptr++;
	  if(G_isdigit(*ptr) )
		  events += numeric;
	  if(*ptr == Quote) {// Quote char?
		  ptr++;
		  *key=ptr;
		  if(*ptr  != Quote) {
			  events += quote;
		  ptr++;
		  }
	  } else *key=ptr;
	  while(1) {
		if(isugly(*ptr) || (*ptr == 0) ) {
			if((events & numeric) && *ptr == Dot)
				ptr++;
			else if(!(events & quote) && (*ptr == Quote) ) {
				*ptr = 0;ptr++; 
			} else {
				console->current = ptr+1;
				while((ptr != console->base) && isspace(*(ptr-1))) ptr--;
				break;
			}	
		  }else {
			ptr++;
		  	if(ptr >= &console->base[console->size])
				return -1;
			}
		}
		*op = (int) *console->current;
		i = (int) ptr - (int) (*console->current); // char count
		*ptr = 0;
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
  Console console;
  int nchars,named_count;
  char line[Line_size];
  G_InitConsole(&console,line,Line_size);
  prev.link = DISCARD;
  current.link = DISCARD;
  G_memset(line,0,Line_size);
  named_count=0;
  new_graph(inner); // enclose this work in a subgraph
  for(;;) {
	  nchars =key_op(&console,&next.key,&next.link);
      if(nchars < 0)
		break;
      if(next.link == 0) next.link='_'; // default operator
	  if(!G_strlen(next.key) ) 
			next.key = null_key;  // valid null key
	  next.pointer=(*inner)->row+1;
#ifdef Debug_parser
	  if(1) {
		  G_printf("{%s  %c  %d}  ", next.key,next.link,next.pointer);
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

PGRAPH init_parser() {
	int status;
  TABLE * t = get_table_name("console");
  status = new_table_graph(t); 
  return(t->list);
}
int parser(PGRAPH *pt) {
#ifdef Debug_parser
  for(;;)process_block(pt);
#else
    return process_block(pt);
#endif
  return(SQLITE_OK);
}

