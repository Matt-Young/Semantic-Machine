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
		SystemExec,SystemScript,SystemDecode,SystemConfig,SystemEcho};
#define SystemMax SystemEcho
#define SystemUser SystemMax + 4
#define OperatorMaximum 128
#define OperatorMask 0x7f
#define OperatorMSB 0x80
#define G_SCRATCH  25
#define SCRATCH_STR "25"
int get_system_call(char * name);
int install_sql_script(char * ch,int opid);
int triple(Triple top[],Handler);
int set_ready_event(int EV_event);