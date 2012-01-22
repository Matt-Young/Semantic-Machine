
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
// various binds
Mapper null_map(Pointer  p,int * i) { 
	return 0;}
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
Mapper map_triple(Pointer *j,int *type) {
	*type = G_TRIPLE;
	return 0;}
NamedAccessor a[] ={ {"null",(Mapper) null_map},{"BindTriple",(Mapper) map_triple},{0,0}};
int init_binders() {
	bind_count=0;
	new_binders(a);
	return 0;
}
int bind_triple(sqlite3_stmt *stmt,TRIPLE top[]) {
  int status; char buff[100];
  PGRAPH g = self_graph();
 
  if(top[1].key) {
	status = sqlite3_bind_text(stmt,1,
	top[1].key,
	G_strlen(top[1].key),0);
  } else // always forcing the default where missing 
  {
	  G_sprintf(buff,"Script: \n%s\n",sqlite3_sql(stmt));
	  status = sqlite3_bind_text(stmt,1,"abc",3,0);
  }
  status = sqlite3_bind_int(stmt,2,top[1].link);
  status = sqlite3_bind_int(stmt,3,top[1].pointer);
  return status;
  }
char buffer[200];
int index = 0;
int local_handler(TRIPLE t) {
  G_sprintf(buffer + G_strlen(buffer)," %s ,",t.key);
  return SQLITE_OK;
}

int bind_graph(sqlite3_stmt *stmt,PGRAPH g,int i,int j) {
  int status;
  if( j== BIND_GRAPH_ROW)
    status = sqlite3_bind_int(stmt,i+1,g->row);
  else if(j == BIND_GRAPH_START)
    status = sqlite3_bind_int(stmt,i+1,g->start);
  else if(j == BIND_GRAPH_END)
    status = sqlite3_bind_int(stmt,i+1,g->end);
  return status;
}
int bind_update_var(sqlite3_stmt *stmt,TRIPLE top[]) {
  PGRAPH g;
  int status;
  int index = (int) top[1].key;
  g = get_table_graph(index);
  status = bind_graph(stmt,g,0,BIND_GRAPH_ROW);
  status = bind_graph(stmt,g,1,BIND_GRAPH_START);
  return status;
}
// Get bound up for a n sql step
  int bind_sql(TRIPLE top[],Code *stmt) {
  int status=SQLITE_OK;
  int i=0,j;
  TRIPLE * triple;

  OP * op = &operands[top[0].link];
  // Get and set the stmt
  	if(op->properties & EV_Immediate)
		*stmt = (sqlite3_stmt *) top[0].key;
	else if( operands[top[0].link].properties & EV_Operand)
		*stmt = operands[top[0].link].stmt; // operand table rules
	else
		return (0);
	op->maps[0]((Pointer *) &i,&j);
    j = op->maptype;
    if(j  == BIND_UPDATE)
      bind_update_var(*stmt,top);
    else if(j == BIND_SELF)
      bind_graph(*stmt,self_graph(),j & 0x0f,i);
    else if(j  == BIND_OTHER)
      bind_graph(*stmt,other_graph(),j & 0x0f,i);
    else if(j == BIND_TRIPLE)
      bind_triple(*stmt,top);
	else if(j == BIND_MAPPER) {
		Mapper * a = operands[top[0].link].maps;
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
					triple = &top[1];
					status = sqlite3_bind_text(*stmt,index++, 
						(char * ) triple->key, G_strlen(triple->key),0);
					status = sqlite3_bind_int(*stmt,index++,triple->link);
					status = sqlite3_bind_int(*stmt,index++,triple->pointer);
					break;
			}
		a++;
		}

	 // G_sprintf(buff,"Script: \n%s\n",sqlite3_sql(stmt));
	  //status = sqlite3_bind_text(stmt,1,"abc",3,0);
 
  }
  return SQLITE_OK;
}