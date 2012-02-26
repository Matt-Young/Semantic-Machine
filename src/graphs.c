
#include "../src/include/g_types.h"
#include "../src/include/config.h"
#include "../src/include/machine.h"
#include "../src/include/tables.h"
#include "../src/include/graphs.h"
#include "../src/include/engine.h"
#include "../src/include/console.h"
#undef DBG_GRAPHS
#ifndef DBG_GRAPHS
#define G_printf
#endif
void list_graphs(PGRAPH  *list,char*);
// every graph is an open subraph of its parent. 
// graph map sqlson sub graphs in the database

PGRAPH new_graph_context() {
  PGRAPH p;
	BC.new_graph_count++;
	 p= (PGRAPH) G_malloc(sizeof(GRAPH));
   G_memset(p,0,sizeof(GRAPH));
  return p;
}
PGRAPH free_graph_context(PGRAPH  child) {
	PGRAPH parent=child->parent;
	if(BC.del_graph_count >= BC.new_graph_count)
		G_printf("Bad graph",EV_Error);
	G_free((void *) child);
	BC.del_graph_count++;
	return parent;
}	

int count_graph(PGRAPH graph) {
	return(graph->rdx.row );
}

PGRAPH dup_graph(PGRAPH l1,PGRAPH l2) {
  G_memset(&l1->rdx,0,sizeof(RowSequence));
	l1->table = l2->table;
  l1->rdx.end = l2->rdx.end- l2->rdx.row;
  l1->rdx.rowoffset = l2->rdx.rowoffset + l2->rdx.row;
	l1->parent = l2;
  l1->rdx.row=0;
	return l1;
}
PGRAPH new_child_graph(PGRAPH *list,void * context) {
	PGRAPH child,parent;
   parent = *list;
	child = new_graph_context();
  child->context = context;
  if(!parent) {
     G_printf("First Parent \n");
    child->rdx.rowoffset=1;
    *list = child;
    }
  else
	  dup_graph(child,parent);
  *list=child;
  set_row_sequence(&child->rdx);  // First on the list control row sequencers
    list_graphs(list,"new ");
	return child;
}
PGRAPH delete_graph(PGRAPH *list) {
	PGRAPH child,parent;
	child = *list;
	if(!child) {
    G_printf("del no child\n");
		return 0;
  }
  parent = child->parent;
	(*list) = child->parent;
	if(child->parent) {
		child->parent->rdx.row += child->rdx.row;
   set_row_sequence(&parent->rdx);  // parent becomes child
   } else G_printf("No parent\n");
 
	free_graph_context(child);
    list_graphs(list,"del ");
	return *list;
}

void close_update_graph(PGRAPH *list) { 
	int status;
  PGRAPH parent;
  TABLE * table;
  parent = (*list);
	if(!parent) {
    G_printf("cuo no parent\n");
		return;
  }
	 table = parent->table;
   if(!table) status = EV_Error;
   else {
	 status = machine_new_operator(&table->operators[update_operator],0);
  delete_graph(list);
   }
    list_graphs(list,"cu");
}
int release_graph_list(void *i) {
  PGRAPH * inner =(PGRAPH *) i;
	if( *inner) {
		while(*inner)
			*inner = delete_graph(inner);
	}
	return 0;
}

int append_graph(PGRAPH *list,Triple node) {
	int status=0;
  PGRAPH parent; 
  TABLE * t;
  parent = (*list);
  if(!parent) 
    status = EV_Error;
  else {
    t = parent->table;
    if(!t) status= EV_Error;
    else {
	    t->operators[append_text_data] = node;
	    status = machine_new_operator(&t->operators[append_text_operator],0);
    }
  }

  list_graphs(list,"app");
	return(status);
}
Pointer new_table_graph(TABLE *table) {
	PGRAPH gr =  new_graph_context();
	table->list =  (Pointer) gr;
	gr->table = table;
	gr->rdx.rowoffset=1; // SQL offset
	return table->list;
}

void list_graphs(PGRAPH  *list,char * arg){
  PGRAPH g;
  int i=0;
  g = *list;
  while(g) {
    // G_printf("%s %d %x %x ",arg,i,g->parent); 
     //G_printf(" %d %d %d\n",g->rdx.row,g->rdx.end,g->rdx.rowoffset); 
     g=g->parent;i++;
  }
}

void * graph_variable(PGRAPH graph) { 
  if(graph) return graph->context; else return 0;}
     