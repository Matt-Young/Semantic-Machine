  /*! \defgroup  Graph
 * Manage nested groups in tables
 */
typedef struct {int row;int end;int rowoffset;} RowSequence;
/*! \ingroup  Graph
 * Graph context.

 */
typedef struct  Graph {
  RowSequence rdx;
  TABLE * table;
  struct Graph * parent;
  void * context;
} GRAPH;
typedef GRAPH * PGRAPH; 

/*! \ingroup  Graph
 * Set the srow and end pointers.
 */
int set_row_sequence(RowSequence *);
/*! \ingroup  Graph
 * New sub graph within the curren graph context.
 */
PGRAPH new_child_graph(PGRAPH *,void * context);
PGRAPH new_graph_context();
PGRAPH delete_graph(PGRAPH *);
/*! \ingroup  Graph
 * Close and delete graph, write the pointer value back into the table..
 */
void close_update_graph(PGRAPH *);
/*! \ingroup  Graph
 * Write the actualy key bytes,link and pointer to the table.
 */
int append_graph(PGRAPH *,Triple );
int release_graph_list(void *);
void * graph_variable(PGRAPH  inner);

int count_graph(PGRAPH );
void graph_counts() ;
PGRAPH dup_graph(PGRAPH l1,PGRAPH l2);
PGRAPH new_graph_by_index(int);




