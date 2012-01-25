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
enum { G_EXIT,G_NONE,G_SWAP,G_POP,
	   G_CALL,G_DUP,G_EXEC,G_SQL,
	   G_SCRIPT,G_CONFIG,G_DEBUG,
	   G_APPEND,G_UPDATE,G_SYS_MAX};

#define G_USERMIN 24
#define G_SCRATCH  25
#define SCRATCH_STR "25"
int get_system_call(char * name);
int install_sql_script(char * ch,int opid);
int triple(Triple top[],Handler);
int set_ready_event(int EV_event);