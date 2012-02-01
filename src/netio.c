/*
engine netio start up
git push git@github.com:Matt-Young/Semantic-Machine

I set up three configurations with defines,
the lab configuratio, the threads oly and the netio
*/
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "../src/g_types.h"
#include "../src/machine.h"
#ifdef SERVER_NAME
#else
#define SERVER_NAME "Graph Machine"
#endif
#define NETIO 

#ifdef STANDALONE
#undef THREADS
#undef NETIO
#else
#ifdef NETIO
#define THREADS
#endif
#endif

#ifdef THREADS
#include <pthread.h>
#endif
// contexts for each thread
#ifdef NETIO
#define NTHREAD 16 
#endif
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
static void commands_and_init(int argc, char *argv[]) {
  int i;
  for(i = 1; i < argc; i++) {
    if(strcmp(argv[i], "-V") == 0) {
      printf("You are using %s.\n", SERVER_NAME);
      exit(0);
    } else if((strcmp(argv[i], "--help") == 0) || (strcmp(argv[i], "-h") == 0)) {
      printf("Usage: graphs [OPTIONS]\n");
      printf("\n");
      printf("Please see https://github.com/Matt-Young/Semantic-Machine/wiki .\n");
      exit(0);
    } else
      printf("Option? %s\n",argv[i]);
  }
    engine_init();
    memset(pendings,0,sizeof(pendings));
    pendings[0].newfd = 1; 
    pendings[i].remote_addr =0;
  }
#ifdef NETIO
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
#define BAD_MSG "HTTP/1.0 404 Not Found\r\n\r\n"
#define MAGIC_SIZE sizeof(JSON_TYPE)
#define HEADER_SIZE (MAGIC_SIZE+10)
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
    char inbuffer[HEADER_SIZE];
    rv = recv(fd, inbuffer, sizeof(inbuffer), HEADER_SIZE);
    if(rv == -1 || rv != HEADER_SIZE)
      message_rejected(p);
    /* find the blank line then go on*/
     if(!strncmp(inbuffer,JSON_TYPE,MAGIC_SIZE)) p->type = 1;
     else if(!strncmp(inbuffer,BSON_TYPE,MAGIC_SIZE)) p->type = 0;
     else 
       message_rejected(p);
     p->count = atoi(&inbuffer[MAGIC_SIZE]); 
     rv = recv(fd, inbuffer, sizeof(inbuffer), HEADER_SIZE);
     if(rv == -1 || rv != p->count)
      message_rejected(p);
     else
       message_accepted(p);  // let the connection go away
     t.key = (char *) calloc(p->count,1);
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
void net_service ()  {
  int sockfd = -1;
  int port = 8000;
  struct sockaddr_in my_addr;
  struct sockaddr_in remote_addr;
  int newfd;
  int i, rv,sin_size;

  sockfd = socket (AF_INET, SOCK_STREAM, 0);
  if(sockfd == -1) crit("Couldn't create socket.");
  my_addr.sin_family = AF_INET;
  my_addr.sin_port = htons (port);
  my_addr.sin_addr.s_addr = INADDR_ANY;
  bzero (&(my_addr.sin_zero), 8);

  if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof (struct sockaddr)) == -1)
    crit("Couldn't bind to specified port.");

  sin_size = sizeof(struct sockaddr_in);
  if(listen(sockfd, 25) == -1) crit("Couldn't listen on specified port.");
  //

  printf("Listening for connections on port %d...\n", port);
  while(1) {
    newfd = accept(sockfd, (struct sockaddr *)&remote_addr, &sin_size);
    if(newfd == -1) crit("Couldn't accept connection!");
    pthread_t *thread;
    int status;
    printf("Thread\n");
    i=0;
    while(pendings[i].newfd && i < NTHREAD) i++;
    if(i==NTHREAD) exit(1);
    pendings[i].newfd = newfd; 
    pendings[i].remote_addr =(struct sockaddr_in *)&remote_addr;
    status = pthread_create(thread,0,handle_request, &pendings[i]);
    printf("Thread %d\n",status);
  }
}
#endif
int main(int argc, char *argv[]) {
commands_and_init(argc,argv);
#ifdef THREADS
       pthread_t *thread;
      int status;
    status = pthread_create(thread,0,console_loop,&pendings[0]);
    if(status == -1) {
      printf("Error threading");
      exit(1);
    }
#ifdef NETIO
    net_service();
#endif
#else
#ifdef STANDALONE
    return main_engine(argc, argv);
#endif
#endif
  }



