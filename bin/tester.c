// This is a file for learning and getting the logic of linus system calls


#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "tester.h"
#ifdef NETIO_TEST
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <dirent.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <pthread.h>
#include "net_utility.c"
#endif
#ifdef SHMEM_TEST
#include <sys/ipc.h>
#include <sys/shm.h>
#endif

int net_test(int argc, char *argv[]);
void * start_routine(void * arg) { 
  printf("Thread %x \n",arg);
  return 0;
}
void  error(char * er) { printf("%s \n",er);}
typedef struct {
int data[20];
int sequence_size;
} shared_data;
void main(int argc, char *argv[]){

  printf("Start%s %s \n",argv[0],argv[1]);
#ifdef SHMEM_TEST
  if(argv[1] && !strcmp(argv[1], "-m")) {
      int segment_id; //Shared Memory ID
      shared_data *mem; //Shared Memory Pointer
        printf("Mem\n");
    segment_id = shmget(IPC_PRIVATE, sizeof(shared_data), S_IRUSR|S_IWUSR);

    printf("Seg Id  %d\n",segment_id);
    exit(0);
  } 
#endif
#ifdef NETIO_TEST
  printf("fh\n");
   if(argv[1] && !strcmp(argv[1], "-send")) {
        printf("Send\n");
    send_test();
    //printf("Addr  %d\n",segment_id);
    exit(0);
  } 
  if(argv[1] && !strcmp(argv[1], "-recv")) {
        printf("Recv\n");
    recv_test();
    //printf("Addr  %d\n",segment_id);
    exit(0);
  } 
#endif
#ifdef THREAD_TEST
    if(argv[1] && !strcmp(argv[1], "-t")) {
      pthread_t *thread;
      int status;
      printf("Thread\n");
  status = pthread_create(thread,0,start_routine,0);
  printf("Thread %d\n",status);
  exit(0);
    }
#endif
#ifdef FORJ_TEST
      if(argv[1] && !strcmp(argv[1], "-f")) {
    int fr;
    fr = fork();
    if(fr == 0) printf("Parent\n");
    else printf("Child\n");
      exit(0);
  }
#endif
  exit(0);
}
