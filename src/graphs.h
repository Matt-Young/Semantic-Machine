
typedef struct  g {
  int row;
  int start;
  int end;
  TABLE * table;
  struct g * parent;
  int match_state;
  TRIPLE pending_triple;
} GRAPH;
typedef GRAPH * PGRAPH; 

PGRAPH new_graph(PGRAPH *);
PGRAPH delete_graph(PGRAPH *);
void close_update_graph(PGRAPH *);
int append_graph(PGRAPH *,TRIPLE );
int del_table_graph(PGRAPH *);

//void reset_G_columns();

int empty_graph(PGRAPH );
void graph_counts() ;
void reset_graphs(PGRAPH );
int reset_graph(PGRAPH );
PGRAPH dup_graph(PGRAPH l1,PGRAPH l2);
void pass_parent_graph(PGRAPH );

PGRAPH new_graph_by_index(int);
PGRAPH *LIST(int i);
