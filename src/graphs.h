
typedef struct  g {
  int row;
  int start;
  int end;
  TABLE * table;
  struct g * parent;
  int match_state;
  Triple pending_triple;
} GRAPH;
typedef GRAPH * PGRAPH; 
PGRAPH self_graph();
PGRAPH other_graph();
PGRAPH new_graph(PGRAPH *);
PGRAPH delete_graph(PGRAPH *);
void close_update_graph(PGRAPH *);
int append_graph(PGRAPH *,Triple );
int del_table_graph(PGRAPH *);
int new_table_graph(TABLE * table);
PGRAPH get_table_graph(int index);
//void reset_G_columns();

int count_graph(PGRAPH );
void graph_counts() ;
void reset_graphs(PGRAPH );
int reset_graph(PGRAPH );
PGRAPH dup_graph(PGRAPH l1,PGRAPH l2);
void pass_parent_graph(PGRAPH );
PGRAPH new_graph_by_index(int);
#define pop_triple_operator 0
#define append_triple_operator 1
#define triple_data_0 2
#define update_triple_operator 3
#define triple_data_1 4
#define select_triple_operator 5
#define installed_triple_operator 6



