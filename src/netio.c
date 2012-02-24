/*
engine netio start up
git push git@github.com:Matt-Young/Semantic-Machine

In the lab, stand alone configuration runs under windows studio on the cheap
setting either threads or netio then you need to run under cygwin or under linux.
the lab configuratio, the threads only and the netio
*/
#define DebugPrint 

#include "./include/config.h"
#include "../src/include/g_types.h"
#include "../socketx/socket_x.h"
#include "../src/include/machine.h"
#include "../src/include/http_hdrs.h"
int set_web_addr(IO_Structure *,int );
#define error printf
#define warn printf
#define NTHREAD 16 

/* Globals */
int sockfd = -1;
static void crit(char * message);
typedef struct  { 
  int count;
  int type;
  int fd;
  struct sockaddr_in remote_addr;
} TH_Struct;  // Holds things a thread needs
TH_Struct  thread_context[NTHREAD];
int thread_count=0;
int triple(Triple *top,Handler);
int header_magic(int newfd,int * count) {
  char inbuffer[HEADER_SIZE];
  int rv; int type; char * content; int len;int i;
  type = -1;

  i=4; memset(inbuffer,0,sizeof(inbuffer));
  rv = recv(newfd, inbuffer,4,0);
  if(rv == 0 ) return 0;
  while(!strstr(&inbuffer[i-4],"\r\n\r\n")) {
  rv = recv(newfd, &inbuffer[i],1,0);
  i += 1;}
  inbuffer[i] = 0;
  http_hdr_grunge(inbuffer,&len,&content,&type);
  *count = len;
  //if(*count > 0 ) type = Json_IO; else type = -1;
  printf("\n content: %s type %d count &d\n",content,type,len);
  return (type);
}

void * handle_data(void * arg) {
  int status=0; char * buff;
    int fd;int rv,rm;
    IO_Structure *from,*to;
TH_Struct *p = (TH_Struct *) arg;
  fd = p->fd;
  BC.new_thread_count++;
  printf("handler count %d\n",p->count);
  buff = (char *) malloc(p->count+4);
  BC.new_data_count++;
  rv = recv(fd, (char* )buff, p->count,0);
  if(rv < p->count) {
    if((rm = send(fd, OK_MSG, strlen(OK_MSG), 0)) == -1) 
      warn("Error sending data to client.");
    free(buff);
    closesocket(fd);
  }
  else {
    buff[p->count]=0;
    wait_io_struct();
    from = new_IO_Struct();
    to = new_IO_Struct();
    machine_lock();
//     set_web_addr(&p->remote_addr,sizeof(p->remote_addr));
    memcpy(from->addr,&p->remote_addr,sizeof(struct sockaddr_in));
     to->sa_family = AF_TABLE;
     from->sa_family= AF_INET;
     from->count = p->count;
     from->buff = (int *) buff;
     strcpy((char *) to->addr,"netio");  // Table name
    system_copy_qson(from,to); 
    machine_unlock();
   if((rm = send(fd, OK_MSG, strlen(OK_MSG), 0)) == -1) 
      warn("Error sending data to client.");
     del_io_structs();
    post_io_struct();
    closesocket(fd);
    printf(" Action %d ",status);
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
  TH_Struct thread_context[NTHREAD];
  int sockfd = -1;
  int status=0;
  struct sockaddr_in my_addr;
   struct sockaddr_in remote_addr;
  int newfd,count,type;
  int i, rv,sin_size;
  pthread_t thread;
  printf("Net Service\n");
  memset(thread_context,0,sizeof(thread_context));
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
  memset(thread_context,0,sizeof(thread_context));
  printf("Listening for connections on port %d...\n", port);

  while(1) {
   // i=0;while(thread_context[i].count && i < NTHREAD) i++;
    newfd = accept(sockfd, 
      (struct sockaddr *) &remote_addr, 
    &sin_size);
    if(newfd == -1) printf("Couldn't accept connection!");
        printf("Connection %d\n",sin_size);
    type = header_magic(newfd,&count); // Consume header

    if(type < 0) {
      if((rv = send(newfd, OK_MSG, strlen(OK_MSG), 0)) == -1) 
        warn("Error sending data to client.");
            if((rv = send(newfd, HELLO_MSG, strlen(HELLO_MSG), 0)) == -1) 
        warn("Error sending data to client.");
      closesocket(newfd);
    } else if(type >= 0){
      printf("Count: %d\n",count);
      i=0;

      if(i==NTHREAD) {
        if((rv = send(newfd, OK_MSG, strlen(PORT_MSG), 0)) == -1)  
          warn("Error sending data to client.");
        closesocket(newfd);
      } 
      else {
        printf("Doing %d\n",status);
        thread_context[i].type = type; 
        thread_context[i].count = count;
         thread_context[i].fd = newfd;
        status = pthread_create(&thread,0,handle_data, &thread_context[i]);//&thread_context[i]);
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
