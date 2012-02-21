// debug utilities
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../src/include/g_types.h"
#include "../src/include/config.h"
#include "../src/include/debug.h"

char * typeface[] = {
  "{@SystemEcho,test}",
  "{def.ccc,bbb.kkk,hhh.lyu,ggg.aaa}",
  "{a=23. . c=9. . c.f,d=33}",
  "{$SystemEcho.hello}",
  "{ {abc.\"def\".joe:jjj.\"kkk\".lll},anyname:{rdf,may},'you'.klf,{ {named,kkk}.{fgh.lmk} }, jkl }",

  "{a=18,$aaa.22. ,{vvv=40,555=2}.local:SystemExec{\"select * from console;\"}}",
	"{abc,def,ghi}",
	"{@config}",
	""};
#define DLINE 0
static int debug_counter=DLINE;

void debug_json_string(Webaddr * w) {
 w->buff = (int *) malloc(400);
 w->size=400;
 memset((char *) w->buff,0,400);
 strcpy((char *) w->buff,typeface[DLINE]);
}


int test_qson() {
  Webaddr w1,w2;
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
  strcat(w1.addr,"/result");
  w1.sa_family = AF_FILE;
  system_copy_qson(&w1,&w2);  //file to mem
  return 0;
}
void debug_console_file(Webaddr *c) {
  console_file(c,TESTFILE);}


 void debug_enter(Webaddr *c,char *ptr) {
   char temp[200];
   memset(temp,0,200);
   printf("Hello debugger\n");
   strcat(strcat(temp,TESTDIR),TESTFILE);
   console_file(c,temp);
 }

