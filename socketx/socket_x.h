#include <pthread.h>
#define WINDOWS
#ifdef WINDOWS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
 #include <direct.h>

#define DEFAULT_FAMILY  AF_UNSPEC
#define DEFAULT_SOCKTYPE SOCK_STREAM
#define DEFAULT_PORT  "1234"
#define BUFFER_SIZE   23    // length of "WinCE Echo Test Packet"
#define DEFAULT_BUFLEN 256
#else

#include <dirent.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#endif
/**
 * ... Interfaces to make windows and linux compatible...
 */

int SocketCleanup();
int SocketStart();
