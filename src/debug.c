// debug utilities
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../src/include/g_types.h"
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

int debug_json_string(Webaddr * w) {
 w->buff = (int *) malloc(400);
 w->size=400;
 memset((char *) w->buff,0,400);
 strcpy((char *) w->buff,typeface[DLINE]);
  return EV_Ok;
}