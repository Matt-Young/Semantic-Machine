typedef struct {
  Mapper maps[4]; //map handlers
  Code stmt;
  Handler handler;
  int properties;
  } OP;
//limits and constants
#define OPERMAX 128
extern OP operands[];
// fundamental links
#define G_EXIT 0
#define G_SWAP 2
#define G_POP 3
#define G_APPEND 4
#define G_UPDATE 5
#define G_GRAPH_MAX 6
#define GCHAR '_'
#define G_CALL 8
#define G_DUP 9
#define G_EXEC 10
#define G_SQL 11
#define G_SCRIPT 15
#define G_CONFIG 16
#define G_DEBUG 17
#define G_USERMIN 24
#define G_SCRATCH  25
#define SCRATCH_STR "25"
int get_system_call(char * name);
int install_sql_script(char * ch,int opid);
int triple(Triple top[],int (*handler)(Triple));