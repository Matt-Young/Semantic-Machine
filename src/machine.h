// any configuration parameters the underlying machine needs
// It is included by the machine layer source
// CYGWIN defined for that environment
//#undef __CYGWIN__
#undef NETIO
#define STANDALONE
#define SERVER_NAME "Joe"
#define VERSION "Jane"
#define NET_PORT 8000