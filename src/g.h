
typedef struct {
  const char * key; 
  int link;
  int pointer;
} TRIPLE;
typedef int (*HANDLER)(TRIPLE);
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
  char * name[8]; 
  int type[8];
  int index;
} COLINFO;
typedef struct  {
  char * name;
  int attribute;
  TRIPLE pop_triple,update_triple,insert_triple,select_triple;
  int index;
  void * list;
  COLINFO  info;
} TABLE;

char * NAME(TABLE *);
int ATTRIBUTE(TABLE *);
TABLE * TABLE_POINTER(int i);
int DELETE_TABLE(TABLE *);
//limits and constants
#define OPERMAX 128

// Three reserved names for local use b all G machines.
#define GBASE "gbase"
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
int init_gfun();


int triple(TRIPLE top,HANDLER);
int gfun_callback(TRIPLE t);
void gerror(char * c,int i);
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
int bind_sql(OP *op,TRIPLE top);
void set_table_name(char * name,int index);
int init_table(int);
int init_gbase();
int install_sql_script(char * ch,int opid);
void print_triple(TRIPLE t);
int init_gfun();
void * start_select();
int dispatch(void *f,TRIPLE s);
int parser();
void* G_malloc(size_t size);
void* G_calloc(size_t nobj, size_t size);
void G_free(void* p);
char* G_strncpy(char* s, const char* ct, size_t n);
char* G_strcpy(char* s, const char* ct);
int G_sprintf(char* s, const char* format, ...);
int G_printf(const char* format, ...);
