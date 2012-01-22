

PGRAPH self_graph();
PGRAPH new_graph(PGRAPH *);
PGRAPH delete_graph(PGRAPH *);
void close_update_graph(PGRAPH *);
int append_graph(PGRAPH *,TRIPLE );
int del_table_graph(PGRAPH *);
int new_table_graph(TABLE * table);
//void reset_G_columns();

int empty_graph(PGRAPH );
void graph_counts() ;
void reset_graphs(PGRAPH );
int reset_graph(PGRAPH );
PGRAPH dup_graph(PGRAPH l1,PGRAPH l2);
void pass_parent_graph(PGRAPH );

PGRAPH new_graph_by_index(int);
PGRAPH *LIST(int i);
// events and properties
#define EV_Graph_Layer 0x07
#define EV_Null 0x08
#define EV_Wild_Triple 0x10
#define EV_Incomplete 0x20
#define EV_Set 0x40
#define EV_Matchable 0x80
#define EV_Matched 0x100
#define EV_Square 0x200
#define EV_Sql_Done EV_Null
#define EV_Overide 0x400