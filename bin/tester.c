#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <dirent.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <time.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <pthread.h>
void * start_routine(void * arg) { 
  printf("Thread %x \n",arg);
}

typedef struct {
int data[20];
int sequence_size;
} shared_data;
void main(int argc, char *argv[]){
  pid_t pid;    //process ID
  int segment_id; //Shared Memory ID
  shared_data *mem; //Shared Memory Pointer
  FILE *f;
  FILE F;
  char buff[100];
  const char * query = "select 'hello Sqlite3';";
  const char * cmd = "sqlite3  test.db  < name > result";
  printf("Start\n");
  if(argv[1] && !strcmp(argv[1], "-m")) {
        printf("Mem\n");
    segment_id = shmget(IPC_PRIVATE, sizeof(shared_data), S_IRUSR|S_IWUSR);

    printf("Seg Id  %d\n",segment_id);
    exit(0);
  } 
    else if(argv[1] && !strcmp(argv[1], "-t")) {
      pthread_t *thread;
      int status;
      printf("Thread\n");
  status = pthread_create(thread,0,start_routine,0);
  printf("Thread %d\n",status);
  exit(0);
    }else if(argv[1] && !strcmp(argv[1], "-f")) {
    int fr;
    fr = fork();
    if(fr == 0) printf("Parent\n");
    else printf("Child\n");
      exit(0);
  }
  f = fopen("name", "w+");
  F = *f;

  if(f==0)	exit(1);
  fwrite(query, strlen(query), 1, f);
  fclose(f);
  system(cmd);
  f = fopen("result", "r");
  if(f==0)	exit(2);

  while (fgets(buff, sizeof(buff)-1, f) != NULL) 
    printf("%s", buff);
  fclose(f);
  exit(0);
}
