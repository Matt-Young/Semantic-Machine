// Console, set up
#include "../src/include/config.h"
#include "../src/include/g_types.h"
#include "../src/include/names.h"
#include "../src/include/console.h"
#include "../src/include/debug.h"
//#define Debug_console

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
#include "../socketx/socket_x.h"
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
void * G_stdout() {return stdout;}
void* G_malloc(int size){return malloc(size);}
void* G_calloc(int size){return  calloc(1,size); }
void G_free(void* p){free(p);}
void* G_new_buff(int size){BC.new_data_count++;return malloc(size);}
void G_free_buff(void* p){BC.del_data_count++;free(p);}
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
//int line[Line_size]; 
int * G_InitConsole(IO_Structure * console);
char * G_AddConsole(IO_Structure * console,char cin) {
	console->empty[0] = cin; 
	console->empty++; console->count++;
	return console->empty;
}

int isin(char c,const char *str) {
	while((*str)  && (*str != c) ) str++;
	return *str;
}
void   console_file(IO_Structure * console,char * ptr) { 
  char  dir[200],*name;
  struct stat buf;
  FILE * f;int dirlen;
  //ptr += strlen(ptr)+1;
  name = strtok(ptr, "\0");
  GetCurrentDir(dir,1024);
  dirlen = strlen(dir);
  dir[dirlen]='\\';
  strcpy(dir+dirlen+1,name);
  printf("\n%s\n",dir);
  f =  fopen(name, "r");
  if(f){
    fstat(_fileno(f), &buf);
    console->buff = G_malloc(buf.st_size+1);
    console->size = buf.st_size;
    console->count = fread(console->buff,1, console->size, f);
    ptr = (char *) console->buff;// defeating the typecastin error
    ptr[console->size] =0;
    printf("\nSize%d\n%s\nCount%d\n",console->size,console->buff,console->count);
  }
  else {
    printf("f %s\n",name);
    perror("Open error ");
  }
}
int console_command(IO_Structure * console,char command ) {
  char  line[200];
  char *ptr;
   fgets(line, 100, stdin);
   ptr = strtok(line," ");
   if(ptr[0] == 'q')
     exit(0);
   else if (ptr[0] == 'f')
     console_file(console,ptr+1);
   else if ((ptr[0] == 'd')) {
     debug_enter(console,ptr);
   }
  return 0;
}
// get line with a bit of input editing
int G_console(IO_Structure * *out) { 
  IO_Structure  console;
	char * ptr,cin,cprev;
	int left,right;
	left = 0; right = 0;
	cin = 0;
  
	ptr = (char *) G_InitConsole(&console);
  cprev = 0;
	for(;;) {
    cin = fgetc(stdin);
    //printf("%x\n",cin);
    if(  (cin == '\n') &&  ( (cprev == '\n') ||  (left == right)) ) {
      IO_Structure *w;
      wait_io_struct();
      w = new_IO_Struct();
      *out = w;
      console.link = w->link;
      *w = console;
     return(console.count);  // two in a row terminate
    }
    else if( (cin == '.') && (left==0))
          return(console_command(&console,cin));
    else if(cin == '{') {left++;G_AddConsole(&console,cin);}
    else if(cin == '}') {right++;G_AddConsole(&console,cin);}
    else if((left > right) && (cin != '\n'))  // if client has an open curly
      G_AddConsole(&console,cin);
    cprev = cin;
  } 

}

// Track memory here, this is not c++

BufferCount BC;

//int newcount=0;int oldcount=0;
void G_graph_counts(){
	printf("Gr: %d %d ",BC.del_graph_count,BC.new_graph_count);
}

void G_buff_counts(){
	printf("Gr: %d %d ",BC.del_graph_count,BC.new_graph_count);
	printf("Tbl: %d %d ",BC.del_table_count,BC.new_table_count);
  printf("Thr: %d %d ",BC.del_thread_count,BC.new_thread_count);
  printf("D: %d %d",BC.del_data_count,BC.new_data_count);
   printf("Names: %d %d\n",BC.del_name_count,BC.new_name_count);
}
#ifdef BUFFER_TRACKING
#define free G_free_buff
#define malloc G_new_buff
#endif
int * G_InitConsole(IO_Structure * console) {
  console->buff = (int *) malloc(Line_size,1);
  memset(console->buff,0,Line_size);
	console->size=Line_size;
	console->empty= (char *)console->buff;
  console->fill=console->empty;
	console->count=0;
  console->fd = (int) G_stdout();
  console->sa_family = AF_CONSOLE;
  console->format = Json_IO;
  strcpy((char *) console->addr,"console");
	printf("\nInit:");
	return (int *) console->buff;
}
IO_Structure *anchor;
IO_Structure * new_IO_Struct(){
  IO_Structure * w = (IO_Structure *) malloc(sizeof(IO_Structure));
  memset(w,0,sizeof(IO_Structure));
  if(anchor)
    w->link = anchor;
  anchor = w;
  BC.new_web_count++;
  return w;
}
int mem_delete(IO_Structure *w);
void release_table_context(void *);
IO_Structure * del_io_struct(IO_Structure *w){
if(!anchor)
  printf("web link error \n");
if(BC.del_web_count >= BC.new_web_count)
  printf("web count error \n");
if(w->buff ) { 
  if( w->sa_family != AF_TABLE)
    mem_delete( w);
  else
    release_table_context( w->buff);
}
anchor = w->link;
free(w);
BC.del_web_count++;
return anchor;
};
// delete webaddr chain
void  del_io_structs() {
  while(anchor) 
    del_io_struct(anchor);
}

int mem_delete(IO_Structure *w) {
  int rows; int i;
  if(!w->buff)
    return 0;
  if( w->sa_family == AF_MEMORY) {
    Triple *Qson; 
    Qson = (Triple *) &w->buff[2];
    rows = Qson[0].pointer;
    for(i=0;i<rows;i++) 
      free( Qson[i].key);
    free(w->buff);
  }
  else 
    free((int*)w->buff);
  w->buff=0;
  return 0;
}

sem_t IO_Struct_mutex;
void init_io_struct() {
    anchor=0;
    sem_init(&IO_Struct_mutex, 1, 1);
  }
void wait_io_struct() {
    sem_wait(&IO_Struct_mutex);  // wait for lock
}
void post_io_struct() {
    sem_post(&IO_Struct_mutex);  // wait for lock
}