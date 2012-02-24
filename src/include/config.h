// any configuration parameters the underlying machine needs
// It is included by the machine layer source
// CYGWIN defined for that environment


//#define STANDALONE
#define SERVER_NAME "Joe"
#define VERSION "Jane"
#define TEST_PORT 8000
//#define TEST_ADDR "127.0.0.1"
#undef TEST_ADDR
#define NETIO
#define WINDOWS
#define TESTFILE "/test.txt"
#define TESTDIR "c:/soft/testfiles"
#define THREAD_MAX 16 
#ifdef BUFFER_TRACKING
//void G_free_buff(void* p);
//void* G_new_buff(int size);
#define malloc G_new_buff
#define free G_free_buff
#endif

