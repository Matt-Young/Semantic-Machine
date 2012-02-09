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
  Pointer list;  //points to the innermost current graph
  COLINFO  info;
} TABLE;
#define TABLE_SQUARE 1
#define TABLE_NULL  2
enum {
	pop_triple_operator,pop_triple_data,
	append_triple_operator,append_triple_data,
	update_triple_operator,update_triple_data,
	fetch_triple_operator,
  installed_triple_operator};

typedef TABLE *PTABLE;
char * NAME(TABLE *);
int ATTRIBUTE(TABLE *);
TABLE * TABLE_POINTER(int i);
int DELETE_TABLE(TABLE *);
int del_create_table(TABLE *);
int init_table(char * name,int options,TABLE **table);
PTABLE get_table_context(char *);
PTABLE get_table_name(const char * name);
void release_table_context(PTABLE pt);
Pointer new_table_graph(PTABLE table);
