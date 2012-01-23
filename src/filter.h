typedef struct f { 
  int status;
  PGRAPH g[2];
  Triple * event_triple;
  TABLE * event_table;
  int properties;
  struct f *prev,*sibling;
} FILTER;
int event_exec(FILTER *f);