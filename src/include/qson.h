  /*! \defgroup  Qson
 * Moving Qson
 */
/*! \ingroup  Qson
 * Move from Qson flat tdirectly to machine table
 */

int qson_to_table(TABLE * table,char  * buff,int count);
/*! \ingroup  Qson
 * Mem directly with send, key by key
 */
int mem_to_net(int fd,int *buff,int protocol);
/*! \ingroup  Qson
 * The file format is flat, like the net format
 */
int *file_to_mem(void *fd); 
int mem_to_file( void * dest,int * buff,int mode);
/*! \ingroup  Qson
 * FIle counts are ssprintf things
 */
int *table_to_mem(TABLE *t);
/*! \ingroup  Qson
 * Vey fast with installed operator
 */
int mem_to_table(void * dest,int * buff,int mode);
/*! \ingroup  Qson
 * General call for a Qson move
 */
int system_copy_qson(Webaddr *from,Webaddr *to );
int test_qson(); 

struct Format { 
  struct {char link; char rows[3];char key_count[4];long key_data;} Flat;
  struct  {
      struct { long link; long rows; long *key_data;} fixed;
      struct {long key_count; long key_data;} variable;} Memory;
};