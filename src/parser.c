
#include "all.h"
#include <ctype.h>
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
int new_jump(char cin, PGRAPH *inner);
// builds a subgraph on inner from user text
Triple prev,current,next;
char cprev,ccurr,cnext;
int start_parser(char * Json, PGRAPH *inner) {
	int nchars;
	append_graph(inner, G_null_graph);
	prev.link = DISCARD;
	current.link = DISCARD;
	nchars=0;cprev=0,ccurr=0,cnext=0;
	new_child_graph(inner); // enclose this work in a subgraph
	while(*Json) {
		nchars = G_keyop(&Json,&next);
		next.pointer=(*inner)->row+1;
    new_jump(next.link,inner);
		G_printf("|%10s| |%c| |%4d|\n", next.key,next.link,next.pointer);

	}  
	// finish up
	while((*inner) && count_graph(*inner))
		close_update_graph(inner);  
	return(EV_Ok);
}
//#define Debug_parser
#define Debug_parser

PGRAPH * init_parser() {
  TABLE * t = get_table_name("console");
  new_table_graph(t); 
  return((PGRAPH *) &(t->list));
}


#ifdef Debug_parser
char * typeface[] = {
	"{abc.def,{fgh.lmk},jkl}",
	"{local:SystemEcho}",
	"{abc,def,ghi}",
	"{@config}",
	""};
#define DLINE 0
static int debug_counter=DLINE;
int   parser(char * x,PGRAPH *inner) {
 char buff[200]; char  u;int i;
 i=0;while(uglies[i]) { G_printf(" %2x  %c ",uglies[i],uglies[i]);i++;}
u  = uglies[0]; 

 G_memset(buff,0,sizeof(buff));
 G_strcpy(buff,typeface[DLINE]);
  return start_parser(buff,inner); }
#else
int   parser(char * x,PGRAPH *inner) {
  return start_parser(x,inner);
}
#endif

char  pt[16*4] = 
"____" // no match
"\0.\0\0" // 1 Dot always append
"____" // 2 spare
".{\0\0"  //3  brackets open on dot (dot gives up pointer)
":{\0\0" // 4 bracket skips, colon opens
",{\0\0" // 5 comma close update and  append, bracket skips 
"{,\0\0" //6 comma append, close and update, new child graph
"{,.\0" //  7 comma append, close and update. dot append only
".,\0\0" //8 dot always appends omma append then close update
"},}\0" //9 first bracket bracket skips, comma close update, no append
"\0:\0\0" //10
"\0}\0\0" // 11 close update
 "\0\0\0\0";
#define ndx(a) a+i*4
int new_jump(char cin, PGRAPH *inner) {
 // prev point to a three element set, all characters in the ublgy set
  int hindex;
  char *p; 
  int i;
  cnext = cin;
  hindex = 0; p = pt;
  if(ccurr) {
  for(i=0;i < 16;i++)  { 
      if( ccurr == p[1]) 
        break;
      p+=4;
    } 
  if(ccurr == p[1] && cprev) { 
        hindex = i;
        for(i= hindex; i < 16;i++,p+=4,cprev){
          if(cprev != p[0]  && ccurr == p[1])
          continue;
        } 
        if(cprev == p[0] && ccurr == p[1] ) {
            hindex = i;
            if(cnext)
            for(i= hindex; i < 16;i++,p+=4,cnext) { 
            if(cprev == p[0] && ccurr == p[1] && cnext != p[2]) 
              continue;
            if(cprev == p[0] && ccurr == p[1] && cnext == p[2]) 
            hindex = i;
            }
       }
    }
  }

  G_printf("Current %c %d ",ccurr,hindex);
  switch(hindex) {
    //dot
  case 0:case 2:
  break;
  case 1: // dot always appends
    G_printf("   append\n");
    append_graph(inner,current);
    break;
    // Brackets
  case 3:case 4:case 5: 
    G_printf("   discard\n ");
    current.link = DISCARD;
    break;
    //Comma   if(prev.link != '{'
  case 6:case 7: case 8:
    G_printf("   Close update\n ");
    close_update_graph(inner);
 case 9: 
    G_printf("   New and  append\n ");
    new_child_graph(inner);
    append_graph(inner,current);
 case 10:
       G_printf("   Named\n ");
    SetAttribute(&current,&next); 
    current.link = DISCARD;
    break;
    case 11:
   //closing bracket
    G_printf("Closing bracket\n");
    close_update_graph(inner);
    break;
  default:
    new_child_graph(inner);
    break;
  }
  cprev = ccurr; ccurr = cnext;
  prev = current;
	current = next; 
  return 0;
}
