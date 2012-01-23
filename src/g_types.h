#ifndef G_TYPEDEFS
typedef struct {
  char  * key; 
  int link;
  int pointer;
} Triple;
typedef void * Pointer; 
typedef int (*HANDLER)(Triple);
#define G_TYPEDEFS
typedef sqlite3_stmt * Code;
#define G_TYPE_Triple 6
#define G_TYPE_CODE 7
#define G_TYPE_NULL 95
#endif