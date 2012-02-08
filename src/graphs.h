typedef struct {int start;int row;int end;int rowoffset;} RowSequence;
typedef struct  g {
  RowSequence rdx;
  TABLE * table;
  Pstruct parent;
} GRAPH;
typedef GRAPH * PGRAPH; 
int set_row_sequence(RowSequence *);
PGRAPH new_child_graph(PGRAPH *);
PGRAPH new_graph_context();
PGRAPH delete_graph(PGRAPH *);
void close_update_graph(PGRAPH *);
int append_graph(PGRAPH *,Triple );
int release_graph_list(PGRAPH *);



int count_graph(PGRAPH );
void graph_counts() ;
PGRAPH dup_graph(PGRAPH l1,PGRAPH l2);
PGRAPH new_graph_by_index(int);
// layout of triplet installed in table contaxts




