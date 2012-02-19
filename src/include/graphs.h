// <h4> Graphs H </h4> <!--
typedef struct {int row;int end;int rowoffset;} RowSequence;
typedef struct  g {
  RowSequence rdx;
  TABLE * table;
  Pstruct parent;
  void * context;
} GRAPH;
typedef GRAPH * PGRAPH; 
 //--> <ul> <li>
int set_row_sequence(RowSequence *);//</li> <li>

PGRAPH new_child_graph(PGRAPH *,void * context);//</li> <li>
PGRAPH new_graph_context();//</li> <li>
PGRAPH delete_graph(PGRAPH *);//</li> <li>
void close_update_graph(PGRAPH *);//</li> <li>
int append_graph(PGRAPH *,Triple );//</li> <li>
int release_graph_list(PGRAPH *);//</li> <li>
void * graph_variable(PGRAPH  inner);//</li> <li>


int count_graph(PGRAPH );//</li> <li>
void graph_counts() ;//</li> <li>
PGRAPH dup_graph(PGRAPH l1,PGRAPH l2);//</li> <li>
PGRAPH new_graph_by_index(int);//</li>  </ul> <!--
// layout of triplet installed in table contaxts
//-->



