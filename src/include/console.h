int  G_file(IO_Structure ** console,char * name);
int G_console(IO_Structure ** console);
#define Line_size 256
int  G_isugly(char ch);
void G_exit();
void G_Test();
void * G_stdout();
char* G_strncpy(char* s, const char* ct, int n);
char * G_strcpy(char* s, const char* ct);
void G_sprintf(char* s, const char* format, ...);
void G_printf(const char* format, ...);
int G_strcmp(const char* cs, const char* ct);
char* G_strcat(char* s, const char* ct);
int G_strlen(const char* cs);
long G_strtol(char *s);
int G_atoi(const char* s);
void G_memset(void* s, int c, int n);
void* G_memcpy(void* s, const void* ct, int n);
int G_ispunct(int c);
int G_isdigit(int c);

char *  G_gets(char * line);
char * G_line(char * line,int n);

void G_debug(void * format);

void G_buff_counts();

void G_qsort( void * base, int n, int size);
void * G_bsearch(const void * key,const void* base,int n, int size);
void G_graph_counts();
