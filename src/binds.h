
typedef int (*Mapper)(Pointer *pointer,int *type);
typedef struct  {char *name; Mapper mapper;} NamedAccessor;
extern NamedAccessor binders[];
extern TRIPLE G_null_graph;
int init_binders();
int new_binders(NamedAccessor[]);


#define BIND_DEFAULT 1
#define BIND_UPDATE 2
#define BIND_OTHER 3
#define BIND_SELF 3
#define BIND_TRIPLE 5
#define BIND_MAPPER 6
#define BIND_GRAPH_ROW 0
#define BIND_GRAPH_START 1
#define BIND_GRAPH_END 2
// The binding system


