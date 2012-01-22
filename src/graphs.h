

PGRAPH self_graph();
PGRAPH other_graph();
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
#define pop_triple_operator 0
#define append_triple_operator 1
#define triple_data 2
#define update_triple_operator 3
#define installed_triple_operator 4

// events and properties
#define EV_Null 0x01  // Do some null operation
#define EV_No_bind 0x02 // This event requires no binding
#define EV_Immediate 0x04  //The statement in the key value
#define EV_Set 0x08  // Operator begines a new descent elemtn for the curren set
#define EV_Matchable 0x10  // Key value is matchable
#define EV_Matched 0x20  // And matched
#define EV_Square 0x40 // Square table looms ahead
#define EV_Operand 0x80 // Operand containd the statement
#define EV_Incomplete 0x100  // DUnno



#define EV_Sql_Done EV_Null

