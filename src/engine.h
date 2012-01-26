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
#define SystemUser SystemMax + 6
#define SystemScratch SystemUser +1
#define SystemNull '_'
#define OperatorMaximum 128
#define OperatorMask 0x7f
#define OperatorMSB 0x80
Code set_ready_code(Code stmt);
int get_system_call(char * name);
int triple(Triple top[],Handler);
int set_ready_event(int EV_event);