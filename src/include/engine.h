// <h4> Engine H </h4> <!-- turn off html
typedef struct {
  Mapper maps[4]; //map handlers
  Code stmt;
  Handler handler;
  int properties;
  } OP;
//limits and constants
extern OP operands[];
#define GCHAR '_'
// Operator space on the predicate
enum SysCalls { SystemExit,SystemCall,SystemDup,SystemPop,
		SystemExec,SystemScript,SystemDecode,SystemConfig,SystemName,SystemEcho};
#define SystemMax SystemEcho
#define SystemUser SystemMax + 6
#define SystemScratch SystemUser +1
#define SystemNull '_'
#define OperatorMaximum 128
#define OperatorMask 0x7f
#define OperatorMSB 0x80
// Overloads in the first byte
enum { OperatorJson = OperatorMSB,OperatorBsonIn,OperatorBsonOut};
Code set_ready_stmt(Code stmt);
int set_ready_code(int opid);
int get_system_call(char * name);
 //--><h4> Ready Events  </h4><ul><li>
int set_ready_event(int EV_event);//</li> <li>
Code get_ready_stmt();//</li> <li>
int reset_ready_event(int EV_event);//</li> <li>
int bind_code(Triple *,Code );//</li> <li>
void  * get_web_addr();//</li> <li>
void * set_web_addr(void  *,int);//</li> 
//</ul> <!-- 
int exit_handler(Triple *node);

  // Generics that cover sqlite3
extern Pointer g_db;
/**
 * ... Stepping Machine API ...
 */
 
int open_machine_layer(char * name,Pointer  mach); 
int  machine_set_operator(Triple top[],Handler handler);
int machine_loop(Triple *t,Handler h) ;
int machine_new_operator(Triple top[],Handler);
int machine_append(Triple *t,Handler h);
int machine_append_blob(Triple *t,Handler h);
Pointer machine_column_text(Code stmt,int colid); 
int machine_step_fetch(Triple *t,Handler h);
int machine_column_int(Code stmt,int colid);
int machine_prepare(Pointer g_db,char * ch,Code * stmt);
int machine_step(Code stmt);
int machine_reset(Code stmt);
int machine_exec(Pointer g_db,char * buff,char ** err);
int machine_install_callback(Pointer g_db,char * name,int nargs,Pointer gfunction);
void machine_result_int(Pointer context, int value);
int machine_value_int(Pointer  v);
int machine_lock();
int machine_unlock();
char * machine_script(Pointer stmt);
int machine_triple(Code stmt,Triple * t);
int machine_key_len(Code stmt); 
int machine_bind_int(Code stmt,int index,int value);
int machine_bind_text(Code stmt,int index,char * ch);
int machine_bind_blob(Code stmt,int index,void * blob,int size);
Handler get_ghandler(Triple top[],Handler handler);
int pop_handler(Triple *node);