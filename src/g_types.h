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
#define EV_Overload 0x80

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
int machine_value_int(Pointer * v);
char * machine_script(Pointer stmt);
#endif