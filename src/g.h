
typedef struct {
  const char * key; 
  int link;
  int pointer;
} TRIPLE;

typedef int (*HANDLER)(TRIPLE);
extern TRIPLE G_null_graph;
typedef struct {
  int vp[4]; //maps
  int overload_operator;
  sqlite3_stmt *stmt;
  HANDLER handler;
  int properties;
  } OP;
typedef struct {
  int col_count;
  int rowid;
  const char * name[8]; 
  int type[8];
  int index;
} COLINFO;
#define NBUILTINS 3
#define pop_triple_operator 0
#define append_triple_operator 1
#define update_triple_operator 2
#define installed_triple_operator 3
typedef struct  {
  char * name;
  int attribute;
  TRIPLE operators[NBUILTINS + 2];
  int index;
  struct g * list;  //points to the innermost current graph
  COLINFO  info;
} TABLE;
typedef struct  g {
  int row;
  int start;
  int end;
  TABLE * table;
  struct g * parent;
  int match_state;
  TRIPLE pending_triple;
} GRAPH;
typedef GRAPH * PGRAPH; 
typedef struct f { 
	int status;
  PGRAPH g[2];
  int properties;
  struct f *prev,*next;
} FILTER;
typedef TABLE *PTABLE;
char * NAME(TABLE *);
int ATTRIBUTE(TABLE *);
TABLE * TABLE_POINTER(int i);
int DELETE_TABLE(TABLE *);
//limits and constants
#define OPERMAX 128

// Three reserved names for local use b all G machines.
#define GBASE "c:/soft/gbase"
#define GFUN "gfun"

// fundamental links
#define G_EXIT 0
#define G_SWAP 2
#define G_POP 3
#define G_POP_MAX 6
#define GCHAR '_'
#define G_CALL 8
#define G_DUP 9
#define G_EXEC 10
#define G_SQL 11
#define G_SCRIPT 15
#define G_CONFIG 16
#define G_INSERT 17
#define G_UPDATE 20
#define G_USERMIN 24
#define G_SCRATCH  25
#define G_NULL  GCHAR
#define SCRATCH_STR "25"


#define G_DONE        102
#define G_READY        103
#define G_UNREADY        103
#define G_REPEAT        103
#define G_START        104
#define G_SCHEMA        105
#define G_MATCHED        106
#define G_UNMATCHED         108      
#define G_SELECTED        109
#define G_UNSELECTED  108  
#define G_REJECTED        109
#define G_UNREJECTED  108 
#define G_COMPLETE  108 
#define G_STOPPED        109
#define G_CONTINUE        109
#define G_SQUARE        110
#define G_NESTED        111
#define G_ERR_INDEX 200
#define G_ERR_PREPARE 201
#define  G_ERR_DUP 202
#define  G_ERR_ENTRY 203
#define  G_ERR_HANDLER 204
#define  G_ERR_BIND 205
#define G_ERR_GRAPH 206
#define G_ERR_SCHEMA 207
#define G_ERR_FILTER 203
#define G_ERR_UNKNOWN 205
// IO is fixed by index and name
#define G_TABLE_SELF 2
#define G_TABLE_OTHER 3
#define G_TABLE_RESULT 4
//const TRIPLE EXIT_TRIPLE = {"Exit",G_EXIT,0};
//const TRIPLE SQL_TRIPLE = {"Sql",G_SQL,0};
//const TRIPLE SCRATCH_TRIPLE = {"Scratch",G_SCRATCH,0};
extern const TRIPLE SCRATCH_TRIPLE;
extern const TRIPLE NULL_TRIPLE;
typedef struct {
    sqlite3 *db;
  int status;
  int output;
  } M;
int init_tables();
int init_gfun();

int triple(TRIPLE top,HANDLER);
int gfun_callback(TRIPLE t);
void G_error(char * c,int i);
int bind_index(sqlite3_stmt *stmt,int i,int j);
void print_triple(TRIPLE t);
//int swap();
int loop();
//int jump(void *);
// mrethods to manage pointers for triple frames
int incr_row(int);
int set_row(int ivar);
int _row();
int stopped_row();
int key_op(char * key);
sqlite3_stmt * bind_sql(TRIPLE top);
void set_table_name(char * name,int index);
TABLE * get_table_name(const char * name);
int init_table(int,char *);
int init_gbase();
int install_sql_script(char * ch,int opid);
void print_triple(TRIPLE t);
int init_tables();
void * start_select();
int dispatch();
int parser();
int event_handler(TRIPLE t);
int event_exec(FILTER *f);
void G_exit();
void* G_malloc(int size);
void* G_calloc(int size);
void G_free(void* p);
char* G_strncpy(char* s, const char* ct, int n);
char * G_strcpy(char* s, const char* ct);
void G_sprintf(char* s, const char* format, ...);
void G_printf(const char* format, ...);
int G_strcmp(const char* cs, const char* ct);
int G_strlen(const char* cs);
int G_atoi(const char* s);
void G_memset(void* s, int c, int n);
int G_ispunct(int c);
char *  G_gets(char * line);

