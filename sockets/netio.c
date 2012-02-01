/*
* engine netio start up
* puts a process on listen, then sends a process to anage console
* Engine code

*/
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "../src/g_types.h"
#include "../src/machine.h"
#undef NETIO
#ifdef NETIO


#ifdef HAVE_SYS_SENDFILE_H
#include <sys/sendfile.h>
#endif
#include <sys/wait.h>
#include <unistd.h>
#include <sys/mman.h>
#include <dirent.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <pthread.h>
#define METHOD_GET 0
#define METHOD_HEAD 1
#define METHOD_PUT 2
#define METHOD_UNSUPPORTED -1
#define SERVER_NAME "Graph Machine"

/* Globals */
char * default_type = "text/plain";
int generate_index = 0;
int verbose = 0;
int background = 0;
int sockfd = -1;
int print_headers = 0; /* Print headers to screen. */
int loglevel = 0;
int links = 0;


static void help() {
  printf("Usage: graphs [OPTIONS] [DIRECTORY]\n");
  printf("\n");
  printf("Please see https://github.com/Matt-Young/Semantic-Machine/wiki .\n");
}

char * msg404 = "<html><head><title>404 Not Found</title></head><body>\
                <h1>404 Not Found</h1><h2>No get allowed.</h2></body></html>\n";

/* SIGHUP handler */
static void sigcatch(int signal);

/* Prototypes */
static void handle_request(int fd, struct sockaddr_in * remote);
static servable * gen_index();
static void handle_connection(int fd, struct sockaddr_in * remote);
static int get_method(char * req);
static servable * match_request(char * req);
static int safesend(int fd, char * out);
static char * get_mimetype(char * file);
static void crit(char * message);
static void warn(char * message);
static void * smalloc(size_t size);


int main(int argc, char *argv[]) {

  int i, fr, rv;
  char console_string[20];

  /* Parse options */
  if(argc < 2) { help(); exit(0); }

  for(i = 1; i < argc; i++) {
    if(strcmp(argv[i], "-V") == 0) {
      printf("You are using %s.\n", SERVER_NAME);
      exit(0);
    } else if((strcmp(argv[i], "--help") == 0) || (strcmp(argv[i], "-h") == 0)) {
      help();
      exit(0);
    }
  }
  rv = fork();
  if(rv == -1) {
    crit("Error forking");
  } else if(rv > 0) 
    console_loop();
  else
    netio();
}


static void handle_connection(int fd, struct sockaddr_in * remote) {
  // everything should be a put, just scoff the data 
  // just grab the data into a ytiple and call the machine
  handle_request(fd, remote);

  /* Shutdown socket */
  if(shutdown(fd, SHUT_RDWR) == -1) {
    warn("Error shutting down client socket.");
    return;
  }

  if(close(fd) == -1) warn("Error closing client socket.");
}

static void loghit(char * req, char *referrer, char *ua, int code, int size, struct sockaddr_in * remote) {
  char * t = curtime(); char *i;
  if( (i = strchr(referrer, ' ')) == NULL) referrer = "-";
  else referrer = i + 1;
  if( (i = strchr(ua, ' ')) == NULL) ua = "-";
  else ua = i + 1;
  printf("%s - - [%s] - \"%s\" %d %d \"%s\" \"%s\"\n",
    inet_ntoa(remote->sin_addr), t, req, code, size, referrer, ua);
  fflush(stdout);
  if(t[0] != '-') free(t);
}

