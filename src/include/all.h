
/**
 * ... all.h just includes the whole bunch...
 */

/*! \defgroup  Global Group
 * A global group.
 */
/*! \ingroup  Global Group
 * A global group.
 */
#include "../src/include/config.h"

#include "../src/include/g_types.h"
#include "../src/include/names.h"
#include "../src/include/tables.h"
#include "../src/include/graphs.h"
#include "../src/include/engine.h"
#include "../src/include/console.h"
#include "../src/include/g.h"
/*! \ingroup  Global
 * Binds variables to position for a prepared statement.
 */
  int init_binder();
  /*! \ingroup  Global
 * Default handlers for incoming table data.
 */
  int init_handlers();
 /*! \ingroup  Global
 * Tables are a from/to point for Qsons.
 */
  int init_tables();
 /*! \ingroup  Global
 * Gfun, the overload manager.
 */
  int init_gfun();
 /*! \ingroup  Global
 * Console picks up from the keyboard.
 */
  int init_console();
 /*! \ingroup  Global
 * Json to Qson table.
 */
  int parser(char *buff,TABLE *table);
 /*! \ingroup  Global
 * network listener.
 */
  int net_start(void *);