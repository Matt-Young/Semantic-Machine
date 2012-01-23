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
typedef sqlite3_stmt * Code;
#define G_TYPE_Triple 6
#define G_TYPE_CODE 7
#define G_TYPE_NULL 95
#endif