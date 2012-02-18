#include "../socketx/socket_x.h"


int closesocket_x(int sock) {
#ifdef WINDOWS
  return closesocket(sock);
#else
  return close(sock);
#endif
}
int SocketStart(){
#ifdef WINDOWS
  WSADATA wsaData;
  WSAStartup(MAKEWORD(2,2), &wsaData);
#endif
  return 0;
  }

int SocketCleanup() {
#ifdef WINDOWS
  WSACleanup();
#endif
  return 0;}

