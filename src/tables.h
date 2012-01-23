#define NBUILTINS 3
typedef struct {
  int col_count;
  int rowid;
  const char * name[8]; 
  int type[8];
  int index;
} COLINFO;
typedef struct  {
  char * name;
  int attribute;
  Triple operators[NBUILTINS + 5];  // operand sequences for triple machine
  int index;
  struct g * list;  //points to the innermost current graph
  COLINFO  info;
} TABLE;
#define TABLE_SQUARE 1
#define TABLE_NULL  2


typedef TABLE *PTABLE;
char * NAME(TABLE *);
int ATTRIBUTE(TABLE *);
TABLE * TABLE_POINTER(int i);
int DELETE_TABLE(TABLE *);
int init_table(int index,char * name);

void set_table_name(char * name,int index);
TABLE * get_table_name(const char * name);
