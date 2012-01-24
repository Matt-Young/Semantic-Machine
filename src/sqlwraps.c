  #include "../include/sqlite3.h"
#include "console.h"
 #include "g_types.h"
 typedef void (*xFunc)(sqlite3_context*,int,sqlite3_value**);
//void gfunction(sqlite3_context* context,int n,sqlite3_value** v);
int open_machine_layer(const char * name,Pointer * g_db) {
	return sqlite3_open(name,(sqlite3 **)&g_db);}
Pointer machine_column_text(Code stmt,int colid) {
	return (unsigned char *) sqlite3_column_text( (sqlite3_stmt*) stmt, colid);}
int machine_column_int(Code stmt,int colid) {
	return sqlite3_column_int((sqlite3_stmt*) stmt, colid);}
int machine_prepare(Pointer g_db,char * ch,Code * stmt) {
	return sqlite3_prepare_v2((sqlite3 *)g_db,ch,G_strlen(ch)+1,*stmt,0);}
int machine_step(Code stmt ) { return sqlite3_step((sqlite3_stmt*) stmt );}
  int machine_exec(Pointer g_db,char * buff,char ** err) {
	  return sqlite3_exec((sqlite3 *)g_db,buff,0,0,err);}
  int machine_install_callback(Pointer g_db,char * name,int nargs,Pointer gfunction) {
   return sqlite3_create_function_v2((sqlite3 *) g_db,
	   name,nargs,SQLITE_UTF8 ,0,
	   (xFunc) gfunction,0,0,0);
  }
	 void machine_result_int(Pointer context, int value) {
	 void sqlite3_result_int(sqlite3_context* context, int value);}

	 int machine_value_int(Pointer * v) {
		 return sqlite3_value_int((sqlite3_value*)v[0]);}
	 char * machine_script(Pointer stmt) {
		 return (char *) sqlite3_sql((sqlite3_stmt*) stmt); }
	 int machine_reset(Pointer stmt) {return sqlite3_reset((sqlite3_stmt*)stmt);}

