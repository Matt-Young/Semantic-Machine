
#include "../socketx/socket_x.h"
#ifndef GTYPES
#include "../src/include/g_types.h"
#endif
#include "../src/include/http_hdrs.h"
int closesocketx(int sock) {
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

int sendx(int sockfd, const void *buf, int len, int flags) {
  return send(sockfd,(const char *) buf,len,flags);}

int open_wabaddr(IO_Structure *w) {
  struct sockaddr_in *sock;
    struct sockaddr_in ip4addr;
  int  portno, n;
  sock = (struct sockaddr_in *) w;
  // int inet_pton(int af, const char *src, void *dst);
  portno = w->port;
  sock->sin_family = AF_INET;
  sock->sin_port = htons(portno);
  n = inet_pton(AF_INET, "127.0.0.1", &sock->sin_addr);
  w->fd = socket(PF_INET, SOCK_STREAM, 0);
  n=connect(w->fd,(struct sockaddr *) &sock, sizeof(ip4addr)); 
  if(n) 
  printf("Connect error \n");
    return 0;
}
/**
 * ... Magic headers
 */

//except anything that moves this version
int http_hdr_grunge(char * buff,int *len,char ** type,int* itype) {
  char * content;char * length;
  content= strstr(buff,"Content-Type:");
  if(!content) return -1;
  content += sizeof("Content-Type:");
  while(isspace(*buff)) content++;
  *type = content;
    if(strstr(content, MULTI_TYPE)) *itype = Json_IO;
  else if(strstr(content,JSON_TYPE)) *itype = Json_IO;
   else if(strstr(content,QSON_TYPE)) *itype = Qson_IO;
   else if(strstr(content,BSON_TYPE)) *itype = Bson_IO;
   else return -1;
  length = strstr(buff,"Content-Length:");
  if(!length) return -1;
  length += sizeof("Content-Length:");
  while(isspace(*buff)) length++;
  *strstr(length,"\r\n")=0;
  sscanf(length,"%d",len);
  *strstr(content,"\r\n")=0;
  return *len;
}

 int send_valid_http_msg(int fd) {
      int rv; char buff[8];
      if((rv = send(fd, HEADER_MSG, strlen(HEADER_MSG), 0)) == -1) 
        return -1;
      sprintf(buff,"%3d\r\n\r\n",strlen(PLAIN_MSG));
      if((rv = send(fd, buff, 7, 0)) == -1) 
        return -1;
      if((rv = send(fd, PLAIN_MSG, strlen(PLAIN_MSG), 0)) == -1) 
        return -1;
      return 0;
      }

