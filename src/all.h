// All includes
#include "../src/config.h"
#include "../src/g_types.h"
#include "../src/names.h"
#include "../src/tables.h"
#include "../src/graphs.h"
#include "../src/engine.h"
#include "../src/console.h"
#include "../src/g.h"

int pop_handler(Triple *node);
  int init_binder();
  int init_handlers();
  int init_tables();
  int init_gfun();
  int init_console();
  int parser(char *,TABLE *);
  TABLE * init_parser();
  int process_block(char * Json, PGRAPH *inner);
  int init_filters();