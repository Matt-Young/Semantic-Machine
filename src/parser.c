
#include "../src/include/g_types.h"

#include "./include/machine.h"
#include "../src/include/tables.h"
#include "../src/include/graphs.h"
#include "../src/include/console.h"
#include "../src/include/names.h"
#include <ctype.h>

#define DISCARD 0xff
const char  *uglies = "\"._,{}$!:@";
char * null_key = "_";
enum {QuoteSyntax,DotSyntax,NullSyntax,CommaSyntax,LeftSyntax,RightSyntax};
#define ESC 33
int isin(char c,const char *str);
int  G_isugly(char ch) { 
  return isin(ch,uglies);
}
int set_ready_event(int );
// apply known attribute when detected in the input stream
int SetAttribute(char * key) {
  Trio * trio;
  if( G_strcmp(key,SystemNameSpace) )
    return 0;
  trio= find_name(key);
  if(trio) {
    if( (int) trio->type == G_TYPE_BIT) 
      set_ready_event((int) trio->value);
      return 1;
  }
  return 0;
}
// Front end key word from text and json operators
#define trim(tmp) while((tmp != (*key)) && isspace(*(tmp-1))) tmp--;
#define white(v)while(isspace(*v)) v++;
int G_keyop(char * *Json,char **key,int *link) {
  enum { quote = 1,numeric =2};
  char * ptr = *Json;
  char * end;
  int i=0;
  int events=0;

  // Key
  white(ptr);
  if(*ptr == 0)
    return -1;
  end = ptr;
  if(G_isdigit(*ptr) ) {
    *key=ptr;
    while(G_isdigit(*ptr) || *ptr == '.') ptr++; 
    end = ptr;
  }
  else if(*ptr == uglies[QuoteSyntax]) {// Quote char?
    ptr++;
    *key=ptr;
    while(*ptr  != uglies[QuoteSyntax])  ptr++;
    end = ptr;ptr++;    
  } else {
    *key=ptr;
    while(!G_isugly(*ptr) && (*ptr != 0) ) ptr++;
    end = ptr; trim(end);
  }
  i =   (int) end;
  i -= (int) (*key) ; // char count

  if(i==0)
    *key = "_";  // valid null key

   white(ptr);
  if((*ptr == 0)|| !G_isugly(*ptr) ){
    *link = '_';
    return -1;
  }
    // next operator
   *link = *ptr;
   *end = 0;
ptr++;
  *Json = ptr;
  return i;
}
int json_rules(char cin, PGRAPH *inner);
// builds a subgraph on inner from user text
Triple prev,current,next;
unsigned int cprev,ccurr,cnext;

#define ParserHeader "parser"
int parser(char * Json, TABLE *table) {
  int link;int nchars=0;
  PGRAPH *inner; // points to first child
  G_printf("\nparse start %s\n",Json);
  inner = (PGRAPH *) &table->list;
  link=0;cprev=1,ccurr=1,cnext=1;
  (*inner)->context=(void *) '_';
  (*inner)->table=table;
  prev = _null_graph;
  current.key = ParserHeader;
  current.link = '_';
  link = '_';
  ccurr = '_';
  while(nchars >= 0) {
    next.link = link;
    nchars = G_keyop(&Json,&next.key,&link);
    next.pointer=1;
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

int json_rules(char cin, PGRAPH *inner) {
  // Some combinations can be ignored
  int child_context;
  child_context = (int ) graph_variable(*inner);
  cnext = cin;
  if(ccurr == '$') 
    if(SetAttribute(current.key))
      return 0;  // this local signal handled
  if(ccurr == '{')
    new_child_graph(inner,(void *) ccurr);
  // Name:Value, Name@V1,v2 
  if ((cnext == ':') || (cnext == '@')) 
    new_child_graph(inner,(void *) cnext);

  // One append through this series
  if( (ccurr == ':')   ) {
    append_graph(inner,current);
    close_update_graph(inner);
  }
  if(ccurr == ',') {
    if(child_context == '@'){

      (*inner)->context =(void *) ',';
    }
    else   {
      close_update_graph(inner);
      new_child_graph(inner,(void *) ccurr);
      append_graph(inner,current);
    }
  } else if((ccurr == '.') ||  (ccurr == '$')||  (ccurr == '{'))// yyy,xxx.xxx

    append_graph(inner,current);
  if(ccurr == '}') {  
    int parent_context = (int ) graph_variable((*inner)->parent);
    if(parent_context == '{')     delete_graph(inner);
    else {
      close_update_graph(inner); //follow through
      new_child_graph(inner,(void *) ccurr);
    }
  } 

  cprev = ccurr; ccurr = cnext;
  prev = current;
  current = next; 
  return 0;

}
