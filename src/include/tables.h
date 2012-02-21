

#define TABLE_SQUARE 1
#define TABLE_NULL  2
enum PreparedTableOps {
  select_operator,select_data,
	pop_operator,pop_data,
	append_operator,append_data,
  append_text_operator,append_text_data,
	update_operator,update_data,
	spare_operator,spare_data};

  typedef struct Tables  {
  char * name;
  int attribute;
  Triple operators[spare_data+1];  /*!< Prepared statements for the table*/
  int index;
  Code stmt;
  struct  Graph * list;  //points to the innermost current graph
  ColInfo info;
} TABLE;


  /*! \defgroup  Table
 * Table context for managing sql table
 */
/*! \ingroup  Table
 * Delete and recreate a Qson table.
 */
int del_create_table(char *);
int dup_table(char * in,char * out);
/*! \ingroup  Table
 * Initialize and potentially delete a table by name.
 */
int init_table(char * name,int options,TABLE **table);
/*! \ingroup  Table
 * Just feed triple into the pipe.
 */
int run_table(TABLE * t,Handler handler);
/*! \ingroup  Table
 * Mainly set row sequencers.
 */

Code start_table(TABLE * t,int index);
/*! \ingroup  Table
 * Set up and run this table.
 */

int init_run_table(char * name);
TABLE * get_table_context(char *);
TABLE * get_table_name(const char * name);
void release_table_context(TABLE *pt);
Pointer new_table_graph(TABLE *table);
