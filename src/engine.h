typedef struct {
  Mapper maps[4]; //map handlers
  Code stmt;
  Handler handler;
  int properties;
  } OP;
//limits and constants
#define OPERMAX 128
extern OP operands[];
#define GCHAR '_'
// fundamental operators
enum { SystemExit,SystemCall,SystemDup,SystemPop,
		SystemExec,SystemScript,SystemDecode,SystemConfig,SystemEcho};
#define SystemMax SystemEcho
#define G_USERMIN 24
#define G_SCRATCH  25
#define SCRATCH_STR "25"
int get_system_call(char * name);
int install_sql_script(char * ch,int opid);
int triple(Triple top[],Handler);
int set_ready_event(int EV_event);