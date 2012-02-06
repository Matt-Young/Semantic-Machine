// Console, set up
#include "g_types.h"
#include "../src/console.h"
//#define Debug_console
int init_console() { return(0);}
// These are here just to keep the std lib includes in one spot
// and run test sequences
#include <ctype.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <stdarg.h>
char * G_debug_line(char *,int);

char * G_line(char * line,int n) {
#ifdef Debug_console
	return (G_debug_line(line,n));
#else
	return(fgets(line, n, stdin));
#endif
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
void* G_memcpy(void* s, const void* ct, int n) {return memcpy(s, ct, (size_t) n);}
void G_error(char * c,int i) {G_printf("error %d\n",c); G_exit(i);}
int G_isdigit(int c) {return(isdigit(c));}
char * G_gets(char * line) { return gets(line);}
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
	printf("Init \n");
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
// get line with a bit of input editing
int G_console(Console * console) { 
	char * ptr,cin,cprev;
	int left,right;
	FILE f;
	left = 0; right = 0;
	cin = 0;
  cprev = 0;
	ptr = G_InitConsole(console);
	f = *stdin;
	for(;;) {
    cin = fgetc(stdin);
    //printf(" %x %x |",cin,cprev);
    if( ((cin == '\n') && (cprev == '\n')) || ((cin == '\n') && (left == right)))
      return(console->count);  // two in a row terminate
    else if(cin == '{') {left++;G_AddConsole(console,cin);}
    else if(cin == '}') {right++;G_AddConsole(console,cin);}
    else if((left > right) && (cin != '\n'))  // if client has an open curly
      G_AddConsole(console,cin);
    cprev = cin;
  } 

}


// Track memory here, this is not c++
int old_filter_count;
int new_filter_count;
int del_graph_count,new_graph_count;
int del_table_count,new_table_count;
void G_buff_counts(){
	printf("F: %d %d\n",old_filter_count,new_filter_count);
	printf("G: %d %d\n",del_graph_count,new_graph_count);
	printf("T: %d %d\n",del_table_count,new_table_count);
}
#define Debug_console

#ifdef Debug_console
const char * typeface[] = {
	"{abc.def,{fgh.lmk},jkl}",
	"{local:SystemEcho}",
	"{abc,def,ghi}",
	"{@config}",
	""};
#define DLINE 3
static int debug_counter=DLINE;
// Here is some console test and debug
char * G_debug_line(char * line,int n) {
	if(debug_counter==DLINE)
	G_strncpy(line,typeface[DLINE],n);
	else line[0]=0;
	debug_counter++;
	return line;
	}
#endif