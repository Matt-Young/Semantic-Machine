// Console, set up
#include "console.h"
int init_console() { return(0);}
// These are here just to keep the std lib includes in one spot
// and run test sequences
#include <ctype.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <stdarg.h>
int debug_counter = 0;
#define test_0 "{abc.def.ghi}"
#define test_1 "{abc,def,ghi}"
#define test_2 "{Test2$SystemDecode}"
#define test_3 ""
#define test_4 "{TestAttribute$SystemDecode}"
char * G_line(char * line) {
	char * prompt="Hello\n";
	memset(line,0,200);
	fgets(line, 100, stdin);
#ifdef DEBUG
	G_strcpy(line,test_4);
	printf("%s\n",test_4);
#endif
	return line;
  }

void G_printf( const char *fmt, ...) 
{    
 va_list argptr;
 va_start(argptr,fmt);
 vprintf(fmt, argptr);
 va_end(argptr);
}
void G_sprintf(char *s, const char *fmt, ...) 
{  
 va_list argptr;
 va_start(argptr,fmt);
 vsprintf(s,fmt, argptr);
 va_end(argptr);
}
void* G_malloc(int size){return malloc(size);}
void* G_calloc(int size){void * p=  calloc(1,size);memset(p,0,size); 
return p;}
void G_free(void* p){free(p);}
char* G_strncpy(char* s, const char* ct, int n){return strncpy(s,ct,n);}
char * G_strcpy(char* s, const char* ct){return strcpy(s, ct);}
int G_strcmp(const char* cs, const char* ct){return strcmp( cs, ct);}
int G_strlen(const char* cs){return strlen(cs);}
int G_atoi(const char* s){ return atoi(s);}
void G_memset(void* s, int c, int n) {memset(s,c,n);}
void G_exit() { exit(0);}
int G_ispunct(int c){return ispunct(c);}

void G_error(char * c,int i) {G_printf("error %d\n",c); G_exit(i);}

char * G_gets(char * line) { return gets(line);}
void G_debug(void * format) {}
#undef DEBUG