static void handle_request(int fd, struct sockaddr_in * remote) {
  int flength;
  int method;
  struct stat curstat;
  int rv, c, infd, h = 0;
  char inbuffer[2048];
  char *out;
  char *lastmod;
  char outb[1024];
  char * referrer = "-"; char * ua = ""; char * request = NULL;
  char * header; /* newline terminated header. */
  int content_length = 0;
  servable * file;

  rv = recv(fd, inbuffer, sizeof(inbuffer), 0);
  if(rv == -1) {
    warn("Error receiving request from client.");
    return;
  }

  /** Read headers and request line. */
  for(c = 0; c < rv; c++) {
    if(inbuffer[c] == '\n') {
      inbuffer[c] = '\0';
      if((c > 1) && (inbuffer[c - 1] == '\r')) inbuffer[c-1] = '\0';
      if(h != 0) {
        header = inbuffer + h;
        if(print_headers) printf("%s\n", header);
        if(strncmp(header, "Referer:", 8) == 0) referrer = header;
        if(strncmp(header, "User-Agent:", 11) == 0) ua = header;
      } else {
        request = inbuffer;
        if(print_headers) printf("%s\n", request);
      }

      h = c + 1;
    }
  }

  if(request == NULL) { return; /* TODO: Return error */ }
  if(verbose) printf("REQ: %s\n", request);
  method = get_method(request);

  /* Find file in linked list */
  file = match_request(request);

  if(file == NULL) {
    out = "HTTP/1.0 404 Not Found\r\n";
    if(safesend(fd, out) == -1) return;

    snprintf(outb, sizeof(outb), "Server: %s/%s\r\n",
      SERVER_NAME, VERSION);
    if(safesend(fd, outb) == -1) return;

    if(method = METHOD_GET) {
      out = "Content-Type: text/html; charset=iso-8859-1\r\n";
      if(safesend(fd, out) == -1) return;

      snprintf(outb, sizeof(outb), "Content-Length: %d\r\n",
        strlen(msg404));
      if(safesend(fd, outb) == -1) return;

      // Send error response
      c = strlen(msg404);
      if(safesend(fd, msg404) == -1) return;
    }

    snprintf(outb, sizeof(outb), "\r\n");
    if(safesend(fd, outb) == -1) return;

    if(loglevel) loghit(request, referrer, ua, 404, c, remote);
    return;
  }

  /* Response status line */
  out = "HTTP/1.0 200 OK\r\n";
  if(safesend(fd, out) == -1) return;

  if(file->filename != NULL) {
    /* Get file stats */
    if(lstat(file->fullpath, &curstat) == -1) {
      warn("Error checking file."); return;
    }

    /* Response headers */
    snprintf(outb, sizeof(outb), "Content-Type: %s\r\n", get_mimetype(file->filename));
    if(safesend(fd, outb) == -1) return;

    lastmod = last_modified((int) &curstat.st_mtime);
    if(lastmod != NULL) {
      snprintf(outb, sizeof(outb), "Last-modified: %s\r\n", lastmod);
      if(safesend(fd, outb) == -1) return;
      free(lastmod);
    }

    if(method == METHOD_GET) {
      snprintf(outb, sizeof(outb), "Content-Length: %d\r\n",
        (int)curstat.st_size);
      if(safesend(fd, outb) == -1) return;
    }

    snprintf(outb, sizeof(outb), "Server: %s/%s\r\n\r\n", 
      SERVER_NAME, VERSION);
    if(safesend(fd, outb) == -1) return;

    /* Response content */
    if(method == METHOD_PUT) {
      infd = open(file->fullpath, O_RDONLY);
      if(infd == -1) {
        printf("Couldn't open %s, error %d\n",
          file->fullpath, errno);
        return;
      }
#ifdef HAVE_SYS_SENDFILE_H
      if(sendfile(fd, infd, 0, curstat.st_size) == -1) {
        warn("Error sending response to client."); return;
      }
#else
      while((rv = read(infd, outb, sizeof(outb)) ) != 0) {
        if(write(fd, outb, rv) == -1) {
          warn("Error sending response to client."); return;
        }
      }
#endif
      close(infd);
    }

    if(loglevel) loghit(request, referrer, ua, 200, curstat.st_size, remote);
  } else {
    /* Use our internal file */
    content_length = 0;
    if(method == METHOD_GET) {
      content_length = file->content_length;
    }

    if(file->last_modified != 0) {
      snprintf(outb, sizeof(outb), "Last-Modified: %s\r\n",
        file->last_modified);
      if(safesend(fd, outb) == -1) return;
    }

    snprintf(outb, sizeof(outb), "Content-Type: %s\r\nContent-Length: %d\r\n\r\n",
      file->content_type, content_length);
    if(safesend(fd, outb) == -1) return;

    if(loglevel) loghit(request, referrer, ua, 200, file->content_length, remote);
    if(method == METHOD_GET) {
      if(safesend(fd, file->content) == -1) return;
    }
  }
}

static int get_method(char * req) {
  if(strncasecmp(req, "GET",  3) == 0) { return METHOD_GET; }
  if(strncasecmp(req, "HEAD", 4) == 0) { return METHOD_HEAD; }
  if(strncasecmp(req, "GET",  3) == 0) { return METHOD_PUT; }
  return METHOD_UNSUPPORTED;
}

