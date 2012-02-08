
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
  if(*ptr == 0) {
    *t = G_null_graph;
				return -1;
  }
	while(isspace(*ptr)) ptr++;
	if(G_isdigit(*ptr) )
		events += numeric;
	else if(*ptr == uglies[QuoteSyntax]) {// Quote char?
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
		} else 
			ptr++;
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
unsigned char cprev,ccurr,cnext;
#define ParserHeader "table:parser"
int start_parser(char * Json, TABLE *table) {
	int nchars;
	PGRAPH *inner; // points to first child
	inner = (PGRAPH *) &table->list;
	nchars=0;cprev=1,ccurr=1,cnext=1;
	new_child_graph(inner); // Header block
  (*inner)->table=table;
  prev = G_null_graph;
  prev.key = ParserHeader;
  append_graph(inner,prev);
	current.link = DISCARD;
  prev.link = DISCARD;
	while(nchars >= 0) {

    nchars = G_keyop(&Json,&next);
		next.pointer=1;
    new_jump(next.link,inner);
	}  
	// finish up
	while((*inner) && count_graph(*inner))
		close_update_graph(inner);  
	return(EV_Ok);
}


void list_graphs(PGRAPH  *list);
#undef Debug_parser
#ifdef Debug_parser
char * typeface[] = {
  "{ {abc.def.jjj.kkk.lll},rdf,you.klf,{ {named,kkk}.{fgh.lmk} }, jkl }",
  "{a,b,c}",
	"{local:SystemEcho}",
	"{abc,def,ghi}",
	"{@config}",
	""};
#define DLINE 1
static int debug_counter=DLINE;
int   parser(char * x,TABLE *table) {
 char buff[200]; 
 //int i;
 //i=0;while(uglies[i]) { G_printf(" %2x  %c ",uglies[i],uglies[i]);i++;}


 G_memset(buff,0,sizeof(buff));
 G_strcpy(buff,typeface[DLINE]);
 G_printf("%s\n",buff);
 start_parser(buff,table);
 G_buff_counts();
  return EV_Ok;
}
#else
int   parser(char * x,TABLE *table) {
  return start_parser(x,table);
}
#endif
int index_of(pt);
char  pt[16*4] = 
"xxxx"    // 0 no match
"\0.\0\0" // 1 Dot always append, then discard
"\0{\0\0"  // 2 new graph
",{\0\0" // 3 closeupdate, new graph
"\0,\0\0" //4  append, close update new_graph
"},}\0" //5 closeupdate, new_graph 
"{,\0\0" //6 new graph append, close and update, new child graph
"\0:\0\0" //7 named
"\0}\0\0" //8 append closeupdate change op
".}\0\0" // 9 append closeupdate change op
",}\0\0" // 10 append closeupdate(twice)change op
 "\0\0\0\0";
#define ndx(a) a+i*4
int new_jump(char cin, PGRAPH *inner) {
 // prev point to a three element set, all characters in the ublgy set
  int hindex;
  cnext = cin;
  hindex = index_of(pt);
  //G_printf("Case %d \n",hindex);
 // G_printf(" |%c|%c|%c| ",pt[4*hindex],pt[4*hindex+1],pt[4*hindex+2]);
  switch(hindex) {
    //dot
  case 0:
  break;
  case 1: // dot always appends
    //G_printf("   append ");
    append_graph(inner,current);
    break;
    // Brackets
  case 3: case 5:
     close_update_graph(inner); //follow through
  case 2:
    new_child_graph(inner);
    break;
 case 4:
    //  G_printf("   append close  ");
     //  print_triple(&current);
       append_graph(inner,current);
        close_update_graph(inner);
    new_child_graph(inner);
    break;
  case 6:
     // G_printf("   append continue  ");
      // print_triple(&current);
   new_child_graph(inner);
       append_graph(inner,current);
       close_update_graph(inner);
    new_child_graph(inner);
    break;
case 7: 
    if(SetAttribute(&current,&next)) 
        current.link = DISCARD;
    else {
      append_graph(inner,current);
      new_child_graph(inner);
    }
  case 8:

  case 9:
    //  G_printf("   append close  ");
     //  print_triple(&current);
    current.link  = '_';
       append_graph(inner,current);
        close_update_graph(inner);
    break;
 case 10:
   current.link  = '_';
       append_graph(inner,current);
        close_update_graph(inner);
        close_update_graph(inner);
         break;
  default:
   G_printf("  append default ");
   //    print_triple(&current);
       append_graph(inner,current);
    break;
  }

  cprev = ccurr; ccurr = cnext;
  prev = current;
	current = next; 
   //G_buff_counts();
     G_printf("\n");
  return 0;
 }

int index_of(char * p) {
  int hindex,i;
  hindex = 0; 
  for(i=hindex;i < 16;i++){
    if( ccurr == p[1]) {hindex = i; break;};
    p+=4;
  } 
  if(ccurr == p[1]) { 
    for(i=hindex;i < 16;i++){
      if(cprev == p[0]  && ccurr == p[1]){hindex = i; break;};
      if(ccurr != p[1]) break;
      p+=4;
    } 
    if(cprev == p[0] && ccurr == p[1]) {
      for(i=hindex;i < 16;i++){
        if(cprev == p[0] && ccurr == p[1] && cnext == p[2]) {hindex = i; break;};
        if(cprev != p[0] || ccurr != p[1]) break; 
        p+=4;
      }
    }
  }
    return hindex; 
  }
