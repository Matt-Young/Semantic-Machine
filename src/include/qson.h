
/**
 * ... Basic  methods to move Qson...
 */

int qson_to_table(TABLE * table,char  * buff,int count);
int mem_to_net(int fd,int *buff,int protocol);
int * file_to_mem(FILE *fd); 
int mem_to_file( FILE * dest,int * buff,int mode);
int *table_to_mem(TABLE *t);
int mem_to_table(void * dest,int * buff,int mode);
int system_copy_qson(Webaddr *from,Webaddr *to );
int test_qson();