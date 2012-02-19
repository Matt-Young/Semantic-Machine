/**
 * ... Table context ad defines...
 */

#define NBUILTINS 10
typedef struct  {
  int col_count;
  int rowid;
  const char * name[8]; 
  int type[8];
  int index;
} COLINFO;
typedef struct tables  {
  char * name;
  int attribute;
  Triple operators[NBUILTINS];  /*!< Prepared statements fo the table */
  int index;
  Pointer list;  //points to the innermost current graph
  COLINFO  info;
} TABLE;
#define TABLE_SQUARE 1
#define TABLE_NULL  2
enum PreparedTableOps {
	pop_operator,pop_data,
	append_operator,append_data,
  append_old_operator,append_old_data,
	update_operator,update_data,
	spare_operator,spare_data};


char * NAME(TABLE *);
int ATTRIBUTE(TABLE *);
TABLE * TABLE_POINTER(int i);
int DELETE_TABLE(TABLE *);
int del_create_table(TABLE *);
int init_table(char * name,int options,TABLE **table);
int run_table(TABLE * t,Handler handler);
Triple * start_table(TABLE * t,int index);
TABLE * get_table_context(char *);
TABLE * get_table_name(const char * name);
void release_table_context(TABLE *pt);
Pointer new_table_graph(TABLE *table);
