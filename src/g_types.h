#ifndef G_TYPEDEFS
#define G_TYPEDEFS
typedef struct {
  char  * key; 
  int link;
  int pointer;
} Triple;
typedef void * Pointer; 

extern Pointer g_db;
typedef int (*Handler)(Triple);
typedef int (*Mapper)(Pointer *pointer,int *type);
typedef void * Code;
#define G_TYPE_INTEGER  1
#define G_TYPE_FLOAT    2
#define G_TYPE_BLOB     4

#define G_TYPE_TRIPLE 6
#define G_TYPE_CODE 7
#define G_TYPE_MAPPER 8
#define G_TYPE_HANDLER 9
#define G_TYPE_SYSTEM 9
#define G_TYPE_NULL 95




#define GBASE "c:/soft/gbase"

// Generics that cover sqlite3
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