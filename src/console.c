// Console, set up
#include "config.h"
#include "g_types.h"
#include "names.h"
#include "../src/console.h"
//#define Debug_console
int init_console() { return(0);}
// These are here just to keep the std lib includes in one spot
// and run test sequences

#ifdef WINDOWS
    #include <direct.h>
    #define GetCurrentDir _getcwd
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
 #endif
#include <ctype.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <stdarg.h>
#include <sys/stat.h>
char * G_debug_line(char *,int);

char * G_line(char * line,int n) {return(fgets(line, n, stdin));}

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
void* G_calloc(int size){return  calloc(1,size); }
void G_free(void* p){free(p);}
char* G_strncpy(char* s, const char* ct, int n){return strncpy(s,ct,n);}
char * G_strcpy(char* s, const char* ct){return strcpy(s, ct);}
int G_strcmp(const char* cs, const char* ct){return strcmp( cs, ct);}
int G_strlen(const char* cs){return strlen(cs);}
int G_atoi(const char* s){ return atoi(s);}
void G_memset(void* s, int c, int n) {memset(s,c,n);}
void G_exit() { exit(0);}
int G_ispunct(int c){return ispunct(c);}
void* G_memcpy(void* s, const void* ct, int n) {return memcpy(s, ct, (size_t) n);}
void G_error(char * c,int i) {G_printf("error %d\n",c); G_exit(i);}
int G_isdigit(int c) {return(isdigit(c));}
char * G_gets(char * line) { return gets(line);}
long G_strtol(char *s) {return strtol(s, 0,0);}
void G_debug(void * format){};


// Below are specific console operations for the parser
// Is it a character known to the syntax? '
//#define Line_size 256
char line[Line_size]; 
char * G_InitConsole(Console * console) {
	memset(line,0,Line_size);
	console->size=Line_size;
	console->base=line;
	console->current=line;
	console->count=0;
	printf("\nInit:");
	return console->base;
}
char * G_AddConsole(Console * console,char cin) {
	console->current[0] = cin; 
	console->current++; console->count++;
	return console->current;
}
void G_Test() {
G_printf("%d ",fgetc(stdin));
}

int isin(char c,const char *str) {
	while((*str)  && (*str != c) ) str++;
	return *str;
}
int console_command(Console * console,char command ) {
  char  dir[200],line[200],*name;
  char *ptr;
  struct stat buf;
  FILE * f;int dirlen;
   fgets(line, 100, stdin);
   ptr = strtok(line," ");
   if(ptr[0] == 'q')
     exit(0);
   else if (ptr[0] == 'f') { 
     ptr += strlen(ptr)+1;
    name = strtok(ptr, "\0");
    GetCurrentDir(dir,1024);
    dirlen = strlen(dir);
    dir[dirlen]='/';
    strcpy(dir+dirlen+1,name);
    printf("\n%s\n",dir);
    f =  fopen(dir, "r");

    if(f){
      fstat(_fileno(f), &buf);
      console->count = fread(console->base,1, 100, f);
      printf("Read: %s  %d ",console->base,console->count);
    }
   else {
     printf("f %s\n",name);
     perror("Open error ");
   }
   }
  return 0;
  
}
// get line with a bit of input editing
int G_console(Console * console) { 
	char * ptr,cin,cprev;
	int left,right;
	left = 0; right = 0;
	cin = 0;
  
	ptr = G_InitConsole(console);
  cprev = 0;
	for(;;) {
    cin = fgetc(stdin);
    printf("%x\n",cin);
    if(  (cin == '\n') &&  ( (cprev == '\n') ||  (left == right)) )
        return(console->count);  // two in a row terminate
    else if( (cin == '.') && (left==0))
          return(console_command(console,cin));
    else if(cin == '{') {left++;G_AddConsole(console,cin);}
    else if(cin == '}') {right++;G_AddConsole(console,cin);}
    else if((left > right) && (cin != '\n'))  // if client has an open curly
      G_AddConsole(console,cin);
    cprev = cin;
  } 

}

// Track memory here, this is not c++
int old_filter_count,new_filter_count;
int del_graph_count,new_graph_count;
int del_table_count,new_table_count;
int del_data_count,new_data_count;
int del_thread_count,new_thread_count;
int del_name_count,new_name_count;
void G_graph_counts(){
	printf("Gr: %d %d ",del_graph_count,new_graph_count);
}
int newcount=0;
int oldcount=0;
void G_buff_counts(){
	printf("\nFilt: %d %d ",old_filter_count,new_filter_count);
	printf("Gr: %d %d ",del_graph_count,new_graph_count);
	printf("Tbl: %d %d ",del_table_count,new_table_count);
  printf("Thr: %d %d ",del_thread_count,new_thread_count);
  printf("D: %d %d",del_data_count,new_data_count);
   printf("Names: %d %d\n",del_name_count,new_name_count);
}
#define Debug_console

#ifdef Debug_console

#endif