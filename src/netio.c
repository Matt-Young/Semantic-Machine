/*
engine netio start up
git push git@github.com:Matt-Young/Semantic-Machine

In the lab, stand alone configuration runs under windows studio on the cheap
setting either threads or netio then you need to run under cygwin or under linux.
the lab configuratio, the threads only and the netio
*/
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "../src/g_types.h"
#include "../src/machine.h"
#define STAND_ALONE
#ifdef STAND_ALONE
#undef NETIO
#else
#ifdef HAVE_SYS_SENDFILE_H
#include <sys/sendfile.h>
#endif
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

#define SERVER_NAME "Graph Machine"
#define NTHREAD 16 

/* Globals */
int sockfd = -1;


/* Prototypes */
void * handle_request(void *);

static int get_method(char * req);
static int safesend(int fd, char * out);
static char * get_mimetype(char * file);
static void crit(char * message);
static void warn(char * message);

  // Magic header, we reject everything else
#define JSON_TYPE "POST\r\nContent-Type:text/json\n\rContent-Length:"
#define BSON_TYPE "POST\r\nContent-Type:text/bson\n\rContent-Length:"
#define OK_MSG    "HTTP/1.0 200 OK\r\n\r\n"
#define PORT_MSG    "HTTP/1.0 989 No Ports\r\n\r\n"
#define BAD_MSG "HTTP/1.0 404 Not Found\r\n\r\n"
#define MAGIC_SIZE sizeof(JSON_TYPE)
#define HEADER_SIZE (MAGIC_SIZE+10)
typedef struct { 
  int newfd;
  void * remote_addr; 
  int count;
  int type;
} Pending;
Pending pendings[NTHREAD];
int triple(Triple *top,Handler);
int main_engine(int argc, char *argv[]);
void engine_init();
void * console_loop(void * arg);
void * netio_loop(void * arg);

int header_magic(int newfd,int * count) {
    char inbuffer[HEADER_SIZE];
    int rv; int type;int i;
    type = -1;
    rv = read(newfd, inbuffer, HEADER_SIZE);
   fwrite(inbuffer, 1, MAGIC_SIZE, stdout);
    fwrite("\n", 1, 1, stdout);
    //printf("%d %d",inbuffer,MAGIC_SIZE);
    for(i=MAGIC_SIZE;i<HEADER_SIZE;i++) printf("%2x|",(unsigned char) inbuffer[i]);
      fwrite("\n", 1, 1, stdout);
    if(rv != -1 && rv == HEADER_SIZE) {
     if(!strncmp(inbuffer,JSON_TYPE,MAGIC_SIZE)) type = 1;
     else if(!strncmp(inbuffer,BSON_TYPE,MAGIC_SIZE)) type = 0;
    }
    if(type != -1)
      *count = (int) inbuffer[MAGIC_SIZE];
    return (type);
    }
delete_thread(Pending *p) {
   close(p->newfd);
  p->newfd=0;
  pthread_exit((void *) 0);
}
void message_rejected(Pending *p) {
  int rv;
  if((rv = send(p->newfd, BAD_MSG, strlen(BAD_MSG), 0)) == -1) 
    warn("Error sending data to client.");
  close(p->newfd);
delete_thread(p);
	}
void message_accepted(Pending *p) {
  int rv;
  if((rv = send(p->newfd, OK_MSG, strlen(OK_MSG), 0)) == -1) 
		warn("Error sending data to client.");
  close(p->newfd);
	}

 void * handle_request(void * arg) {
    Triple t;
    Pending *p = (Pending *) arg;
    int fd = p->newfd;
    struct sockaddr_in * remote = p->remote_addr;
    char * data_buff;
    int rv;
    t.key = (char *) calloc(p->count,1);
    rv = recv(p->newfd, t.key, sizeof(p->count), 1);
     message_accepted(p);  // let the connection go away
     t.link = p->type | 0x80;
     t.pointer = p->count;
     triple(&t,0);
     free(t.key);
     delete_thread((void *) 1);
}

