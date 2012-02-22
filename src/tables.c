
#include "../src/include/config.h"
#include "../src/include/g_types.h"
#include "../src/include/names.h"
#include "./include/machine.h"
#include "../src/include/tables.h"
#include "../src/include/graphs.h"
#include "../src/include/engine.h"
#include "../src/include/console.h"



#define NUMBER_TABLES 20
// Keep a limited set o table open
TABLE *triple_tables[NUMBER_TABLES];

//extern int del_table_count,new_table_count;
TABLE  *new_table_context() {
  TABLE *pt;
  pt = (TABLE *) G_calloc(sizeof(TABLE));
  BC.new_table_count++;
return(pt);}

void free_table_context(TABLE *pt) {
   if(BC.del_table_count >= BC.new_table_count)
		G_printf("Bad table",EV_Error);
   delete_graph((PGRAPH *) pt->list);
	G_free((void *) pt);
	BC.del_table_count++;
}

TABLE * get_table_context(char * name) {
  TABLE *pt; int i;
  pt = new_table_context();
  pt->name = (char *) G_calloc(G_strlen(name));
  G_strcpy(pt->name,name);
  pt->list = 0;
  pt->attribute = G_TYPE_TABLE;

  for(i=0;i< NUMBER_TABLES;i++) {if(triple_tables[i]==0) break;}
  pt->index = i;
  triple_tables[i] = pt;
  return pt;
}
void release_table_context(TABLE *pt) {
   triple_tables[pt->index]=0;
    G_free((void *) pt->name);
	G_free((void *) pt);
	BC.del_table_count++;
}

// direct sql utilities
#define Sql_create \
  "drop table if exists %s; create table %s (link integer, pointer integer,key blob);" 
int del_create_table(char * table) {
  char buff[400];char  *err;
  G_sprintf(buff,Sql_create,table,table);
    return machine_exec(g_db,buff,&err);
}
#define Sql_copy \
  "insert into %s select * from  %s;" 
int dup_table(char * in,char * out) {
  char buff[400];char * err;
  del_create_table( out);
    G_sprintf(buff,Sql_copy,out,in);
    return machine_exec(g_db,buff,&err);
}
int make_stmt(TABLE * table,int format,char * table_name);
int init_table(char * name,int options,TABLE **table) {
	 int status = EV_Ok;
	 int i; TABLE *in;
   if(options)
		 del_create_table(name);
	 in =  get_table_context(name);
	 for(i=0; i < (spare_operator)/2;i++) {
		 make_stmt(in,i,name);
	 }
   new_child_graph((PGRAPH *) &in->list,(void *) '_');
   *table = in;
	 return status;
 }
int run_table(TABLE * t,Handler handler){
    PGRAPH  g;
  g = (PGRAPH ) t->list;
  g->rdx.rowoffset=1;
  g->rdx.end=-1;
  set_row_sequence(&g->rdx);
  G_printf("run table\n");
 machine_new_operator(&t->operators[pop_operator],handler);
 G_printf("\ndone\n");
 return 0;
}

 int init_run_table(char * name) {
	  TABLE * table; int status; Triple *t;
	  init_table(name,0,&table);
	  t  = &table->operators[pop_operator];
	  status = set_ready_graph(table);
	  machine_new_operator(t,ugly_handler);
	  return 0;
  }

Code   start_table(TABLE * t,int index){
  PGRAPH  g;
  g = (PGRAPH ) t->list;
  g->rdx.rowoffset=1;
  g->rdx.end=-1;
  set_row_sequence(&g->rdx);
 machine_set_operator(&t->operators[index],exit_handler);
 t->stmt=get_ready_stmt();
 return t->stmt;
}

#define Sql_delete_rows "delete from %s;"
int del_table_rows(TABLE *table) {
  char buff[400], *err; int status;
  G_sprintf(buff,Sql_delete_rows,table->name,table->name);
    status = machine_exec(g_db,buff,&err);
	return(status);
}

TABLE * TABLE_POINTER(int i) { return triple_tables[i];}
TABLE * get_table_name(const char * name) { 
	int i=0;
	while(triple_tables[i]) {
			if(!G_strcmp(triple_tables[i]->name,name))
				return (triple_tables[i]);
			i++;
		}
	return (0);
}

const struct new_install{
	int opindex;
	int opid;
	char * sql;
	char * map_name[4];
} installs[] = {
	{pop_operator,SystemMax+1,
	"select key,link,pointer from %s where (gfun(0,rowid) == rowid);",
  "UnbindTriple",0,0,0},
  {select_operator,SystemMax+2,
	"select key,link,pointer from %s;",
  "UnbindTriple",0,0,0},
	{append_text_operator,SystemMax+3,"insert into %s values( ?, ?, ?);",
	  "BindOldTriple","AppendHandler",0,0},
 {append_operator,SystemMax+4,"insert into %s values( ?, ?, ?);",
	  "BindTriple","AppendHandler",0,0},
	{update_operator,SystemMax+5,"update %s set pointer = (?) where rowid = ?;",
	"BindRelativeSelfRow","BindSelfStart","ExitHandler",0},
	{0,0,0,0,0,0,0}
};
Mapper null_map(void * p,int * i);
 int make_stmt(TABLE * table,int format,char * table_name) {
  char buff[200];
  int i,status=EV_Ok;
  Code stmt;Trio  * local_symbol;
  int opid = installs[format].opid;
  int index = installs[format].opindex;
  Triple *p = &table->operators[index]; 
  // make an sql script
  G_sprintf(buff,installs[format].sql, table_name);
  status= machine_prepare(g_db,buff, &stmt);
  if(status != EV_Ok) return status;
  p->key = (char *) stmt; 
  p->link = opid;  p->pointer = 0;
  operands[opid].stmt = stmt;  // Look in the table context for stmt
  operands[opid].handler = 0;  // May be over ridden
  operands[opid].properties = EV_Overload;
  for(i=0; installs[format].map_name[i];i++) {
	  local_symbol = find_name(installs[format].map_name[i]);
  if(local_symbol->type == G_TYPE_MAPPER)
  	  operands[opid].maps[i]= (Mapper) local_symbol->value;
  else if(local_symbol->type == G_TYPE_BIT)
    operands[opid].properties |= (int) local_symbol->value;
    else if(local_symbol->type == G_TYPE_HANDLER)
    operands[opid].handler = (Handler) local_symbol->value;
  }
  return status;
}


void gfunction(Pointer context,int n,Pointer * v);

 Trio table_trios[] = {{"TablesInit",0,0},{0,0,0}};
int init_tables() {
	int status;
	add_trios(table_trios);
	G_memset(triple_tables,0,sizeof(triple_tables));
	BC.del_table_count=0,BC.new_table_count=0;
	status = machine_install_callback(g_db,GFUN,2,gfunction);
	return status;
}