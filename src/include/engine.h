
typedef struct Operand {
  Mapper maps[4]; //map handlers
  Code stmt;
  Handler handler;
  int properties;
  } OP;

extern OP operands[];
#define GCHAR '_'
// Operator space on the predicate
enum SysCalls { SystemExit,SystemCall,SystemDup,SystemPop,
		SystemExec,SystemScript,SystemDecode,SystemConfig,SystemName,SystemEcho};

  /*! \defgroup  Readyset
 * Read set and run variables
 */


#define SystemMax SystemEcho
#define SystemUser SystemMax + 6
#define SystemScratch SystemUser +1
#define SystemNull '_'
#define OperatorMaximum 128
#define OperatorMask 0x7f
#define OperatorMSB 0x80
// Overloads in the first byte
enum { OperatorJson = OperatorMSB,OperatorBsonIn,OperatorBsonOut};
/*! \ingroup  Readyset
 * The compiled statement is always availale when its valid
 */
Code set_ready_stmt(Code stmt);
/*! \ingroup  Readyset
 * Which of the 256 possible triple operations caused this
 */
int set_ready_code(int opid);
int get_system_call(char * name);
/*! \ingroup  Readyset
 * Events occur when a triple runs through the pipe, and must be reset by handers
 */
int set_ready_event(int EV_event);
/*! \ingroup  Readyset
 * Grab the statement to step, or get results
 */
Code get_ready_stmt();
/*! \ingroup  Readyset
 * Reset particular events flags, run the machine and see if any got sent
 */
int reset_ready_event(int EV_event);
/*! \ingroup  Readyset
 * A fast bind, for rapid rerunning of a compiled statement
 */
int bind_code(Triple *,Code );
/*! \ingroup  Readyset
 * A generic hold holding information needed to kmove a Qson along
 */
void  * get_web_addr();
/*! \ingroup  Readyset
 * The webaddr is kept in the ready set
 */
void * set_web_addr(void  *,int);
int set_ready_graph(void *);

int exit_handler(Triple *node);


/**
 * ... Stepping Machine API ...
 */
Handler get_ghandler(Triple top[],Handler handler);
int pop_handler(Triple *node);
int call_handler_name(Triple *t);
