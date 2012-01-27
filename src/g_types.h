#ifndef G_TYPEDEFS
#define G_TYPEDEFS
typedef struct {
  char  * key; 
  int link;
  int pointer;
} Triple;
typedef void * Pointer; 
typedef int (*Handler)(Triple *);
typedef int (*Mapper)(Pointer *pointer,int *type);
typedef void * Code;


/*
#define SQLITE_INTEGER  1
#define SQLITE_FLOAT    2
#define SQLITE_BLOB     4
#define SQLITE_NULL     5
# define SQLITE_TEXT     3
*/
// git push git@github.com:Matt-Young/Embedded-SQL.git
enum {G_TYPE_NONE,G_TYPE_INTEGER,G_TYPE_FLOAT,
	G_TYPE_TEXT,G_TYPE_BIT,G_TYPE_TRIPLE,G_TYPE_CODE,
	G_TYPE_MAPPER,G_TYPE_HANDLER,G_TYPE_TABLE,
	G_TYPE_SYSTEM};

#define G_TYPE_NULL 95
#define GBASE "c:/soft/gbase"
#define GFUN "gfun"
#define SystemNameSpace "local"
#define LocalSymbolCount 100
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
int machine_bind_int(Code stmt,int index,int value);
int machine_bind_text(Code stmt,int index,char * ch);
void unbind_triple(Code stmt,Triple *t);
extern const Triple G_null_graph;
#define EV_Ok 0x00
#define EV_Null 0x01  // Do some null operation
#define EV_Data 0x02
#define EV_Done	0x04 
#define EV_Error 0x08 
#define EV_Graphs 0x70
#define EV_Comma 0x80  // Key value is matchable
#define EV_Matchable 0x100  // Key value is matchable
#define EV_Overload 0x200
#define EV_No_bind 0x400 // This event requires no binding
#define EV_Immediate 0x800  //The statement in the key value
#define EV_Matched 0x1000  // And matched
#define EV_Square 0x2000 // Square table looms ahead
#define EV_Incomplete 0x4000 // Square table looms ahead
#define EV_Debug 0x8000
#define EV_Init 0x10000
#endif