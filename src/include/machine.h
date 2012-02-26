
 /*! \defgroup  Machine
 * Machine API
 */
/*! \ingroup  Machine
 * Report the types and names of a square row.
 */
typedef struct  ColInfo {
  int count;
  int rowid;
  const char * name[8]; 
  int type[8];
  int index;
}ColInfo;
/*! \ingroup  Machine
 * Open he data base.
 */
int open_machine_layer(char * name,Pointer  mach); 
/*! \ingroup  Machine
 * Set up but don't run a new statement.
 */
int  machine_set_operator(Triple top[],Handler handler);
/*! \ingroup  Machine
 * Loop until EV_Done.
 */
int machine_loop(Triple *t,Handler h) ;
/*! \ingroup  Machine
 * Set up and run a new statement.
 */
int machine_new_operator(Triple top[],Handler);
/*! \ingroup  Machine
 * inser the current triple at the end.
 */
int machine_append(Triple *t,Handler h);
/*! \ingroup  Machine
 * Blob format of same.
 */
int machine_append_blob(Triple *t,Handler h);
/*! \ingroup  Machine
 * Grap ponter to key bytes.
 */
Pointer machine_column_text(Code stmt,int colid); 
/*! \ingroup  Machine
 * Step machine once and return triple.
 */
int machine_step_fetch(Triple *t,Handler h);
/*! \ingroup  Machine
 * Grav the colunm long.
 */
int machine_column_int(Code stmt,int colid);
/*! \ingroup  Machine
 * Compile script to statement.
 */
int machine_prepare(Pointer g_db,char * ch,Code * stmt);
/*! \ingroup  Machine
 * Step once.
 */
int machine_step(Code stmt);
/*! \ingroup  Machine
 * Reset statement.
 */
int machine_reset(Code stmt);
/*! \ingroup  Machine
 * Excute uncompiled script.
 */
int machine_exec(Pointer g_db,char * buff,char ** err);
/*! \ingroup  Machine
 * Install call back, mainly on row comparisons.
 */
int machine_install_callback(Pointer g_db,char * name,int nargs,Pointer gfunction);
/*! \ingroup  Machine
 * Get the int on a call back.
 */
void machine_result_int(Pointer context, int value);
/*! \ingroup  Machine
 * Rturn an int on the call back.
 */
int machine_value_int(Pointer  v);
/*! \ingroup  Machine
 * Run the Begin statement.
 */
int machine_lock();
int machine_unlock();
char * machine_script(Pointer stmt);
int machine_triple(Code stmt,Triple * t);
/*! \ingroup  Machine
 * Keys hold the variable length value bytes.
 */
int machine_key_len(Code stmt); 
int machine_bind_int(Code stmt,int index,int value);
/*! \ingroup  Machine
 * Version bind text.
 */
int machine_bind_text(Code stmt,int index,char * ch);
/*! \ingroup  Machine
 *And bind blob
 */
int machine_bind_blob(Code stmt,int index,void * blob,int size);
/*! \ingroup  Machine
 * Get the column types and names for a square.
 */
void machine_row_info(Code  stmt, ColInfo *cinfo);
/*! \ingroup  Machine
 * Grab a row of longs from a square table.
 */
void machine_unbind_row(Code stmt,ColInfo * cinfo, void * vals[]);
/*! \ingroup  Machine
 * Return current col value as text.
 */
char * machine_text(Code stmt,int colid);
int machine_value_len(Code stmt,int i);