
/* This file contains all the routines to bind prepared 
and installed statements, and it prepares the table
for insert, update and selectg.  
Table stuff really doesn belong here, as table are going to be an
an index system built on the sqlite_master
*/
#include "all.h"
#include "filter.h"


#define NUMBINDS 10
int bind_count=0;
// various binds
Mapper null_map(Pointer  pointer,int  *type) { 
	*type = G_TYPE_NULL;
	return 0;}
Mapper map_triple(Pointer *pointer,int *type) {
	*type = G_TYPE_TRIPLE;
	return 0;}


int local_handler(Triple t) {
	char buffer[200];
  G_sprintf(buffer + G_strlen(buffer)," %s ,",t.key);
  return EV_Ok;
}

// Get bound up for a n sql step
 void look_stmt(Code stmt) ;
  int bind_sql(Triple top[],Code *stmt) {
  int status=EV_Ok;
  Pointer pointer;
  int type;
  int index;
  Mapper * a;
  // Get and set the stmt
  	if(operands[top[0].link].properties & EV_Overload)
		*stmt = (Code) top[0].key;
	else 
		*stmt = operands[top[0].link].stmt; // operand table rules
	if(!stmt)
		return (0);
	look_stmt(*stmt);
	a = operands[top[0].link].maps;
	index =1;
	while(*a) {
		(*a) ( (Pointer *) &pointer,&type);
		switch(type) {
			case G_TYPE_TEXT:
				status = machine_bind_text(*stmt,index++,(char *) pointer);
			break;
			case G_TYPE_INTEGER:
				status = machine_bind_int(*stmt,index++,(int) pointer);
			break;
			case G_TYPE_TRIPLE:
				status = machine_bind_text(*stmt,index++,top[1].key);
				status = machine_bind_int(*stmt,index++,top[1].link);
				status = machine_bind_int(*stmt,index++,top[1].pointer);
				look_stmt(*stmt);
			break;
			case G_TYPE_CODE:
				G_debug(*stmt);
				*stmt = 0; // cancel this event
				break;
			}
		a++;
		} 
  return EV_Ok;
}
  // tios local to binds
  Trio bind_trios[] ={ {"BindNull",G_TYPE_MAPPER,(Mapper) null_map},
{"BindTriple",G_TYPE_MAPPER,(Mapper) map_triple},
{0,0,0}};
  int init_binder() {add_trios(bind_trios);return(0);}
 void look_stmt(Code stmt) {
	 char buff[200];
	  G_sprintf(buff,"Script: \n%s\n",machine_script(stmt));
  }
