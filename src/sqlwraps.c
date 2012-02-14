#include "../sqlite/sqlite3.h"
#include "g_types.h"
#include "engine.h"
#include "console.h"


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
int open_machine_layer(const char * name,Pointer  g_db) {
	return msg_id(sqlite3_open(name,(sqlite3 **)g_db));}

Pointer machine_column_text(Code stmt,int colid) {
	return (unsigned char *) sqlite3_column_text( (sqlite3_stmt*) stmt, colid);}

int machine_column_int(Code stmt,int colid) {
	return sqlite3_column_int((sqlite3_stmt*) stmt, colid);}

int machine_prepare(Pointer g_db,char * ch,Code * stmt) {
	return msg_id(sqlite3_prepare_v2(
		(sqlite3 *)g_db,ch,G_strlen(ch)+1,(sqlite3_stmt **) stmt,0));}


int machine_step(Code stmt ) { 
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

