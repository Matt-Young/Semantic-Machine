/*
engine netio start up
git push git@github.com:Matt-Young/Semantic-Machine

In the lab, stand alone configuration runs under windows studio on the cheap
setting either threads or netio then you need to run under cygwin or under linux.
the lab configuratio, the threads only and the netio
*/
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
#include "../src/machine.h"
#include "../src/engine.h"
#include "../src/http_hdrs.h"
#define error printf
#define warn printf
#define NTHREAD 16 

/* Globals */
int sockfd = -1;
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

void * handle_data(void * arg) {
	Triple t;
	Pending *p = (Pending *) arg;
	int fd;
	struct sockaddr_in * remote = p->remote_addr;
	int rv;
  printf("handler count %d\n",p->count);
  fd = p->newfd;
	t.key = (char *) calloc(p->count,1);
	rv = read(p->newfd, t.key, p->count);
	if(rv < p->count) {
		if((rv = send(p->newfd, BAD_MSG, strlen(OK_MSG), 0)) == -1) 
			warn("Error sending data to client.");
	}
	else {
		if((rv = send(p->newfd, OK_MSG, strlen(OK_MSG), 0)) == -1) 
			warn("Error sending data to client.");
	close(p->newfd);
  //printf("%s\n",t.key);
	t.link = OperatorBsonIn;
	t.pointer = p->count;
#if DebugNETIO
	printf("Triple\n");
#else
	triple(&t,0);
#endif
  }
  free(t.key);
  p->newfd = 0;
 
}

static void crit(char * message) {
	fprintf(stderr, "%s\n", message);
	exit(1);
}

void * net_service (void * arg)  {
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
		error("Stern: Couldn't bind to specified port.");
	sin_size = sizeof(struct sockaddr_in);
	if(listen(sockfd, 25) == -1) printf("Couldn't listen on specified port.");
  	memset(pendings,0,sizeof(pendings));
	printf("Listening for connections on port %d...\n", port);
	while(1) {
		newfd = accept(sockfd, (struct sockaddr *)&remote_addr, &sin_size);
		if(newfd == -1) printf("Couldn't accept connection!");
		pthread_t *thread;
		int status;
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
				pendings[i].newfd = newfd; 
				pendings[i].count = count;
				pendings[i].remote_addr =(struct sockaddr_in *)&remote_addr;
        printf("Call %d\n",status);
				status = pthread_create(thread,0,handle_data, &pendings[i]);
				printf("Done %d\n",status);
			}
		}
	}
  return 0;
}

int net_start() {
	pthread_t *thread;
  int status;
  status = pthread_create(thread,0,net_service,0);
	if(status == -1) {
		printf("Error threading");
		exit(1);
	}
	return 0;
}

#if 0
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
#endif