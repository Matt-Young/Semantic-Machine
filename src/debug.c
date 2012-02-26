// debug utilities
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../src/include/g_types.h"
#include "../src/include/config.h"
#include "../src/include/debug.h"
#include "../socketx/socket_x.h"

char * typeface[] = {
  "{@SystemExec,select * from square;}",
  "{@SystemConfig,37 .\"select * from square; \" }",
  "{def.ccc,bbb.kkk,hhh.lyu,ggg.aaa}",
  "{a=23. . c=9. . c.f,d=33}",
  "{$SystemEcho.hello}",
  "{ {abc.\"def\".joe:jjj.\"kkk\".lll},anyname:{rdf,may},'you'.klf,{ {named,kkk}.{fgh.lmk} }, jkl }",

  "{a=18,$aaa.22. ,{vvv=40,555=2}.local:SystemExec{\"select * from console;\"}}",
	"{abc,def,ghi}",
	"{@config}",
	""};
#define DLINE 2
static int debug_counter=DLINE;

void debug_json_string(IO_Structure ** w) {
  IO_Structure * x;
  wait_io_struct();
  x  = new_IO_Struct();
  x->buff = (int *) malloc(400);
  x->size=400;
  memset((char *) x->buff,0,400);
  strcpy((char *) x->buff,typeface[DLINE]);
  *w=x;
  x->sa_family=AF_CONSOLE;
  x->count = strlen(x->buff);
  x->format = Json_IO;
  x->fd = (int) G_stdout();
}


int test_qson() {
  IO_Structure w1,w2;
  char filname[40];
  printf("Test qson\n");
  strcpy((char *) w1.addr,"test");
  strcpy((char *) w2.addr,"console");
  w1.sa_family = AF_TABLE;
  w2.sa_family = AF_TABLE;
  system_copy_qson(&w2,&w1);// table to table
  w2.sa_family = AF_MEMORY;
  system_copy_qson(&w1,&w2);// table to mem
  w1.sa_family = AF_CONSOLE;
  system_copy_qson(&w2,&w1); // mem to console
  strcpy((char *) w1.addr,"c:/soft/result");
  w1.sa_family = AF_FILE;
  system_copy_qson(&w2,&w1); // mem to file
  strcpy((char *) w1.addr,"testagain");
  w1.sa_family = AF_TABLE;
  system_copy_qson(&w2,&w1);  //mem to table
  strcpy((char *) w1.addr,TESTDIR);
  strcat((char *)w1.addr,"/result");
  w1.sa_family = AF_FILE;
  system_copy_qson(&w1,&w2);  //file to mem
  return 0;
}


void look_buff(void * buff) {
  char ch[200];char *ptr; int i;
     ptr = (char *) buff;
    for(i=0; i <200;i++) ch[i]= ptr[i];
}
#include <sys/stat.h>
#include  <fcntl.h> 

  sem_t *ab, ac, *ad, *bf, *ce, *de, *ef;
void try_semaphoe() {
    sem_wait(&ac);  // wait for lock
    return;
}
void post_semaphoe() {
    sem_post(&ac);  // wait for lock
    return;
}