static servable * gen_index() {
  servable * myi = NULL;
  servable * ptr = files;
  char * content = NULL;

  /* Big ass embedded string. I know, it be ugly. */
  char * st = "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\r\n\r\n<html><head><title>Index of /</title></head><body><h2>Index of /</h2><ul>\n";
  char * en = "</ul><p><i>Powered by <a href=\"http://www.neuro-tech.net/cheetah\">Cheetah</a></i>.</body></html>\n";
  int sz = 0;
  myi = smalloc(sizeof(servable) + 2);
  myi->filename = NULL;
  myi->fullpath = NULL;

  sz = strlen(st) + strlen(en);
  content = smalloc(sz + 2);

  strncpy(content, st, sz + 1);
  strcat(content, en);
  myi->content = content;
  myi->content_length = strlen(content);
  myi->content_type = "text/html";
  myi->last_modified = NULL;

  return myi;
}

static servable * match_request(char * req) {
  servable * rv = NULL;
  char uri[1024]; char * u;
  int c; int in = 0; int ptr = 0;
  if(req->type == json)
  else
  if(req->type == json)
  else 
  error
  return NULL;
}

static int safesend(int fd, char * out) {
  int rv;
  if((rv = send(fd, out, strlen(out), 0)) == -1) {
    warn("Error sending data to client.");
  }

  return rv;
}

static void sigcatch(int signal) {
  if(verbose) printf("Signal caught, exiting.\n");
  if(sockfd != -1) {
    close(sockfd);
    exit(0);
  }
}

static char * check_mimetype(char * file) {
  char * comp;

  for(comp = file + strlen(file); comp > file; comp--) {
    if(comp[0] == '.') {
      comp++;
      if(strcasecmp(comp, "json") == 0) return "text/json";
      if(strcasecmp(comp, "bson") == 0) return "text/bson";
      return default_type;
    }
  }

  return default_type;
}

static void crit(char * message) {
  fprintf(stderr, "%s\n", message);
  exit(1);
}

static void warn(char * message) {
  fprintf(stderr, "%s\n", message);
}
struct { int newfd,sockfd, 
  (struct sockaddr *) remote_addr } pendings[4];  //five pending 
int thread_count=0;
void netio ()  {
  int port = 8000;
  struct sockaddr_in my_addr;
  struct sockaddr_in remote_addr;
  int sin_size;
  int flength;
  char * fullpath = NULL;
  char * dir = NULL;
  DIR * dirpnt;
  struct dirent * curdir = NULL;
  struct stat curstat;
  servable * curfile = NULL;
  servable * lastfile = NULL;
  int newfd;
  int i, fr, rv;

  if(setsid() == -1) crit("Couldn't create SID session.");
  if(signal(SIGCHLD, SIG_IGN) == SIG_ERR) {
    crit("Couldn't initialize signal handlers.");
  }
  if( (close(0) == -1) || (close(1) == -1) || (close(2) == -1)) {
    crit("Couldn't close streams.");
  }
  /* Trap signals */
  if( (signal(SIGTERM, sigcatch) == SIG_ERR) || (signal(SIGINT, sigcatch) == SIG_ERR)) {
    crit("Couldn't setup signal traps."); }

  sockfd = socket (AF_INET, SOCK_STREAM, 0);
  if(sockfd == -1) crit("Couldn't create socket.");
  my_addr.sin_family = AF_INET;
  my_addr.sin_port = htons (port);
  my_addr.sin_addr.s_addr = INADDR_ANY;
  bzero (&(my_addr.sin_zero), 8);

  if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof (struct sockaddr)) == -1)
    crit("Couldn't bind to specified port.");

  sin_size = sizeof(struct sockaddr_in);
  if(listen(sockfd, 25) == -1) crit("Couldn't listen on specified port.");
  //

  if(verbose) printf("Listening for connections on port %d...\n", port);
  while(1) {
    newfd = accept(sockfd, (struct sockaddr *)&remote_addr, &sin_size);
    if(newfd == -1) crit("Couldn't accept connection!");
    pthread_t *thread;
    int status;
    printf("Thread\n");
    i=0;
    while(pendings[i].newfd) i++;
    if(i==4) exit(1);
    pendings[i] = newfd; 
    pendings[i].remote_addr =(struct sockaddr_in *)&remote_addr;
    status = pthread_create(thread,0,netio_loop,newfd, &);
    printf("Thread %d\n",status)
  }
}
#else
int main_engine(int argc, char *argv[]);
int main(int argc, char *argv[]) {
  return main_engine(argc, argv);
}
#endif NETIO


