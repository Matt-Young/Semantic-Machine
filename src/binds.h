
typedef int (*Mapper)(Pointer *pointer,int *type);
typedef struct  {char *name; Mapper mapper;} NamedAccessor;
extern NamedAccessor binders[];
extern Triple G_null_graph;
int init_binders();
int new_binders(NamedAccessor[]);
void print_binders();
Mapper  find_binder(char * name);


// The binding system


