
typedef struct  g {
  int row;
  int start;
  int end;
  TABLE * table;
  struct g * parent;
  Triple pending_triple;
  int rowid;
} GRAPH;
typedef GRAPH * PGRAPH; 

PGRAPH new_child_graph(PGRAPH *);
PGRAPH new_graph_context();
PGRAPH delete_graph(PGRAPH *);
void close_update_graph(PGRAPH *);
int append_graph(PGRAPH *,Triple );
int del_table_graph(PGRAPH *);
PGRAPH get_table_graph(int index);
PGRAPH new_table_graph(TABLE * table);
int convert_start(PGRAPH g);
int convert_row(PGRAPH g);
int convert_end(PGRAPH g);
int count_graph(PGRAPH );
void graph_counts() ;
void reset_graphs(PGRAPH );
int reset_graph(PGRAPH );
PGRAPH dup_graph(PGRAPH l1,PGRAPH l2);
void pass_parent_graph(PGRAPH );
PGRAPH new_graph_by_index(int);
// layout of triplet installed in table contaxts
enum {
	pop_triple_operator,
	append_triple_operator,triple_data_0,
	update_triple_operator,triple_data_1,
	select_triple_operator, installed_triple_operator};



