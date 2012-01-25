typedef struct {char * base; char * current; int size;} Console;
void G_exit();
void* G_malloc(int size);
void* G_calloc(int size);
void G_free(void* p);
char* G_strncpy(char* s, const char* ct, int n);
char * G_strcpy(char* s, const char* ct);
void G_sprintf(char* s, const char* format, ...);
void G_printf(const char* format, ...);
int G_strcmp(const char* cs, const char* ct);
int G_strlen(const char* cs);
int G_atoi(const char* s);
void G_memset(void* s, int c, int n);
void* G_memcpy(void* s, const void* ct, int n);
int G_ispunct(int c);
int G_isdigit(int c);
char *  G_gets(char * line);
char * G_line(char * line,int n);
char * G_InitConsole(Console * console,char * line,int size);
char * G_console(Console * console);
void G_debug(void * format);
int G_keyop(Console * console,char * *key,int * op);
//#define Debug_parser
#define Debug_console
//#define Debug_engine