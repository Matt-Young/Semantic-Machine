/**
 * ... g_types.h basic type unique to qson
 */
#define GTYPES
typedef struct {
    int link;
  int pointer;
  char  * key; 

} Triple;

typedef void * Pointer; 
typedef int (*Handler)(Triple *);
typedef int (*Mapper)(Pointer *pointer,int *type);
typedef void * Code;
typedef struct g * Pstruct;
  // Generic context to sqlite3
extern Pointer g_db;

/**
 * ... g_types.h basic type unique to qson
 */
enum qson_base_types {G_TYPE_NONE,G_TYPE_INTEGER,G_TYPE_FLOAT,
	G_TYPE_TEXT,G_TYPE_BLOB,G_TYPE_BIT,G_TYPE_TRIPLE,G_TYPE_CODE,
	G_TYPE_MAPPER,G_TYPE_HANDLER,G_TYPE_TABLE,
	G_TYPE_SYSTEM,G_TYPE_NAME,G_TYPE_USER,G_TYPE_ADDR,G_TYPE_OLD_TRIPLE,};
	
#define AF_INET 2

#define AF_INET6 23

typedef struct IO_Structure {   
  unsigned short  sa_family; unsigned short port;
  long addr[5]; int fd; int format;
  int * buff; int count; int size; char * fill; char * empty;struct  IO_Structure *link; 
  } IO_Structure;
  IO_Structure * new_IO_Struct();
  IO_Structure * del_IO_Struct(IO_Structure *);
  void del_IO_Structs();
  int mem_delete(IO_Structure *w);


  // Qson io formas
#define AF_JSON  0x70e4
#define AF_FILE  0x70e3
#define AF_CONSOLE 0x70e2
#define AF_TABLE   0x70e1
#define AF_MEMORY  0x70e0
int system_copy_qson(IO_Structure *,IO_Structure * );
void init_io_struct();
void wait_io_struct();
void post_io_struct();
void del_io_structs();
// Two sub types on IO_Structure
enum SerializedFormat { Json_IO,Bson_IO,Qson_IO};

// The null value and other defaults
#define G_TYPE_NULL 95
#define GBASE "c:/soft/gbase"
#define GFUN "gfun"
#define SystemNameSpace "local"
#define LocalSymbolCount 100

void unbind_triple(Code stmt,Triple *t);

extern Triple _null_graph;

#define EV_Ok 0x00
#define EV_Null 0x01  // Do some null operation
#define EV_Data 0x02
#define EV_Done	0x04 
#define EV_Error 0x08 
#define EV_Graphs 0x70
#define EV_Comma 0x80  // Key value is matchable
#define EV_Matchable 0x100  // Key value is matchable
#define EV_Overload 0x200
#define EV_No_bind 0x400 // This event requires no binding
#define EV_Immediate 0x800  //The statement in the key value
#define EV_Matched 0x1000  // And matched
#define EV_Square 0x2000 // Square table looms ahead
#define EV_Incomplete 0x4000 // Square table looms ahead
#define EV_Debug 0x8000
#define EV_Ugly 0x10000
#define EV_Forms 0xe0000  // system calls use these
#define EV_FormZero 0x2000
#define EV_FormOne 0x4000
#define EV_FormTwo 0x6000
typedef struct {
int del_graph_count,new_graph_count;
int del_table_count,new_table_count;
int del_data_count,new_data_count;
int del_thread_count,new_thread_count;
int del_name_count,new_name_count;
int del_web_count,new_web_count; } BufferCount;
extern   BufferCount BC;
