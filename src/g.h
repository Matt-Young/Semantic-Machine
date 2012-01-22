typedef struct {
  Mapper maps[4]; //map handlers
  Code stmt;
  HANDLER handler;
  int properties;
  } OP;

extern OP operands[];
#define TABLE_SQUARE 1
#define TABLE_NULL  2

typedef struct f { 
	int status;
  PGRAPH g[2];
  int properties;
  struct f *prev,*next;
} FILTER;

//limits and constants
#define OPERMAX 128

// Three reserved names for local use b all G machines.
#define GBASE "c:/soft/gbase"
#define GFUN "gfun"

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
#define G_USERMIN 24
#define G_SCRATCH  25
#define G_NULL  GCHAR
#define SCRATCH_STR "25"


#define G_DONE        102
#define G_READY        103
#define G_UNREADY        103
#define G_REPEAT        103
#define G_START        104
#define G_SELECTED        106
#define G_CONTINUE        107

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

extern const TRIPLE SCRATCH_TRIPLE;
extern const TRIPLE NULL_TRIPLE;


typedef struct {
    sqlite3 *db;
  int status;
  int output;
  } M;


int triple(TRIPLE top[],HANDLER);
int bind_sql(TRIPLE top[],Code *);
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

void set_table_name(char * name,int index);
TABLE * get_table_name(const char * name);
PGRAPH get_table_graph(int index);
int install_sql_script(char * ch,int opid);
void print_triple(TRIPLE t);

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

