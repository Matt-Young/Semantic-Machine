
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
	if(trio) 
    if( (int) trio->type == G_TYPE_BIT) 
			set_ready_event(EV_SystemEvent);
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

#define ParserHeader "table:parser"
int start_parser(char * Json, TABLE *table) {
	int nchars;
	PGRAPH *inner; // points to first child
	inner = (PGRAPH *) &table->list;
	nchars=0;cprev=1,ccurr=1,cnext=1;
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
  print_trios();
	return(EV_Ok);
}


void list_graphs(PGRAPH  *list);
#define Debug_parser
#ifdef Debug_parser
char * typeface[] = {
  "{def,hhh,ggg}",
  "{a=23. . c=9. . c.f,d=33}",
  "{\"hello everyone\"}",
  "{ {abc.\"def\".joe:jjj.\"kkk\".lll},anyname:{rdf,may},'you'.klf,{ {named,kkk}.{fgh.lmk} }, jkl }",
  "{a,b,c}",
  "{local:SystemScript{\"select * from console;\"}}",
	"{abc,def,ghi}",
	"{@config}",
	""};
#define DLINE 1
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
void * set_parent_graph_context(PGRAPH * inner,void * val) { 
  if(*inner) (*inner)->context=val; else  val =0;
  return val;
}
int json_rules(PGRAPH * inner);
   enum {None,Del,App,Close,New,AppClose,NewApp,
     DelApp,CloseNew,NewNewApp,AppCloseNew,CloseNewApp,CloseNewAppClose,Name};
 int new_jump(char cin, PGRAPH *inner) {
   // prev point to a three element set, all characters in the ublgy set
   int hindex;
   cnext = cin;
   hindex = json_rules(inner);
   //G_printf("Case %d \n",hindex);

   switch(hindex) {
   case None: 
     break;
   case Del:
     delete_graph(inner);
     break;
   case App: // dot always appends
     append_graph(inner,current);
     break;
   case Close: // dot always appends
     close_update_graph(inner);
     break;
   case New:
     new_child_graph(inner,(void *) ccurr);
     break;
   case AppClose:
     append_graph(inner,current);
     close_update_graph(inner);
     break;
  case NewApp:
     new_child_graph(inner,(void *) '_');
     append_graph(inner,current);
     break;
case DelApp:
     delete_graph(inner);
     append_graph(inner,current);
     break;
 case CloseNew: 
     close_update_graph(inner); //follow through
     new_child_graph(inner,(void *) ccurr);
     break;
 case NewNewApp:
     new_child_graph(inner,(void *) '_');
     new_child_graph(inner,(void *) ccurr);
     append_graph(inner,current);
     break;
 case AppCloseNew: // next Coma element 
     append_graph(inner,current);
     close_update_graph(inner);
     new_child_graph(inner,(void *) ccurr);
     break;
case CloseNewApp: // next Coma element
     close_update_graph(inner);
     new_child_graph(inner,(void *) ccurr);
     append_graph(inner,current);
     break;
 case CloseNewAppClose: // next Coma element
     close_update_graph(inner);
     new_child_graph(inner,(void *) ccurr);
     append_graph(inner,current);
     close_update_graph(inner);
     break;
   case Name: 
     SetAttribute(&current,&next);
     new_child_graph(inner,(void *) ccurr);
     append_graph(inner,current);
     break;

   default:
     G_printf("  append default \n");
     append_graph(inner,current);
     break;
   }
   //G_printf("%dPC %c %c|",hindex,ccurr,parent_graph_context(inner));
   cprev = ccurr; ccurr = cnext;
   // G_printf("%x %x %x %x\n",cprev,ccurr,cnext,ctest);
   prev = current;
   current = next; 
   return 0;
 }

int json_rules(PGRAPH * inner) {
  char context = (char ) parent_graph_context(inner);
  int action=0;
if(cprev == '=') return AppClose;  // append, close_update 
else if((ccurr == '.') && (cnext == '{')) return NewApp; // append potential temp
else if( ((cprev == ':') ||  (cprev = ',') || (cprev == '.') )
  && (ccurr == '{')) return None; // do nothing
else if((cprev = 0) && (ccurr == '{')) return New; // open potential or
else if((ccurr == '}') && (context == '_')) return DelApp; //context is a potential or, delete it. 
else if((ccurr == '}') && context =='.') return DelApp; // real and sequence delete context,Append
// Comma: close_update, new_graph, append, close update. Coma context
else if((ccurr == '}') &&(context == ',')) return CloseNewAppClose; 
else if(( cprev == ',') &&(ccurr == ',')) return CloseNewApp; // close_update, new_graph, append.
else if((ccurr == '}') &&(context == ':')) return Close; //close and update it
else if(( cprev == '.') &&(ccurr == ',')) return AppCloseNew; // append, close and update ,new graph
else if(( cprev == '{') &&(ccurr == ',')) return NewApp; //  new_graph, append
else if((cprev == '{') &&(ccurr == '=')) return NewNewApp; // open potential temp and, open equals context append}
else if((ccurr == '=')) return NewApp; // open potential temp and, open equals context append}
else if(ccurr == ':') return Name; // open naming context, append
else return 0;
}
