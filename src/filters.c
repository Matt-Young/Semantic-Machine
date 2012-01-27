#include "all.h"
#include "filter.h"
// Null is always operational
FILTER null_filter;
FILTER * filter_list;
int newfilter=0;
int oldfilter=0;
int isnull_filter(FILTER * f) {
	if(f == &null_filter)
		return 1;
	return 0;
}
FILTER * new_filter(FILTER * parent) {
	FILTER * f;
	if(!parent || !filter_list)
		return 0;
	 f = (FILTER *) G_calloc(sizeof(FILTER));
    newfilter++;
    f->parent = parent;
    f->status =  parent->status;
    return(f);
  }
FILTER * close_filter(FILTER * f) {
  delete_graph((PGRAPH *) f->g[0]->table->list);
  delete_graph((PGRAPH *) f->g[1]->table->list);
  return delete_filter(f);
}
FILTER *delete_filter(FILTER * f) {
  FILTER * g;
  if(oldfilter >= newfilter) 
    G_error("Filter",G_ERR_FILTER);
  g = (FILTER *) f->parent; 
  oldfilter++; 
  G_free( (void *) f);
  return g;
  }
int delete_filter_contents(FILTER * f) {
	PGRAPH g;
	g = f->g[0];
	if(g) release_graph_list((PGRAPH *)g->table->list);
	release_table_context(f->event_table);
	delete_filter(f);
	return(0);
}
int init_filters() {
	G_memset(&null_filter,0,sizeof(null_filter));
	filter_list=&null_filter;
	null_filter.g[0]=0;
	null_filter.g[1]=0;
	null_filter.properties= EV_Null;
	return init_table("console",1,&null_filter.event_table);
};
