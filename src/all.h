#include "console.h"
#include "g_types.h"
#include "binds.h"
#include "tables.h"
#include "graphs.h"
#include "engine.h"

typedef struct f { 
  int status;
  PGRAPH g[2];
  Triple * event_triple;
  int properties;
  struct f *prev,*next;
} FILTER;
int event_exec(FILTER *f);

#include "g.h"
// init list
  int init_binders();
  int init_handlers();
  int init_tables();
  int init_gfun();
  int init_console();
  int parser();