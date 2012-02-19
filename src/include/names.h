/**
 * ... Names use the qsort and bsearch for 
       local smbols whne processing tables...
 */

typedef struct {  char * name; int type; void * value;} Trio;
int init_trios();
void print_trios();
int add_trios( Trio[]); 
Trio * add_trio( char * name,int type,void * value);
void *  find_trio_value(char * name);

Trio * find_or_add_trio(char * name);
char * new_string(const char * );
void del_string(const char *);
Trio * find_name(char * key);
Trio * get_name(char * key);
void sort_names() ;
int flush_users();
extern int g_name_count;