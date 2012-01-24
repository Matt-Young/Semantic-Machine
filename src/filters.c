#include "all.h"
#include "filter.h"
// Null is always operational
FILTER null_filter;
FILTER * filter_list;
int newfilter=0;
int oldfilter=0;
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
FILTER *f;
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
	if(g) del_table_graph((PGRAPH *)g->table->list);
	free_table_context(f->event_table);
	delete_filter(f);
	return(0);
}
int init_filters() {
	G_memset(&null_filter,0,sizeof(null_filter));
	filter_list=&null_filter;
	null_filter.g[0]=0;
	null_filter.g[1]=0;
	null_filter.properties= EV_Null;
	init_table("console",1,&null_filter.event_table);
	return(0);
};