

int qson_to_table(TABLE * table,char  * buff,int count);
int mem_to_net(int fd,int *buff,int protocol);
int *file_to_mem(void *fd);  /*!< Returns a pointer to a Qson  */
int mem_to_file( void * dest,int * buff,int mode);
int *table_to_mem(TABLE *t);/*!< Returns the memory Qson pointer */
int mem_to_table(void * dest,int * buff,int mode);/*!< Delete and open table by name */
int system_copy_qson(Webaddr *from,Webaddr *to );
int test_qson();