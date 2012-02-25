
#define SERVER_NAME "Joe"
#define VERSION "Jane"
#define ORIGIN "c:/soft/"
#define GBASE "gbase"
#define NETIO
#define WINDOWS

//#define TEST_ADDR "127.0.0.1"
#define TEST_PORT 8000
#define TESTFILE "test.txt"
#define TESTDIR "c:/soft/testfiles/"
#undef TEST_ADDR
#define THREAD_MAX 16 
#ifdef BUFFER_TRACKING
#define malloc G_new_buff
#define free G_free_buff
#endif

