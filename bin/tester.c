// This is a file for learning and getting the logic of linus system calls
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "tester.h"

#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <dirent.h>
#include <signal.h>

#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <pthread.h>
#include "net_utility.c"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include "../src/g_types.h"
int net_test(int argc, char *argv[]);
void * start_routine(void * arg) { 
  printf("Thread %x \n",arg);
  return 0;
}

typedef struct {
  int data[20];
  int sequence_size;
} shared_data;
void main(int argc, char *argv[]){

  printf("Start%s %s \n",argv[0],argv[1]);

  if(argv[1] && !strcmp(argv[1], "-m")) {
    int segment_id; //Shared Memory ID
    shared_data *mem; //Shared Memory Pointer
    printf("Mem\n");
    segment_id = shmget(IPC_PRIVATE, sizeof(shared_data), S_IRUSR|S_IWUSR);

    printf("Seg Id  %d\n",segment_id);
    exit(0);
  } 
  if(argv[1] && !strcmp(argv[1], "-sem")) {
    int segment_id; //Shared Memory ID
    sem_t * sem; //Shared Memory Pointer
    sem = sem_open("TestName", O_CREAT,S_IRUSR|S_IWUSR,0);
    printf("Sem %d %d\n",sem,SEM_FAILED);
    // int sem_post(sem_t *sem);

    printf("Seg Id  %d\n",segment_id);
    exit(0);
  } 

  printf("fh\n");
  if(argv[1] && !strcmp(argv[1], "-send")) {
    printf("Send\n");
    //send_test();
    //printf("Addr  %d\n",segment_id);
    exit(0);
  } 
  if(argv[1] && !strcmp(argv[1], "-recv")) {
    printf("Recv\n");
    net_start();
    //printf("Addr  %d\n",segment_id);
    exit(0);
  } 

  if(argv[1] && !strcmp(argv[1], "-t")) {
    pthread_t thread;
    int status;
    thread = 0;
    printf("Thread %d\n",thread);
    status = pthread_create(&thread,0,start_routine,0);
    printf("Thread %d %d %d\n",status,thread,*thread);
    exit(0);
  }
  if(argv[1] && !strcmp(argv[1], "-f")) {
    int fr;
    fr = fork();
    if(fr == 0) printf("Parent\n");
    else printf("Child\n");
    exit(0);
  }
  exit(0);
}
