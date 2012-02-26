// Console, set up
#undef BUFFER_TRACKING
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
char* G_strcat(char* s, const char* ct){return strcat(s,ct);}
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
int * console_io_struct(IO_Structure * console);
char * G_AddConsole(IO_Structure * console,char cin) {
	console->empty[0] = cin; 
	console->empty++; console->count++;
	return console->empty;
}

int isin(char c,const char *str) {
	while((*str)  && (*str != c) ) str++;
	return *str;
}
char  DirOrigin[200];
int origin_length;
char * find_origin() {
#ifdef ORIGIN
  strcpy(DirOrigin,ORIGIN);
#else
  GetCurrentDir(origin,1024);  // returns back slashes in windows
#endif
origin_length = strlen(DirOrigin);
  printf("\n%s\n",DirOrigin);
  return DirOrigin;
  }
int   G_file(IO_Structure ** out,char * name) { 
   char  complete_name[400]; char *b;
  struct stat buf;
  FILE * f;
   G_strcpy(complete_name,DirOrigin);
   G_strcat(complete_name,name);
   //strcpy(complete_name,"c:/soft/test.txt");
  f =  fopen(complete_name, "r");

  if(f){
      IO_Structure *w;
      wait_io_struct();
      w = new_IO_Struct();
      *out = w;
    fstat(_fileno(f), &buf);
     w->size = buf.st_size;
    w->buff = (int *) G_malloc(w->size+1);
    b = (char *) w->buff;
    w->count = fread(b,1, w->size, f);
    b[w->count] = 0;
  w->fd = (int) G_stdout();  // default valut
  w->sa_family = AF_FILE;
  w->format = Json_IO;
    printf("\nSize%d\n%s\nCount%d\n",w->size,w->buff,w->count);
  }
  else {
    printf("f %s\n",name);
    perror("Open error ");
  }
  return 0;
}
int file_loop(char * name);
int console_command(char * line,char command ) {
   fgets(line, 100, stdin);
   line = strtok(line," ");
   if(line[0] == 'q')
     exit(0);
   else if (line[0] == 'f') 
     file_loop(line+1);
   else if ((line[0] == 'd')) {}// debug?
  return 0;
   return 0;
}
// get line with a bit of input editing
int G_console(IO_Structure * *out) { 
	char * ptr; char * pstart; char cin,cprev;
	int left,right;
	left = 0; right = 0;
	cin = 0;
	ptr =(char *) malloc(Line_size);
  pstart = ptr;
  cprev = 0;

	for(;;) {
    cin = fgetc(stdin);
    //printf("%x\n",cin);
    if(  (cin == '\n') &&  ( (cprev == '\n') ||  (left == right)) ) {
      IO_Structure *w;
      wait_io_struct();
      w = new_IO_Struct();
      *out = w;
      w->sa_family=AF_CONSOLE;
      w->buff = (int *) pstart;
      w->count = ptr-pstart;
       w->format = Json_IO;
       w->fd = (int) G_stdout();
     return(w->count);  // two in a row terminate
    } else if( (cin == '.') && (left==0) )
          return(console_command(pstart,cin) );
     else if(cin == '{') {left++;*ptr=cin;}
    else if(cin == '}') {right++;*ptr=cin;}
    else if((left > right) && (cin != '\n'))  // if client has an open curly
      *ptr=cin;
    ptr++;
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
#define BUFFER_TRACKING
#ifdef BUFFER_TRACKING
#define free G_free_buff
#define malloc G_new_buff
#endif

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
  if( w->sa_family == AF_TABLE)
       release_table_context( w->buff);
  else mem_delete( w);
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
    free(w->buff);
  w->buff=0;
  return 0;
}

sem_t IO_Struct_mutex;
void init_io_struct() {
    anchor=0;
    sem_init(&IO_Struct_mutex, 1, 1);
  }
int set_ready_graph(void * t) ;
void wait_io_struct() {
    sem_wait(&IO_Struct_mutex);  // wait for lock
    set_ready_graph(0); 
}
void post_io_struct() {
    sem_post(&IO_Struct_mutex);  // wait for lock
}

int port = TEST_PORT;
int engine_init();
int net_start(void *);
int console_loop();  // this, starting  thread
int file_loop(char *);  // this, starting  thread
char * argv[] = {"g","-V","-O","-help"};
int argc = 4;
 
int test_qson();
int main() {
//    int main(int argc, char * argv[]) {
  int i;
  find_origin();
  G_memset((char *) &BC,0,sizeof(BufferCount));
  engine_init();
  G_printf("Main engine\n");
  for(i=1; i < argc;i++) {
    if(!G_strcmp(argv[i], "-V")) {
      G_printf("V: %s.\n",VERSION);
      continue;
    } 
     if(!G_strcmp(argv[i], "-exit")) 
      G_exit();
     if(!G_strcmp(argv[i], "-O")) {
      G_printf("O: %s\n",DirOrigin);
     continue;
    } 
    if( !G_strcmp(argv[i], "-help")  || !G_strcmp(argv[i], "-h") ) {
      G_printf("help: graphs\n");
      G_printf("Please see https://github.com/Matt-Young/Semantic-Machine/wiki .\n");
      continue;
    } 
    if(!G_strcmp(argv[i], "-port") )
    { G_printf("Old port %d ",port); 
    port = G_strtol(argv[i+1]); i++; 
    G_printf("New port %d\n",port);
    continue;}
    if(!G_strcmp(argv[i], "-file") ) {
      G_printf("File %s\n",argv[i+1]);
      file_loop(argv[i+1]); 
    }
  }
    G_printf("Port %d\n",port);
   // print_trios();
    net_start((void *) port);
    G_printf("Main engine\n");
    for(;;)
    console_loop();
  return(0);
}
void print_triple(Triple *t) { 
  G_printf(" %10s %c %4d  ",t->key,t->link,t->pointer);}
// his is a little debgger, and stays ith this file
Handler g_debugger(Triple *t) {
  print_triple(t);
  return 0;
}