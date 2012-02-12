
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
		trio= find_name(next->key);
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
    tmp = ptr; trim(tmp);
}

white(ptr);

// operator
	if(G_isugly(*ptr)) 
      t->link = *ptr;
  else
    t->link = uglies[NullSyntax];

	i =   (int) tmp;
	i -= (int) start ; // char count
	
	if(t->key == ptr)
		t->key = null_key;  // valid null key
  else
      *tmp = 0;
  *Json = ptr+1;
	return i;
}
int json_rules(char cin, PGRAPH *inner);
// builds a subgraph on inner from user text
Triple prev,current,next;
unsigned int cprev,ccurr,cnext;

#define ParserHeader "table:parser"
int start_parser(char * Json, TABLE *table) {
	int nchars;
	PGRAPH *inner; // points to first child
  G_printf("\nparse start\n");
	inner = (PGRAPH *) &table->list;
  *inner =0;
	nchars=0;cprev=1,ccurr=1,cnext=1;
  del_create_table(table);
  new_child_graph(inner,(void *) '@'); // Header block
  (*inner)->table=table;
  prev = G_null_graph;
  current.key = ParserHeader;
  current.link = '@';
  ccurr = '@';
	while(nchars >= 0) {
    nchars = G_keyop(&Json,&next);
		next.pointer=1;
    
    //print_triple(&next);
    json_rules(next.link,inner);
    // G_graph_counts();
     //G_printf("\n");
	}  
	// finish up
	while(*inner) 
    if(count_graph(*inner))
		close_update_graph(inner);
  else delete_graph(inner);

    G_printf("\nparse done\n");
	return(EV_Ok);
}


void list_graphs(PGRAPH  *list);
#undef Debug_parser
#ifdef Debug_parser
char * typeface[] = {
  "{def.ccc.bbb.kkk,hhh.lyu,ggg.aaa}",
  "{a=23. . c=9. . c.f,d=33}",
  "{\"hello everyone\"}",
  "{ {abc.\"def\".joe:jjj.\"kkk\".lll},anyname:{rdf,may},'you'.klf,{ {named,kkk}.{fgh.lmk} }, jkl }",
  "{a,b,c}",
  "{a=18,aaa=22. ,{vvv=40,555=2}.local:SystemExec{\"select * from console;\"}}",
	"{abc,def,ghi}",
	"{@config}",
	""};
#define DLINE 0
static int debug_counter=DLINE;
int   parser(char * x,TABLE *table) {
 char buff[200]; 
 
 G_memset(buff,0,sizeof(buff));
 G_strcpy(buff,typeface[DLINE]);
 G_printf("%s\n",buff);
 start_parser(buff,table);
// G_graph_counts();
  return EV_Ok;
}
#else
int   parser(char * x,TABLE *table) {
  return start_parser(x,table);
}
#endif
   enum {None,New,App,Del,AppClose,NewApp,DelApp,
     CloseNew,CloseNewApp,AppDel,AppDelClose,
     AppCloseNew,AppClosePrev,Name,Done};

   int graph_changes(PGRAPH *inner,int hindex) {
char * debugs[]= 
       {"None","New","App","Del","AppClose","NewApp","DelApp",
     "CloseNew","CloseNewApp","AppDel""AppDelClose",
     "AppCloseNew","AppClosePrev","Name"};
   // G_printf("Case: %s ",debugs[hindex]);
   // G_printf("p %x c %x n %x\n",cprev,ccurr,cnext);

    switch(hindex) {
    case None: 
       break;
    case App: // dot always appends
       append_graph(inner,current);
      break;
    case New: // dot always appends
       new_child_graph(inner,(void *) ccurr);
      break;
      case Del:
     delete_graph(inner);
     break;
   case AppClose:
     append_graph(inner,current);
     close_update_graph(inner);
     break;
  case NewApp:
     new_child_graph(inner,(void *) ccurr);
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
 case CloseNewApp:
     close_update_graph(inner);
     new_child_graph(inner,(void *) ccurr);
     append_graph(inner,current);
     break;
 case AppDel:
        append_graph(inner,current);
     delete_graph(inner);
     break;
 case AppDelClose:
     append_graph(inner,current);
     delete_graph(inner);
     close_update_graph(inner);
     break;
  case AppClosePrev:
     append_graph(inner,current);
     new_child_graph(inner,(void *) cprev);
     close_update_graph(inner);
     break;
 case AppCloseNew: // next Coma element 
     append_graph(inner,current);
     close_update_graph(inner);
     new_child_graph(inner,(void *) ccurr);
     break;

   case Name: 
     SetAttribute(&current,&next);
     new_child_graph(inner,(void *) ccurr);
     append_graph(inner,current);
     break;
   }
   //G_printf("%dPC %c %c|",hindex,ccurr,graph_variable(inner));
   // G_printf("p %x c %x n %x\n",cprev,ccurr,cnext);
    return 0;
 }
int json_rules(char cin, PGRAPH *inner) {
// Some combinations vcan be ignored
   if((ccurr == '.') && (cprev == '}') ) { // this combination just skipped
     ccurr = cin;
     current = next;
        return 0;
   }
  if((ccurr == ',') && (cprev == '}')) {
     graph_changes( inner,CloseNew);
     ccurr = cin;
     current = next;
  return 0;
  }
   cnext = cin;
   if(ccurr == '{')   
       graph_changes( inner,New); 
   // One append through this series
   if( (cprev == ':') ||  (cprev == '=') )
     graph_changes( inner,AppCloseNew);
   if((ccurr == '.') && !( (cprev == ':') ||  (cprev == '=') ))
     graph_changes( inner,App);
   else if((ccurr == ',') && !( (cprev == ':') ||  (cprev == '=') ))
       graph_changes( inner,AppCloseNew);
   else if((ccurr == ',') && ( (cprev == ':') ||  (cprev == '=') ))
     graph_changes( inner,CloseNew);
   else if(ccurr == '}') {  
     int child_context = (int ) graph_variable(*inner);
     int parent_context = (int ) graph_variable((*inner)->parent);
     if( (cprev == ':') ||  (cprev == '=') ) graph_changes(inner,Del);
     else if(parent_context == '{') graph_changes(inner,AppDel);
     else graph_changes(inner,AppCloseNew);
     }
   else if (ccurr == ':') 
       graph_changes( inner,NewApp); // Named
   else  if(ccurr == '=') graph_changes( inner,NewApp); // Equals pair
   else if(ccurr == '@')  graph_changes( inner,NewApp); 
       // Just set up potential compound object
 
  // print_triple(&current);
  //  G_graph_counts();
   //     G_printf("\n");
    cprev = ccurr; ccurr = cnext;
   prev = current;
   current = next; 
   return 0;
 
 }
