typedef struct f { 
  int status;
  PGRAPH g[2];
  Triple * event_triple;
  TABLE * event_table;
  int properties;
  struct f *parent,*sibling;
} FILTER;
int event_exec(FILTER *f);
FILTER *delete_filter(FILTER * f);
FILTER * new_filter_context(FILTER * parent);
FILTER * close_filter(FILTER * f);
int delete_filter_contents(FILTER * f);
extern FILTER null_filter;
int isnull_filter(FILTER * f);
// events and properties
