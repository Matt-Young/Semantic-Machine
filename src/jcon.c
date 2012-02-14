#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <dirent.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include "g_types.h"
#include "config.h"
#include "console.h"
#include "http_hdrs.h"
#undef WINDOWS
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
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    portno = TEST_PORT;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)  error("ERROR opening socket");
    server = gethostbyname(ip_addr);
    if (server == NULL) { error("ERROR, no such host\n");exit(0);}

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    n = write(sockfd,buffer,count);
    if (n < 0)  error("ERROR writing to socket");
    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0)  error("ERROR reading from socket");
   // printf("%s\n",buffer);
    close(sockfd);
    return 0;
}
Console console;


int main(int argc,char *argv[]){
  char buffer[1024];
  printf("|%s| \n",GetCurrentDir(buffer,1024));

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
