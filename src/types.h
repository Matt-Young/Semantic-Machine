#ifndef G_TYPEDEFS
typedef struct {
  char  * key; 
  int link;
  int pointer;
} TRIPLE;
typedef void * Pointer; 
typedef int (*HANDLER)(TRIPLE);
#define G_TYPEDEFS
typedef sqlite3_stmt * Code;
#define G_TRIPLE 6
#define G_NULL 95
#endif