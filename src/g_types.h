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
typedef void * Code;
#define G_TYPE_Triple 6
#define G_TYPE_CODE 7
#define G_TYPE_NULL 95
#define GBASE "c:/soft/gbase"

typedef struct { char * name; Pointer value;} NameValue;
int init_name_value();
void print_name_value();
int add_name_value(NameValue pairs[]); 
Pointer  find_name_value(char * name);
#endif