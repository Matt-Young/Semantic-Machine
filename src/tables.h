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
  TRIPLE operators[NBUILTINS + 5];  // operand sequences for triple machine
  int index;
  struct g * list;  //points to the innermost current graph
  COLINFO  info;
} TABLE;
typedef TABLE *PTABLE;
char * NAME(TABLE *);
int ATTRIBUTE(TABLE *);
TABLE * TABLE_POINTER(int i);
int DELETE_TABLE(TABLE *);