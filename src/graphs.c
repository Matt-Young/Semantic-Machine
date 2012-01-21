
#include "../include/sqlite3.h"
#include "g.h"
#include "graphs.h"

// every graph is an open subraph of its parent. 
// the table holds a permanent pointer the the 
// current innermost graph on a descending sequence.

int del_count=0,new_count=0;
void restart_graph(PGRAPH graph) {
  graph->row = graph->start;
}
void pass_parent_graph(PGRAPH graph) {
PGRAPH parent = graph->parent;
if(!parent)
  return;
parent->start = graph->end;
parent->row = graph->end;
}
int empty_graph(PGRAPH graph) {
  if(graph->row == graph->start)
    return(1);
  return(0);
}
int reset_graph(PGRAPH graph) {
  graph->row=0;
  graph->start=0;
  graph->match_state = G_START;
  return G_START;
}
PGRAPH dup_graph(PGRAPH l1,PGRAPH l2) {
  l1->row = l2->row;
  l1->start = l2->row;
  l1->table = l2->table;
  l1->end = l2->end;
  l1->parent = l2;
  return l1;
}
void reset_graphs(PGRAPH inner) {  
  while(inner) {
      reset_graph(inner);
	  inner = inner->parent;
  }
}

PGRAPH new_graph(PGRAPH *outer) {
  PGRAPH inner;
  inner = (PGRAPH) G_calloc(sizeof(GRAPH));

  if(*outer) dup_graph(inner,(*outer));
  else inner->table = TABLE_POINTER(2);
    inner->match_state = G_START;
  *outer = inner;
  new_count++;
  return inner;
}

PGRAPH delete_graph(PGRAPH *list) {
  PGRAPH parent,child;
  child = *list;
  if(!child)
	  return 0;
  if(del_count >= new_count)
    G_error("Bad graph",G_ERR_GRAPH);
  parent = child->parent;
  if(parent)
    parent->row = child->row;
  G_free((void *) child);
  *list = parent;
  del_count++;
  return parent;
}
void close_update_graph(PGRAPH *list) { 
  int status; char buff[20];
  PGRAPH child = *list;
  TABLE *table = child->table;
  G_sprintf(buff,"%d",table->index);  
  table->update_triple.key = buff;
  status = triple(table->update_triple,0);
  //pass_parent_graph(*list);
  delete_graph(list);
}
int del_table_graph(PGRAPH *inner) {
 if(*inner) {
	DELETE_TABLE((*inner)->table);
	while(*inner)
		*inner = delete_graph(inner);
 }
  return 0;
}

int append_graph(PGRAPH *list,TRIPLE node) {
  int status=0;
  char buff[20];
  PGRAPH g = *list;
  TABLE *table = g->table;
  if(table->index == 0)
    print_triple(node);
  else {
    G_sprintf(buff,"%d",table->index);
    g->pending_triple = node;
    table->insert_triple.key = buff;
    status = triple(table->insert_triple,0);
    g->row++;
  }
  return(status);
}

void graph_counts() {
  G_printf("%d %d \n",del_count,new_count);
}


