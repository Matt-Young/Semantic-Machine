//Names
typedef struct {  char * name; int type; Pointer value;} Trio;
int init_trios();
void print_trios();
int add_trios( Trio[]); 
int add_trio( char * name,int type,Pointer value);
Pointer  find_trio_value(char * name);
Trio *  find_trio(char * name);