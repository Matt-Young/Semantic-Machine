#include "../sqlite/sqlite3.h"
#include "../src/include/g_types.h"

#include "./include/machine.h"
#include "../src/include/tables.h"
#include "../src/include/graphs.h"
#include "../src/include/engine.h"
#include "../src/include/console.h"

#define clear_events  reset_ready_event( EV_Done| EV_Data | EV_Error)
int msg_id(int sqlite_msg) {
	if(sqlite_msg == SQLITE_OK) return EV_Ok;
	else if(sqlite_msg == SQLITE_ERROR) return EV_Error;
	else if(sqlite_msg == SQLITE_MISUSE) return EV_Error;
	else if(sqlite_msg == SQLITE_DONE) return EV_Done;
	else if(sqlite_msg == SQLITE_ROW) return EV_Data;
	else return EV_Error;
}

void unbind_triple(Code stmt,Triple *t);
typedef void (*xFunc)(sqlite3_context*,int,sqlite3_value**);
//void gfunction(sqlite3_context* context,int n,sqlite3_value** v);
extern char  DirOrigin[];
int open_machine_layer(char * name,Pointer  g_db) {
 char  complete_name[400];
   G_strcpy(complete_name,DirOrigin);
   G_strcat(complete_name,name);
	return msg_id(sqlite3_open(complete_name,(sqlite3 **)g_db));}

Pointer machine_column_text(Code stmt,int colid) {
	return (unsigned char *) sqlite3_column_text( (sqlite3_stmt*) stmt, colid);}

int machine_column_int(Code stmt,int colid) {
	return sqlite3_column_int((sqlite3_stmt*) stmt, colid);}

int machine_prepare(Pointer g_db,char * ch,Code * stmt) {
	return msg_id(sqlite3_prepare_v2(
		(sqlite3 *)g_db,ch,G_strlen(ch)+1,(sqlite3_stmt **) stmt,0));}


int machine_step(Code stmt ) { 
  clear_events;
	return set_ready_event(msg_id(sqlite3_step((sqlite3_stmt*) stmt )));}
int machine_reset(Pointer stmt) {
  reset_ready_event( EV_Done| EV_Data | EV_Error);
	return msg_id(sqlite3_reset((sqlite3_stmt*)stmt));}

int machine_exec(Pointer g_db,char * buff,char ** err) {
	return msg_id(sqlite3_exec((sqlite3 *)g_db,buff,0,0,err));}

int  machine_lock() {
  char * err;
  return msg_id(sqlite3_exec((sqlite3 *)g_db,"BEGIN IMMEDIATE;",0,0,&err));}
int  machine_unlock() {
    char * err;
  return msg_id(sqlite3_exec((sqlite3 *)g_db,"COMMIT TRANSACTION;",0,0,&err));}
int machine_install_callback(Pointer g_db,char * name,int nargs,Pointer gfunction) {
	return msg_id(sqlite3_create_function_v2((sqlite3 *) g_db,
		name,nargs,SQLITE_UTF8 ,0,
		(xFunc) gfunction,0,0,0));
}

void machine_result_int(Pointer context, int value) {
	sqlite3_result_int( (sqlite3_context*) context, value);}

int machine_key_len(Code stmt) {return sqlite3_column_bytes((sqlite3_stmt*) stmt,0 );}
int machine_value_len(Code stmt,int i) {return sqlite3_column_bytes((sqlite3_stmt*) stmt,i );}
int machine_value_int(Pointer v) {
	return sqlite3_value_int((sqlite3_value*)v);}
char * machine_script(Pointer stmt) {
	return (char *) sqlite3_sql((sqlite3_stmt*) stmt); }

int machine_bind_int(Code stmt,int index,int value) {
	return msg_id(sqlite3_bind_int((sqlite3_stmt*) stmt,index++,value));}

int machine_bind_text(Code stmt,int index,char * ch) {
	return msg_id(sqlite3_bind_text((sqlite3_stmt*)stmt,index,
		ch,G_strlen( ch),0));}
int machine_bind_blob(Code stmt,int index,void * blob,int size){
return msg_id( sqlite3_bind_blob((sqlite3_stmt*)stmt, index ,blob, size, 0));
}
void unbind_triple(Code stmt,Triple *t) {
  t->key =(char *) sqlite3_column_text( (sqlite3_stmt*) stmt, 0);
	t->link= machine_column_int(stmt, 1);
	t->pointer= machine_column_int(stmt, 2);
}

int machine_triple(Code stmt,Triple * t) {
	unbind_triple(stmt,t);
	return(EV_Ok);
}
int machine_step_fetch(Triple *t,Handler h) {
  Code stmt = get_ready_stmt();
     machine_step(stmt);
        machine_triple(stmt,t);
      if(h) h(t);
    return set_ready_event(0);
}
int machine_append(Triple *t,Handler h) {
  Code stmt = get_ready_stmt();
  bind_code(t,stmt);
  machine_step(stmt );
  if(h) h(t);
  return set_ready_event(0);
}
int machine_loop(Triple *t,Handler h) {
  int events;
  Code stmt = get_ready_stmt();
  if(stmt)
		do {
      events=machine_step(stmt);
      if(events & EV_Error)
			  G_printf("err: loop ");
      else if((events & EV_Data) && h)
        h(t);
		}  while( !(events & EV_Done) );
	machine_reset(stmt);
  return 0;
}
void machine_row_info(Code  stmt, ColInfo *cinfo) {
  int i;
  cinfo->count = sqlite3_column_count((sqlite3_stmt*)stmt);
  for(i=0;i< cinfo->count;i++) {
    cinfo->name[i] = sqlite3_column_name((sqlite3_stmt*) stmt, i);
    cinfo->type[i] = sqlite3_column_type((sqlite3_stmt*) stmt, i);
    if(i == 3)
      cinfo->rowid = sqlite3_column_int((sqlite3_stmt*)stmt, i);
  }
}  
void machine_unbind_row(Code stmt,ColInfo * cinfo, void * vals[]) {
  int i;
  for (i=0;i < cinfo->count;i++) {
    switch (cinfo->type[i]) {
    case G_TYPE_INTEGER:
	vals[i]= (void *) machine_column_int(stmt, i);
      break;
    case G_TYPE_TEXT:
    case G_TYPE_BLOB :
       vals[i]= (void *)  sqlite3_column_text( (sqlite3_stmt*) stmt, i);
      break;
    default:
        G_printf("Unknown%d\n",cinfo->type[i]);
    }
  }
}
