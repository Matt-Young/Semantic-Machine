// Console, set up
#include "console.h"
#define Debug_console
int init_console() { return(0);}
// These are here just to keep the std lib includes in one spot
// and run test sequences
#include <ctype.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <stdarg.h>
#ifdef Debug_console
int debug_counter = 0;
#define test_0 "{abc.def.ghi}"
#define test_1 "{abc,def,ghi}"
#define test_2 "{abc}"
#define test_3 "SystemConfig$local , 'a d'{ and}"
#define test_4 "{TestAttribute$Testing}"
#endif
char * G_line(char * line,int n) {
	memset(line,0,n);

#ifdef Debug_console
	debug_counter++; debug_counter &= 0x01;
	if(debug_counter != 0) 
		G_strncpy(line,test_2,n);
	else
		debug_counter=0;
	return line;
	//} else
#else
	fgets(line, n, stdin);
#endif
	return line;
  }
char * G_console(Console * console) { return G_line(console->base,console->size);}
char * G_InitConsole(Console * console,char * line,int size) {
  console->size=size;
  console->base=line;
  console->current=line;
  return console->base;
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
void* G_memcpy(void* s, const void* ct, size_t n) {return memcpy(s, ct, (size_t) n);}
void G_error(char * c,int i) {G_printf("error %d\n",c); G_exit(i);}
int G_isdigit(int c) {return(isdigit(c));};
char * G_gets(char * line) { return gets(line);}
void G_debug(void * format){};
// Is it a character known to the syntax? '
const char  *uglies = "'._,{}$!"; // minuse the quote
char * null_key = "_";
enum {QuoteSyntax,DotSyntax,NullSyntax,CommaSyntax};
char  isugly(char ch) { 
	const char * ptr = uglies;
	while((*ptr) && ((*ptr) != ch)) ptr++;
		return *ptr;}
typedef char * CharPointer;
// Front end key word from text and json operators
int G_keyop(Console * console,CharPointer *key,int * op) {
	enum { quote = 1,numeric =2};
      int i;
	  CharPointer ptr= console->current;
	  int start= (int) console->current;
	  int events;
	  if(*ptr == 0) {
		  	console->current = console->base;
			ptr = console->current;
			G_console(console);  // get a line of data
			if(console->base[0] == 0)
				return -1;
	  }
	  i = 0; events=0;
	  while(isspace(*ptr)) ptr++;
	  if(G_isdigit(*ptr) )
		  events += numeric;
	  if(*ptr == uglies[QuoteSyntax]) {// Quote char?
		  ptr++;
		  *key=ptr;
		  if(*ptr  != uglies[QuoteSyntax]) {
			  events += quote;
		  ptr++;
		  }
	  } else *key=ptr;
	  while(1) {
		if(isugly(*ptr) || (*ptr == 0) ) {
			if((events & numeric) && *ptr == uglies[DotSyntax])
				ptr++;
			else if(!(events & quote) && (*ptr == uglies[QuoteSyntax]) ) {
				*ptr = 0;ptr++; 
			} else { char * tmp;
				console->current = ptr+1;
				tmp = ptr; while((tmp != console->base) && isspace(*(tmp-1))) tmp--;
				if(isspace(*tmp)) *tmp = 0;
				break;
			}	
		  }else {
			ptr++;
		  	if(ptr >= &console->base[console->size])
				return -1;
			}
		}
	  	if((*ptr == 0) || !isugly(*ptr) )
			*op = uglies[NullSyntax];
		else
			*op = *ptr;
		i =   (int) console->current;
		i -= start ; // char count
		*ptr = 0;
		if(!(*key[0])) 
			*key = null_key;  // valid null key
		return i;
	}

