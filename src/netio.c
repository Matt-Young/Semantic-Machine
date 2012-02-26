/*
engine netio 
git push git@github.com:Matt-Young/Semantic-Machine

It uses a header interface to perform the compile
switch between windows and linux
*/
#define BUFFER_TRACKING
#include "./include/config.h"
#include "../src/include/g_types.h"
#include "../socketx/socket_x.h"
#include "../src/include/machine.h"
#include "../src/include/http_hdrs.h"
int set_io_struct(IO_Structure * );
#define error printf
#define warn printf
#define error printf
static void crit(char * message);

// Hold the context of a finite set of threads
typedef struct  { 
  int count;
  int type;
  int fd;
  struct sockaddr_in remote_addr;
} TH_Struct;  // Holds things a thread needs
TH_Struct  thread_context[THREAD_MAX];
int thread_count=0;
int header_magic(int newfd,int * count) {
  char inbuffer[HEADER_SIZE];
  int rv; int type; char * content; int len;int i;
  type = -1;

  i=4; memset(inbuffer,0,sizeof(inbuffer));
  rv = recv(newfd, inbuffer,4,0);
  if(rv <= 0 ) {printf("Bad Header\n");return -1;}
  while(!strstr(&inbuffer[i-4],"\r\n\r\n")) {
  rv = recv(newfd, &inbuffer[i],1,0);
  if(rv <= 0 ) {printf("Bad Header\n");return -1;}
  i += 1;}
  inbuffer[i] = 0;
  for(len=0;len < i;len++) printf("%c",inbuffer[len]);
  http_hdr_grunge(newfd,inbuffer,&len,&content,&type);
  *count = len;
  //if(*count > 0 ) type = Json_IO; else type = -1;
  printf("type %d count %d\n",type,len);

  return (type);
}
 int init_run_table(IO_Structure *);
 void G_buff_counts();
void * handle_data(void * arg) {
  int status=0; char * buff;
    int fd,rv;
    IO_Structure *from,*to;
TH_Struct *p = (TH_Struct *) arg;
  fd = p->fd;
  BC.new_thread_count++;
  //printf("handler count %d\n",p->count);
  buff = (char *) G_malloc(p->count+4);
  rv = recv(fd, (char* )buff, p->count,0);
  if(rv < p->count) {
send_valid_http_msg(fd,0,0) ;
    G_free(buff);
    closesocket(fd);
        printf(" Bad data \n");
  }
  else {
    printf(" Good data \n");
    buff[p->count]=0;
    wait_io_struct();
    from = new_IO_Struct();
    to = new_IO_Struct();
    machine_lock();
    memcpy(from->addr,&p->remote_addr,sizeof(struct sockaddr_in));
     to->sa_family = AF_TABLE;
     from->sa_family= AF_INET;
     from->count = p->count;
     from->buff = (int *) buff;
     for(rv=0;rv < p->count;rv++) printf("%c",buff[rv]);
     strcpy((char *) to->addr,"netio");  // Table name
    system_copy_qson(from,to); 
       printf(" Action\n ");
     //init_run_table(from,to);
    machine_unlock();
send_valid_http_msg(fd,DONE_MSG,0);
     del_io_structs();
    post_io_struct();
    closesocket(fd);
      G_buff_counts();
  }
  BC.del_thread_count++;
  p->count = 0;
  return 0;
}

static void crit(char * message) {
  fprintf(stderr, "%s\n", message);
  exit(1);
}

void * net_service (void * port)  {
  TH_Struct thread_context[THREAD_MAX];
  int sockfd = -1;
  int status=0;
  struct sockaddr_in my_addr;
//   struct sockaddr_in remote_addr;
  int newfd,count,type;
  int thread_index,sin_size;
  pthread_t thread;
  printf("Net Service\n");
  memset(thread_context,0,sizeof(thread_context));
  SocketStart();  // Linux dummy call, WAS all for windows
  sockfd = socket (AF_INET, SOCK_STREAM, 0);
  if(sockfd == -1) printf("Couldn't create socket.");
  memset (&(my_addr),0, sizeof(my_addr));
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
    thread_index=0;
    while(thread_context[thread_index].count && thread_index < THREAD_MAX) thread_index++;
    if(thread_index== THREAD_MAX) {
      wait_io_struct();
      post_io_struct();
      warn("running out of thread space");

      continue;}
    newfd = accept(sockfd, 
      (struct sockaddr *) &thread_context[thread_index].remote_addr, 
      &sin_size);
    if(newfd == -1) { 
      printf("Couldn't accept connection!"); 
      continue; }

    type = header_magic(newfd,&count); // Consume header

    if(type < 0 || count == 0) {
     // send_valid_http_msg(newfd,0,0) ;
      closesocket(newfd);
     printf("\n Handled elsewhere \n");
    } else if(type >= 0){
      printf("\n Connection! \n");
      thread_context[thread_index].type = type; 
      thread_context[thread_index].count = count;
      thread_context[thread_index].fd = newfd;
      status = pthread_create(
        &thread,0,handle_data, 
        &thread_context[thread_index]);
      printf("Done %d\n",status);
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


// Little sender
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
