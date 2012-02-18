#define WINDOWS
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <direct.h>
#include "../socketx/socket_x.h"

#include "g_types.h"
#include "config.h"
#include "console.h"
#include "http_hdrs.h"

#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else
//#include <unistd.h>
#define GetCurrentDir getcwd
#endif
#define error printf
// Little sender
char  ip_addr[20] = "127.0.0.1";



int send_buff(char *buffer,int count)
{
  struct sockaddr_in ip4addr;
  int sockfd, portno, n;
  struct addrinfo *result,hints;
  struct sockaddr server;
  portno = TEST_PORT;
  ip4addr.sin_family = AF_INET;
  ip4addr.sin_port = htons(8000);
  n = inet_pton(AF_INET, "127.0.0.1", &ip4addr.sin_addr);
  sockfd = socket(PF_INET, SOCK_STREAM, 0);
  n=connect(sockfd,(struct sockaddr *) &ip4addr, sizeof(ip4addr)); 
if(n) {
  printf("Connect error \n");
    return EV_Error;
}
  n = send(sockfd,buffer,count,0);
  if (n < 0)  error("ERROR writing to socket");
  memset(buffer,0,256);
  n = recv(sockfd,buffer,255,0);
  if (n < 0)  error("ERROR reading from socket");
  printf("%s\n",buffer);
  closesocket(sockfd);
  return 0;
}
Console console;

#define plus(a,b,c) a+b+c
int main(int argc,char *argv[]){
  char buffer[1024];
  printf("|%s| \n",GetCurrentDir(buffer,1024));
  printf("|%d| \n",plus(2,4,7));
  SocketStart();
  if(argc > 1)
    if( !strcmp(argv[1], "-ip")) 
      strncpy(ip_addr, argv[2],20);
  printf("IP: %s\n",ip_addr);
  for(;;) {
    G_console(&console);
    // printf("C: %s %d %d\n",console.base,strlen(console.base),console.count);
    if(strlen(console.base)) {
      strncpy(buffer,JSON_TYPE,MAGIC_SIZE);
      sprintf(buffer+MAGIC_SIZE,"%8d\n\r\n\r",console.count);
      strncpy(buffer+HEADER_SIZE,console.base,console.count);
      send_buff(buffer,HEADER_SIZE+console.count);
    }
  }
}

/*
struct addrinfo {
    int              ai_flags;
    int              ai_family;
    int              ai_socktype;
    int              ai_protocol;
    size_t           ai_addrlen;
    struct sockaddr *ai_addr;
    char            *ai_canonname;
    struct addrinfo *ai_next;
};
*/