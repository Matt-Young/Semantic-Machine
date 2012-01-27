#include "all.h"
#include "filter.h"
// Null is always operational
FILTER null_filter;
FILTER * filter_list;
extern int new_filter_count;
extern int old_filter_count;
int isnull_filter(FILTER * f) {
	if(f == &null_filter)
		return 1;
	return 0;
}
FILTER * new_filter_context(FILTER * parent) {
	FILTER * f;
	if(!parent || !filter_list)
		return 0;
	 f = (FILTER *) G_calloc(sizeof(FILTER));
    new_filter_count++;
    f->parent = parent;
    f->status =  parent->status;
    return(f);
  }
FILTER * delete_filter_context(FILTER * f) {
  FILTER * g = f->parent;
  if(old_filter_count >= new_filter_count) 
    G_error("Filter",G_ERR_FILTER);
  old_filter_count++;
  G_free( (void *) f);
  return g;
  }
FILTER * close_filter(FILTER * f) {
  delete_graph((PGRAPH *) f->g[0]->table->list);
  delete_graph((PGRAPH *) f->g[1]->table->list);
  return delete_filter(f);
}
FILTER *delete_filter(FILTER * f) {
  return delete_filter_context(f);
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
