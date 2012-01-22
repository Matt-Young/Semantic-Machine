
/* This file contains all the routines to bind prepared 
and installed statements, and it prepares the table
for insert, update and selectg.  
Table stuff really doesn belong here, as table are going to be an
an index system built on the sqlite_master
*/
#include "../include/sqlite3.h"
#include "all.h"

extern OP operands[];
extern M m;
#define NUMBINDS 10
int bind_count=0;
NamedAccessor binders[NUMBINDS];

int new_binders(NamedAccessor pairs[]) {
	int i=0;
	char *  p;
	NamedAccessor a;
	while(pairs[i].name != 0) {
		a = pairs[i];
		p =  (char * )G_malloc(G_strlen(a.name));
		G_strcpy((char *) p,a.name);
		binders[bind_count].name = p;
		binders[bind_count].mapper = a.mapper;
		bind_count++;
		i++;
	}
	return 0;
}
Mapper  find_binder(char * name) { 
	int i;
	for(i=0; binders[i].name;i++) 
		if(!G_strcmp(binders[i].name,name))
			return((Mapper) binders[i].mapper);
	return 0;}
// various binds
Mapper null_map(Pointer  p,int * i) { 
	return 0;}
Mapper map_triple(Pointer *j,int *type) {
	*type = G_TRIPLE;
	return 0;}
NamedAccessor a[] ={ {"null",(Mapper) null_map},{"BindTriple",(Mapper) map_triple},{0,0}};
int init_binders() {
	bind_count=0;
	new_binders(a);
	return 0;
}

char buffer[200];
int index = 0;
int local_handler(TRIPLE t) {
  G_sprintf(buffer + G_strlen(buffer)," %s ,",t.key);
  return SQLITE_OK;
}


// Get bound up for a n sql step
  int bind_sql(TRIPLE top[],Code *stmt) {
  int status=SQLITE_OK;
  int i=0,j;
  Mapper * a;
  // Get and set the stmt
  	if(operands[top[0].link].properties & EV_Immediate)
		*stmt = (sqlite3_stmt *) top[0].key;
	else if( operands[top[0].link].properties & EV_Operand)
		*stmt = operands[top[0].link].stmt; // operand table rules
	else
		return (0);
	a = operands[top[0].link].maps;
	while(*a) {
		(*a) ( (Pointer *) &i,&j);
		switch(j) {
			case SQLITE_TEXT:
				status = sqlite3_bind_text(*stmt,index++,
				(char * ) i, G_strlen((char * ) i),0);
			break;
			case SQLITE_INTEGER:
				status = sqlite3_bind_int(*stmt,index++,i);
			break;
			case G_TRIPLE:
				status = sqlite3_bind_text(*stmt,index++, 
					(char * ) top[1].key, G_strlen(top[1].key),0);
				status = sqlite3_bind_int(*stmt,index++,top[1].link);
				status = sqlite3_bind_int(*stmt,index++,top[1].pointer);
			break;
			}
		a++;
		} 
  return SQLITE_OK;
}

   /* {
	  G_sprintf(buff,"Script: \n%s\n",sqlite3_sql(stmt));
	  status = sqlite3_bind_text(stmt,1,"abc",3,0);
  }*/