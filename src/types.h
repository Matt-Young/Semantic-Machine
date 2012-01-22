#ifndef G_TYPEDEFS
typedef struct {
  char  * key; 
  int link;
  int pointer;
} TRIPLE;
typedef void * Pointer; 
typedef int (*HANDLER)(TRIPLE);
#define G_TYEDEFSS
typedef sqlite3_stmt * Code;
#define G_TRIPLE 6
#endif