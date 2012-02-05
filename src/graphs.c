
#include "all.h"

// every graph is an open subraph of its parent. 
// the table holds a permanent pointer the the 
// current innermost graph on a descending sequence.

extern int del_graph_count,new_graph_count;
PGRAPH new_graph_context() {
	new_graph_count++;
	return (PGRAPH) G_calloc(sizeof(GRAPH));
}
PGRAPH free_graph_context(PGRAPH  child) {
	PGRAPH parent=child->parent;
	if(del_graph_count >= new_graph_count)
		G_error("Bad graph",G_ERR_GRAPH);
	G_free((void *) child);
	del_graph_count++;
	return parent;
}	
void restart_graph(PGRAPH graph) {
	graph->row = graph->start;
}
int offset_row(PGRAPH g) {
	return (g->rowid);
}

void pass_parent_graph(PGRAPH graph) {
	PGRAPH parent = graph->parent;
	if(!parent)
		return;
	parent->start = graph->end;
	parent->row = graph->end;
}
int count_graph(PGRAPH graph) {
	return(graph->row - graph->start);
}
int reset_graph(PGRAPH graph) {
	graph->row=0;
	graph->start=0;
	return 0;
}
PGRAPH dup_graph(PGRAPH l1,PGRAPH l2) {
	l1->row = l2->row;
	l1->start = l2->row;
	l1->table = l2->table;
	l1->end = l2->end;
	l1->rowid = l2->rowid+l1->start;
	l1->parent = l2;
	return l1;
}
void reset_graphs(PGRAPH inner) {  
	while(inner) {
		reset_graph(inner);
		inner = inner->parent;
	}
}

PGRAPH new_child_graph(PGRAPH *outer) {
	PGRAPH inner;
	if(!(*outer)) return(0);  // call new_table_graph first
	inner = new_graph_context();
	dup_graph(inner,(*outer));
	*outer = inner;
	return inner;
}

PGRAPH delete_graph(PGRAPH *list) {
	PGRAPH child;
	child = *list;
	if(!child)
		return 0;
	(*list) = child->parent;
	if(child->parent)
		child->parent->row = child->row;
	free_graph_context(child);
	return *list;
}
void close_update_graph(PGRAPH *list) { 
	int status; 
	TABLE * table = (*list)->table;
	Triple * data = &table->operators[triple_data_1];
	data->key= "Not needed";
	status = triple(&table->operators[update_triple_operator],0);
	delete_graph(list);
}
int release_graph_list(PGRAPH *inner) {
	if(*inner) {
		while(*inner)
			*inner = delete_graph(inner);
	}
	return 0;
}

int append_graph(PGRAPH *list,Triple node) {
	int status=0;
	TABLE * t = (*list)->table;
	t->operators[append_triple_data] = node;
	status = triple(&t->operators[append_triple_operator],0);
	(*list)->row++;
	return(status);
}



