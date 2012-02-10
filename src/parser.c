
#include "all.h"
#include <ctype.h>
#define DISCARD 0xff
const char  *uglies = "\"._,{}$!:@=";
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
		return next->link;
		trio= find_trio(next->key);
	if(trio) { 
		if( (int) trio->type == G_TYPE_SYSTEM) {
			next->link =  (int) trio->value;
      *current = *next;
      return DISCARD;
    }
		else if( (int) trio->type == G_TYPE_BIT) 
			set_ready_event(EV_SystemEvent);
  }
		return next->link;
	}
// Front end key word from text and json operators
#define trim(tmp) while((tmp != start) && isspace(*(tmp-1))) tmp--;
#define white(v)while(isspace(*v)) v++;
int G_keyop(char * *Json,Triple *t) {
	enum { quote = 1,numeric =2};
	char * ptr = *Json;
  char * tmp;
	char *  start= ptr;
	int i=0;
	int events=0;
  if(*ptr == 0) {
    *t = G_null_graph;
				return -1;
  }
  // Key
	white(ptr);

	if(G_isdigit(*ptr) ) {
    t->key=ptr;
  while(G_isdigit(*ptr) || *ptr == '.') ptr++; 
  tmp = ptr;
  }
	else if(*ptr == uglies[QuoteSyntax]) {// Quote char?
		ptr++;
		t->key=ptr;
		while(*ptr  != uglies[QuoteSyntax])  ptr++;
    tmp = ptr;ptr++;    
	} else {
    	t->key=ptr;
    while(!G_isugly(*ptr) && (*ptr != 0) ) ptr++;
    trim(ptr);tmp = ptr;
}

white(ptr);

// operator
	if(G_isugly(*ptr)) 
      t->link = *ptr;
  else
    t->link = uglies[NullSyntax];
  *tmp = 0;
	i =   (int) ptr;
	i -= (int) start ; // char count
	*Json = ptr+1;
	if(!(t->key[0]))
		t->key = null_key;  // valid null key
	return i;
}
int new_jump(char cin, PGRAPH *inner);
// builds a subgraph on inner from user text
Triple prev,current,next;
unsigned char cprev,ccurr,cnext;
unsigned int ctest;
#define ParserHeader "table:parser"
int start_parser(char * Json, TABLE *table) {
	int nchars;
	PGRAPH *inner; // points to first child
	inner = (PGRAPH *) &table->list;
	nchars=0;cprev=1,ccurr=1,cnext=1;ctest=0;
  del_create_table(table);
  new_child_graph(inner,(void *) '@'); // Header block
  (*inner)->table=table;
  prev = G_null_graph;
  prev.key = ParserHeader;
	current.link = DISCARD;
  prev.link = DISCARD;
	while(nchars >= 0) {
    nchars = G_keyop(&Json,&next);
		next.pointer=1;
    
    //print_triple(&next);G_printf("\n");
    new_jump(next.link,inner);
	}  
	// finish up
	while((*inner) && count_graph(*inner))
		close_update_graph(inner);  
	return(EV_Ok);
}


void list_graphs(PGRAPH  *list);
#define Debug_parser
#ifdef Debug_parser
char * typeface[] = {
  "{def,hhh,ggg}",
  "{a=b.c=9. . c.f,d=ef}}",
  "{\"hello everyone\"}",
  "{ {abc.\"def\".jjj.\"kkk\".lll},rdf,'you'.klf,{ {named,kkk}.{fgh.lmk} }, jkl }",
  "{a,b,c}",
  "{local:SystemScript{\"select * from console;\"}}",
	"{abc,def,ghi}",
	"{@config}",
	""};
#define DLINE 3
static int debug_counter=DLINE;
int   parser(char * x,TABLE *table) {
 char buff[200]; 
 
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

int index_of(unsigned char *,PGRAPH * inner);
unsigned char  pt[16*4] = 
"xxxx"    // 0 no match
"\0.\0\0" // 1 Dot default append
"\0{\0\0"  // 2 new graph
",{\0\0" // 3 closeupdate, new graph
"\0,\0\0" //4  append, close update new_graph
"},\0\0" //5 closeupdate, new_graph 
"{,\0\0" //6 new graph append, close and update, new child graph
"\0:\0\0" //7 named
"\0}\0\0" //8 append closeupdate change op
",}\0\0" // 9  append, close update 
"}}\0\0" // 10 close update
"\0_\0\0" // 11 OPAQUE CARRIER   
"\0\xff\0\0" // 12 discard
"\0=\0\0" // 13 new graph append
 "\0\0\0\0";
#define ndx(a) a+i*4
int new_jump(char cin, PGRAPH *inner) {
 // prev point to a three element set, all characters in the ublgy set
  int hindex;
  cnext = cin;
  hindex = index_of(pt,inner);
  //G_printf("Case %d \n",hindex);
  switch(hindex) {
    //dot
  case 0:  case 12:
  break;
   case 1: // dot always appends
    append_graph(inner,current);
    break;
    // Brackets
  case 3: case 5:
    G_printf("35PC %c| ",parent_graph_context(inner));
     close_update_graph(inner); //follow through
  case 2:
    new_child_graph(inner,(void *) ccurr);
    break;
 case 4:
       append_graph(inner,current);
       G_printf("4P C %c|",parent_graph_context(inner));
        close_update_graph(inner);
    new_child_graph(inner,(void *) ccurr);
    break;
    case 13:
      new_child_graph(inner,(void *) ccurr);
      append_graph(inner,current);
      break;
  case 6:
   new_child_graph(inner,(void *) ccurr);
       append_graph(inner,current);
       G_printf("6 PC %c|",parent_graph_context(inner));
       close_update_graph(inner);
    new_child_graph(inner,(void *) ccurr);
    break;
case 7: 
    cnext = SetAttribute(&current,&next);
  case 8:case 9:
    current.link  = '_';
  case 20:
       append_graph(inner,current);
       G_printf("89 PC %c|",parent_graph_context(inner)); 
        close_update_graph(inner);
    break;
 case 10:
   G_printf("10 PC %c|",parent_graph_context(inner));
close_update_graph(inner);
  break;
  default:
   G_printf("  append default \n");
       append_graph(inner,current);
    break;
  }
  ctest = (ctest << 8) | cnext;
  cprev = ccurr; ccurr = cnext;
 // G_printf("%x %x %x %x\n",cprev,ccurr,cnext,ctest);
  prev = current;
	current = next; 
  return 0;
 }

int index_of(unsigned char * p,PGRAPH * inner) {
  int hindex,i;
  hindex = 0; 
  if((char )parent_graph_context(inner) == '=')
    return 20;
  for(i=hindex;i < 16;i++){
    if( ccurr == p[1]) {
      hindex = i; break;
    }
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
