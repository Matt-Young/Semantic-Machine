
/**
 * ... all.h just includes the whole bunch...
 */

/*! \defgroup  Global Group
 * A global group.
 */
/*! \ingroup  Global Group
 * A global group.
 */

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
  int echo_handler(Triple *);
  int event_handler(Triple * t);
  void print_triple(Triple *t);
  int incr_row(int delta);