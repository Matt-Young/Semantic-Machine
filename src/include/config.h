
#define SERVER_NAME "Joe"
#define VERSION "Jane"
#define ORIGIN "c:/qson/"
#define GBASE "gbase"
#define NETIO
#define WINDOWS

//#define TEST_ADDR "127.0.0.1"
#define TEST_PORT 8000
#define TESTFILE "test.txt"
#define TESTDIR "c:/soft/testfiles/"
#undef TEST_ADDR
#define THREAD_MAX 16 
// buffer tracking
#undef BUFFER_TRACKING
#ifdef BUFFER_TRACKING
void G_free_buff(void* p);
void* G_new_buff(int size);
#define G_malloc G_new_buff  
#define G_free G_free_buff 
#else
void G_free(void* p);
void* G_malloc(int size);
#endif

