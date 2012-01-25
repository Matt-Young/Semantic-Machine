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
FILTER * new_filter(FILTER * parent);
FILTER * close_filter(FILTER * f);
int delete_filter_contents(FILTER * f);
extern FILTER null_filter;
int isnull_filter(FILTER * f);
// events and properties

#define EV_Null 0x01  // Do some null operation
#define EV_No_bind 0x02 // This event requires no binding
#define EV_Immediate 0x04  //The statement in the key value
#define EV_Set 0x08  // Operator begines a new descent elemtn for the curren set
#define EV_Matchable 0x10  // Key value is matchable
#define EV_Matched 0x20  // And matched
#define EV_Square 0x40 // Square table looms ahead
#define EV_Operand 0x80 // Operand containd the statement
#define EV_Incomplete 0x100  // DUnno
#define EV_Debug EV_Overload
#define EV_No_data 0x200
#define EV_Done	0x200 