// All includes
#include "g_types.h"
#include "names.h"
#include "tables.h"
#include "graphs.h"
#include "engine.h"
#include "console.h"
#include "g.h"
// init list
int pop_handler(Triple *node);
  int init_binder();
  int init_handlers();
  int init_tables();
  int init_gfun();
  int init_console();
  int parser(char *,PGRAPH *);
  PGRAPH * init_parser();
  int process_block(char * Json, PGRAPH *inner);
  int init_filters();