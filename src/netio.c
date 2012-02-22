/*
engine netio start up
git push git@github.com:Matt-Young/Semantic-Machine

In the lab, stand alone configuration runs under windows studio on the cheap
setting either threads or netio then you need to run under cygwin or under linux.
the lab configuratio, the threads only and the netio
*/
#define DebugPrint 



#include "./include/config.h"
#include "../socketx/socket_x.h"

#include "../src/include/g_types.h"
#include "../src/include/config.h"
//#include "../src/include/engine.h"
#include "../src/include/http_hdrs.h"
int set_web_addr(Webaddr *,int );
#define error printf
#define warn printf
#define NTHREAD 16 

/* Globals */
int sockfd = -1;
static void crit(char * message);
typedef struct { 
  Webaddr remote_addr; 
  int count;
  int type;
} Pending;  // Holds things a thread needs
Pending pendings[NTHREAD];
int thread_count=0;
int triple(Triple *top,Handler);
int header_magic(int newfd,int * count) {
  char inbuffer[HEADER_SIZE];
  int rv; int type;
  type = -1;
  rv = recv(newfd, inbuffer, HEADER_SIZE,0);
  if(rv != -1 && rv == HEADER_SIZE) {
    if(!strncmp(inbuffer,JSON_TYPE,MAGIC_SIZE)) type = Json_IO;
    else if(!strncmp(inbuffer,BSON_TYPE,MAGIC_SIZE)) type = Bson_IO;
    else if(!strncmp(inbuffer,BSON_TYPE,MAGIC_SIZE)) type = Qson_IO;
  }
  if(type != -1)
    sscanf(inbuffer+ MAGIC_SIZE,"%8d",count);
  else
    *count=0;
  printf("\nCOUNT ");
  fwrite(inbuffer,1,*count,stdout);
  return (type);
}
int event_handler(Triple *t);
void * handle_data(void * arg) {
  int status=0;
    int fd;int rv,rm;
    Webaddr dest;
  Pending *p = (Pending *) arg;
  fd = p->remote_addr.fd;
  BC.new_thread_count++;
  DebugPrint("handler count %d\n",p->count);
  fd = p->remote_addr.fd;
  dest.buff = (int *) G_malloc(p->count);
  BC.new_data_count++;
  rv = recv(fd, (char *) dest.buff, p->count,0);
  if(rv < p->count) {
    if((rm = send(fd, BAD_MSG, strlen(OK_MSG), 0)) == -1) 
      warn("Error sending data to client.");
    closesocket(fd);
  }
  else {
    if((rm = send(fd, OK_MSG, strlen(OK_MSG), 0)) == -1) 
      warn("Error sending data to client.");
    closesocket(fd);
    //machine_lock();
     set_web_addr(&p->remote_addr,sizeof(p->remote_addr));
     dest.sa_family = AF_TABLE;
     p->remote_addr.count = p->count;
     strcpy((char *) dest.addr,"netio");
    system_copy_qson(&p->remote_addr,&dest ); 
    //machine_unlock();
    DebugPrint(" Action %d ",status);
//    print_triple(&t);
  }
 // free(t.key);
  BC.del_data_count++;
  BC.del_thread_count++;
  p->count = 0;
  return 0;
}

static void crit(char * message) {
  fprintf(stderr, "%s\n", message);
  exit(1);
}

void * net_service (void * port)  {
  Pending pendings[NTHREAD];
  int sockfd = -1;
  int status=0;
  struct sockaddr_in my_addr;
  struct sockaddr_in remote_addr;
  int newfd,count,type;
  int i, rv,sin_size;
  pthread_t thread;
  printf("Net Service\n");
  memset(pendings,0,sizeof(pendings));
  SocketStart();
  sockfd = socket (AF_INET, SOCK_STREAM, 0);
  if(sockfd == -1) printf("Couldn't create socket.");
  memset (&(my_addr.sin_zero),0, sizeof(my_addr));
  my_addr.sin_family = AF_INET;
  my_addr.sin_port = htons ((int) port);
  my_addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(sockfd, (struct sockaddr *) &my_addr,  sizeof (struct sockaddr)) == -1)
    error("Stern: Couldn't bind to specified port.");
  sin_size = sizeof(struct sockaddr_in);
  if(listen(sockfd, 25) == -1) printf("Couldn't listen on specified port.");
  memset(pendings,0,sizeof(pendings));
  printf("Listening for connections on port %d...\n", port);

  while(1) {
    newfd = accept(sockfd, (struct sockaddr *)&remote_addr, &sin_size);
    if(newfd == -1) printf("Couldn't accept connection!");


    type = header_magic(newfd,&count); // Consume header
    printf("Connection %d\n",type);
    if(type < 0) {
      if((rv = send(newfd, BAD_MSG, strlen(BAD_MSG), 0)) == -1) 
        warn("Error sending data to client.");
      closesocket(newfd);
    } else if(type >= 0){
      printf("Count: %d\n",count);
      i=0;
      while(pendings[i].count && i < NTHREAD) i++;
      if(i==NTHREAD) {
        if((rv = send(newfd, PORT_MSG, strlen(PORT_MSG), 0)) == -1)  
          warn("Error sending data to client.");
        closesocket(newfd);
      } 
      else {
        printf("Doing %d\n",status);
        pendings[i].type = type; 
        pendings[i].count = count;
        memcpy(&pendings[i].remote_addr,&remote_addr,sizeof(remote_addr));
         pendings[i].remote_addr.fd = newfd;
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
  printf("Start netio \n");
  status = pthread_create(&thread,0,net_service,(void *) port);
  printf("Old thread \n");
  if(status == -1) {
    printf("Error threading");
    exit(1);
  }
  return 0;
}

#define error printf
// Little sender
int Sqlson_to_Bson(Triple t[],char ** buff);
int send_buff(char *buffer,int count,void * ip_addr)
{
  struct sockaddr_in ip4addr;
  int sockfd, portno, n;
//  struct addrinfo *result; //,hints;
  //struct sockaddr server;
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
