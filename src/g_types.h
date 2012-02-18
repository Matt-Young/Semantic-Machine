#ifndef G_TYPEDEFS
#define G_TYPEDEFS

typedef struct {
  char  * key; 
  int link;
  int pointer;
} Triple;
typedef void * Pointer; 
typedef int (*Handler)(Triple *);
typedef int (*Mapper)(Pointer *pointer,int *type);
typedef void * Code;
typedef struct g * Pstruct;
int net_start(void *);
/*
#define SQLITE_INTEGER  1
#define SQLITE_FLOAT    2
#define SQLITE_BLOB     4
#define SQLITE_NULL     5
# define SQLITE_TEXT     3
*/
// git push git@github.com:Matt-Young/Embedded-SQL.git
enum {G_TYPE_NONE,G_TYPE_INTEGER,G_TYPE_FLOAT,
	G_TYPE_TEXT,G_TYPE_BLOB,G_TYPE_BIT,G_TYPE_TRIPLE,G_TYPE_CODE,
	G_TYPE_MAPPER,G_TYPE_HANDLER,G_TYPE_TABLE,
	G_TYPE_SYSTEM,G_TYPE_NAME,G_TYPE_USER,G_TYPE_ADDR,G_TYPE_OLD_TRIPLE,};
#define AF_INET 2

#define AF_INET6 23

typedef struct {    unsigned short  sa_family; 
unsigned short pad[12];int * buff;int count;  int fd; char * data[20];} Webaddr;
// A console sa family types
#define AF_FILE  0x70e3
#define AF_CONSOLE 0x70e2
#define AF_TABLE   0x70e1
#define AF_MEMORY  0x70e0
int system_copy_qson(Webaddr *,Webaddr * );

// Two sub types on Webaddr
enum { Json_IO,Bson_IO,Qson_IO};

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
#endif


