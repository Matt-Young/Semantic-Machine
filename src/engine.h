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
enum { SystemExit,SystemCall,SystemDup,SystemPop,
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
Code set_ready_code(Code stmt,int opid);
int get_system_call(char * name);
int triple(Triple top[],Handler);
int set_ready_event(int EV_event);
Code get_ready_stmt();
int reset_ready_event(int EV_event);
int bind_code(Triple *,Code );

  // Generics that cover sqlite3
extern Pointer g_db;
int open_machine_layer(const char * name,Pointer  mach);
Pointer machine_column_text(Code stmt,int colid); 
int machine_column_int(Code stmt,int colid);
int machine_prepare(Pointer g_db,char * ch,Code * stmt);
int machine_step(Code stmt );
int machine_reset(Code stmt);
int machine_exec(Pointer g_db,char * buff,char ** err);
int machine_install_callback(Pointer g_db,char * name,int nargs,Pointer gfunction);
void machine_result_int(Pointer context, int value);
int machine_value_int(Pointer  v);
char * machine_script(Pointer stmt);
int machine_triple(Code stmt,Triple * t);
int machine_key_len(Code stmt); 
int machine_bind_int(Code stmt,int index,int value);
int machine_bind_text(Code stmt,int index,char * ch);
