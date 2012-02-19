
/**
 * ... all.h just includes the whole bunch...
 */


#include "../src/include/config.h"
#include "../src/include/g_types.h"
#include "../src/include/names.h"
#include "../src/include/tables.h"
#include "../src/include/graphs.h"
#include "../src/include/engine.h"
#include "../src/include/console.h"
#include "../src/include/g.h"
/**
 * ... Initialization for modules...
 */


  int init_binder();
  int init_handlers();
  int init_tables();
  int init_gfun();
  int init_console();
  int parser(char *,TABLE *);
  int net_start(void *);