static void crit(char * message) {
  fprintf(stderr, "%s\n", message);
  exit(1);
}
static void warn(char * message) {
  fprintf(stderr, "%s\n", message);
}

int thread_count=0;
typedef struct { 
  int newfd;
  void * remote_addr; 
  int count;
  int type;
} Pending;
typedef void * (*SocketHandler)(void *);

void net_service ()  {
  Pending pendings[NTHREAD];
  int sockfd = -1;
  int port = TEST_PORT;
  struct sockaddr_in my_addr;
  struct sockaddr_in remote_addr;
  int newfd,count,type;
  int i, rv,sin_size;
  memset(pendings,0,sizeof(pendings));
  sockfd = socket (AF_INET, SOCK_STREAM, 0);
  if(sockfd == -1) printf("Couldn't create socket.");
  my_addr.sin_family = AF_INET;
  my_addr.sin_port = htons (port);
  my_addr.sin_addr.s_addr = INADDR_ANY;
  bzero (&(my_addr.sin_zero), 8);

  if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof (struct sockaddr)) == -1)
    printf("Stern: Couldn't bind to specified port.");

  sin_size = sizeof(struct sockaddr_in);
  if(listen(sockfd, 25) == -1) printf("Couldn't listen on specified port.");
  //

  printf("Listening for connections on port %d...\n", port);
  while(1) {
    newfd = accept(sockfd, (struct sockaddr *)&remote_addr, &sin_size);
    if(newfd == -1) printf("Couldn't accept connection!");
    pthread_t *thread;
    int status;

    type = header_magic(newfd,&count);
    printf("Connection %d\n",type);
    if(type < 0) {
      if((rv = send(newfd, BAD_MSG, strlen(BAD_MSG), 0)) == -1) 
          warn("Error sending data to client.");
    } else if(type == 0){
            if((rv = send(newfd, BSON_TYPE, strlen(BSON_TYPE), 0)) == -1) 
          warn("Error sending data to client.");
  } else if(type == 1){
            if((rv = send(newfd, JSON_TYPE, strlen(JSON_TYPE), 0)) == -1) 
          warn("Error sending data to client.");
    }
    i=0;
    while(pendings[i].newfd && i < NTHREAD) i++;
    if(i==NTHREAD) {
      if((rv = send(newfd, PORT_MSG, strlen(PORT_MSGE), 0)) == -1) 
          warn("Error sending data to client.");
         close(newfd);
    } else {
    pendings[i].newfd = newfd; 
    pendings[i].remote_addr =(struct sockaddr_in *)&remote_addr;
#ifdef THREAD_TEST
    //status = pthread_create(thread,0,handler, &pendings[i]);
#endif
    printf("Done %d\n",status);
    }
  }
}



// A utility to translate triples
typedef struct { int rowid; char * buffer; char * start; Triple t[]; int byte_count} CallBox;
int add_next_descent(CallBox * parent) {
  int i; 
  CallThing child;
  char * bson_key =parent->t[0]->key
  int bson_type = parent->t[0]->link >> 8;
  if(bson_type== BsonString)
    bson_len = strlen(bson_key);
  else if( bson_type == BsonInt)
      bson_len=4
  else
  printf("Lazy Programmer\n");
   while(parent->rowid < parent->t[0]->pointer) {
      child = *parent;
      child->start = parent->buffer;
      add_next_descent(&child);
      parent->rowid = child->rowid;
   }
   parent->rowid++;
   strncpy(parent->start,INT(bson_len);
   parent->start += 4;
   parent->start++ = bson_type;
   strncpy(parent->start,bson_key,bson_len);
  }
}
int net_start() {

    memset(pendings,0,sizeof(pendings));
    pendings[0].newfd = 1; 
    pendings[i].remote_addr =0;
       pthread_t *thread;
      int status;
    status = pthread_create(thread,0,console_loop,&pendings[0]);
    if(status == -1) {
      printf("Error threading");
      exit(1);
      net_service();
    }

    net_service();
  }
#endif
#ifdef STAND_ALONE
int net_start() {return 0;}
#endif
