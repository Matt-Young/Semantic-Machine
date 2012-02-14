/*
engine netio start up
git push git@github.com:Matt-Young/Semantic-Machine

In the lab, stand alone configuration runs under windows studio on the cheap
setting either threads or netio then you need to run under cygwin or under linux.
the lab configuratio, the threads only and the netio
*/
#include "config.h"
#ifdef NETIO
#define DebugNETIO 1
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
#include "../src/g_types.h"
#include "../src/config.h"
#include "../src/engine.h"
#include "../src/http_hdrs.h"
#define error printf
#define warn printf
#define NTHREAD 16 

/* Globals */
int sockfd = -1;
extern int del_data_count,new_data_count;
extern int del_thread_count,new_thread_count;
static void crit(char * message);
typedef struct { 
  int newfd;
  void * remote_addr; 
  int count;
  int type;
} Pending;  // Holds things a thread needs
Pending pendings[NTHREAD];
int thread_count=0;
int triple(Triple *top,Handler);
int header_magic(int newfd,int * count) {
  char inbuffer[HEADER_SIZE];
  int rv; int type;int i;
  type = -1;
  rv = read(newfd, inbuffer, HEADER_SIZE);
  if(rv != -1 && rv == HEADER_SIZE) {
    if(!strncmp(inbuffer,JSON_TYPE,MAGIC_SIZE)) type = 1;
    else if(!strncmp(inbuffer,BSON_TYPE,MAGIC_SIZE)) type = 0;
  }
  if(type != -1)
    sscanf(inbuffer+ MAGIC_SIZE,"%8d",count);
  else
    *count=0;
  return (type);
}
int event_handler(Triple *t);
void * handle_data(void * arg) {
  Triple t;int status;
  Pending *p = (Pending *) arg;
  int fd;
  new_thread_count++;
  struct sockaddr_in * remote = p->remote_addr;
  int rv,rm;
  printf("handler count %d\n",p->count);
  fd = p->newfd;
  t.key = (char *) malloc(p->count);
  new_data_count++;
  rv = read(p->newfd, t.key, p->count);
  if(rv < p->count) {
    if((rm = send(p->newfd, BAD_MSG, strlen(OK_MSG), 0)) == -1) 
      warn("Error sending data to client.");
    close(p->newfd);
  }
  else {
    if((rm = send(p->newfd, OK_MSG, strlen(OK_MSG), 0)) == -1) 
      warn("Error sending data to client.");
    t.key[rv]=0;
    close(p->newfd);

    if(p->type = 0)
      t.link = OperatorBsonIn;
    else if(p->type = 1) 
      t.link = OperatorJson;
    t.pointer = p->count;
    machine_lock();
    status = triple(&t,event_handler);
    machine_unlock();
    printf(" Action %d ",status);
    print_triple(&t);
  }
  free(t.key);
  del_data_count++;
  del_thread_count++;
  p->newfd = 0;

}

static void crit(char * message) {
  fprintf(stderr, "%s\n", message);
  exit(1);
}

void * net_service (void * port)  {
  Pending pendings[NTHREAD];
  int sockfd = -1;
  int status;
  struct sockaddr_in my_addr;
  struct sockaddr_in remote_addr;
  int newfd,count,type;
  int i, rv,sin_size;
  pthread_t thread;
  printf("Net Service\n");
  memset(pendings,0,sizeof(pendings));
  sockfd = socket (AF_INET, SOCK_STREAM, 0);
  if(sockfd == -1) printf("Couldn't create socket.");
  my_addr.sin_family = AF_INET;
  my_addr.sin_port = htons ((int) port);
  my_addr.sin_addr.s_addr = INADDR_ANY;
  bzero (&(my_addr.sin_zero), 8);

  if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof (struct sockaddr)) == -1)
    error("Stern: Couldn't bind to specified port.");
  sin_size = sizeof(struct sockaddr_in);
  if(listen(sockfd, 25) == -1) printf("Couldn't listen on specified port.");
  memset(pendings,0,sizeof(pendings));
  printf("Listening for connections on port %d...\n", port);
  //status = pthread_create(thread,0,handle_data, 0);

  while(1) {
    newfd = accept(sockfd, (struct sockaddr *)&remote_addr, &sin_size);
    if(newfd == -1) printf("Couldn't accept connection!");


    type = header_magic(newfd,&count); // Consume header
    printf("Connection %d\n",type);
    if(type < 0) {
      if((rv = send(newfd, BAD_MSG, strlen(BAD_MSG), 0)) == -1) 
        warn("Error sending data to client.");
      close(newfd);
    } else if(type >= 0){
      printf("Count: %d\n",count);
      i=0;
      while(pendings[i].newfd && i < NTHREAD) i++;
      if(i==NTHREAD) {
        if((rv = send(newfd, PORT_MSG, strlen(PORT_MSG), 0)) == -1)  
          warn("Error sending data to client.");
        close(newfd);
      } 
      else {
        printf("Doing %d\n",status);
        pendings[i].type = type; 
        pendings[i].newfd = newfd; 
        pendings[i].count = count;
        pendings[i].remote_addr =(struct sockaddr_in *)&remote_addr;
        status = pthread_create(&thread,0,handle_data, &pendings[i]);//&pendings[i]);
        printf("Done %d\n",status);
      }
    }
  }
  return 0;
}

int net_start(void * port) {
  pthread_t thread;
  int status;
  G_printf("Start netio \n");
  status = pthread_create(&thread,0,net_service,(void *) port);
  G_printf("Old thread \n");
  if(status == -1) {
    printf("Error threading");
    exit(1);
  }
  return 0;
}
///////////////////////////////////////
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

int send_buff(char *buffer,int count,char * ip_addr)
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
#endif