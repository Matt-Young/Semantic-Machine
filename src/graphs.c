//#include "targetver.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <tchar.h>

typedef struct sqlite3_stmt sqlite3_stmt;
typedef struct sqlite3 sqlite3;
#include "g.h"
#include "graphs.h"

// every graph is an open subraph of its parent. 
// The root parent points to the whole table
// These graphs ount rows starting with zero, unlike sql
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
int reset_graph(PGRAPH graph) {
  graph->row=0;
  graph->start=0;
  graph->match_state = G_START;
  return G_START;
}
void reset_G_calls();
void reset_graphs(int k) {
  int i;
  if(k < 0)
    for(i=0;i<5;i++)
      reset_graph(*LIST(i));
  else
    reset_graph(*LIST(k));
  reset_G_columns(); // a cheat
}
PGRAPH dup_graph(PGRAPH l1,PGRAPH l2) {
  l1->row = l2->row;
  l1->start = l2->row;
  l1->table = l2->table;
  l1->end = l2->end;
  l1->parent = l2;
  return l1;
}
PGRAPH new_graph(PGRAPH *list) {
  PGRAPH child;
  child = (PGRAPH) calloc(1,sizeof(GRAPH));

  child->match_state = G_START;
  if(*list) dup_graph(child,*list);
  *list = child;
  new_count++;
  return child;
}

PGRAPH delete_graph(PGRAPH *list) {
  PGRAPH parent,child;
  child = *list;
  if(del_count >= new_count)
    gerror("Bad graph",G_ERR_GRAPH);
  parent = child->parent;
  if(parent)
    parent->row = child->row;
  free((void *) child);
  *list = parent;
  del_count++;
  return parent;
}
void close_update_graph(PGRAPH *list) { 
  int status; char buff[20];
  PGRAPH child = *list;
  TABLE *table = child->table;
  sprintf_s(buff,20,"%d",table->index);  
  table->update_triple.key = buff;
  status = triple(table->update_triple,0);
  //pass_parent_graph(*list);
  delete_graph(list);
}
int del_table_graph(PGRAPH *graph) {
  reset_graph(*graph);
  DELETE_TABLE((*graph)->table);
  return 0;
}
int empty_graph(PGRAPH graph) {
  if(graph->row == graph->start)
    return(1);
  return(0);
}
int append_graph(PGRAPH *list,TRIPLE node) {
  int status;
  char buff[20];
  PGRAPH g = *list;
  TABLE *table = g->table;
  if(table->index == 0)
    print_triple(node);
  else {
    sprintf_s(buff,20,"%d",table->index);
    g->pending_triple = node;
    table->insert_triple.key = buff;
    status = triple(table->insert_triple,0);
    g->row++;
  }
  return(status);
}

void graph_counts() {
  printf("%d %d \n",del_count,new_count);
}


