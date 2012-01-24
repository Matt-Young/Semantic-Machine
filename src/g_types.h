#ifndef G_TYPEDEFS
#define G_TYPEDEFS
typedef struct {
  char  * key; 
  int link;
  int pointer;
} Triple;
typedef void * Pointer; 

extern Pointer g_db;
typedef int (*HANDLER)(Triple);
typedef int (*Mapper)(Pointer *pointer,int *type);
typedef void * Code;
#define G_TYPE_TRIPLE 6
#define G_TYPE_CODE 7
#define G_TYPE_MAPPER 8
#define G_TYPE_SYSTEM 9
#define G_TYPE_NULL 95
#define GBASE "c:/soft/gbase"

typedef struct { char * name; int type; Pointer value;} NameTypeValue;
int init_trios();
void print_trios();
int add_trios(NameTypeValue[]); 
int add_trio(char *,int,Pointer); 
Pointer  find_trio(char * name);
#endif