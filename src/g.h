
// Three reserved names for local use b all G machines.
#define GBASE "c:/soft/gbase"
#define GFUN "gfun"

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
extern const Triple SCRATCH_Triple;
extern const Triple NULL_Triple;



int bind_sql(Triple top[],Code *);
int gfun_callback(Triple t);
void G_error(char * c,int i);
int bind_index(sqlite3_stmt *stmt,int i,int j);
void print_triple(Triple t);
//int swap();
int loop();
//int jump(void *);
// mrethods to manage pointers for triple frames
int incr_row(int);
int set_row(int ivar);
int _row();
int stopped_row();



void print_triple(Triple t);


int event_handler(Triple t);
