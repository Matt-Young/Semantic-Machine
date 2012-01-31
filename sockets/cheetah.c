/*
 * cheetah.c
 *
 * Copyright (C) 2003 Luke Reeves (luke@neuro-tech.net)
 * http://www.neuro-tech.net/
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 */

#include "config.h"

#ifdef HAVE_SYS_SENDFILE_H
#include <sys/sendfile.h>
#endif
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <dirent.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <time.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>

#define METHOD_GET 0
#define METHOD_HEAD 1
#define METHOD_UNSUPPORTED -1
#define SERVER_NAME "Cheetah"

/* Globals */
char * default_type = "text/plain";
int generate_index = 0;
int verbose = 0;
int background = 0;
int sockfd = -1;
int print_headers = 0; /* Print headers to screen. */
int loglevel = 0;
int links = 0;

/* Linked list of files being shared. This is much slower than simply opening
 * files off disk (and I'm too lazy to write a hashtable), but this is a much
 * more safe and secure way. */
typedef struct FSTRUCT {
	char * filename;
	char * fullpath;
	void * next;

	/* Only for internal files */
	char * content;
	int content_length;
	char * content_type;
	char * last_modified;
} servable;
servable * files = NULL;
servable * index_page = NULL;

static void help() {
	printf("Usage: cheetah [OPTIONS] [DIRECTORY]\n");
	printf("Serves up the files listed in the specified directory using HTTP.\n");
	printf("Note that the list of files is scanned on startup only - to rescan the files\n");
	printf("for additions, you must restart cheetah.\n\n");
	printf("-d, --default-type   default mime-type if file isn't found in the database,\n");
	printf("                       defaults to text/plain\n");
	printf("-b                   background mode (disables console output, and allows\n");
	printf("                       multiple requests to be served simultaneously)\n");
	printf("-g                   generate indices for the root directory when no index.html\n");
	printf("                       is found\n");
	printf("    --headers        print out all client request headers\n");
	printf("-l, --log            log (in combined log format) all requests to standard\n");
	printf("                       output\n");
	printf("-p, --port           port to listen for requests on, defaults to 8000\n");
	printf("-s                   follow symbolic links\n");
	printf("-v                   verbose output\n");
	printf("-V                   print version and exit\n");
	printf("-h, --help           display this message and exit\n");
	printf("\n");
	printf("Please see http://www.neuro-tech.net/cheetah for updates and bug reporting.\n");
}

char * msg404 = "<html><head><title>404 Not Found</title></head><body>\
<h1>404 Not Found</h1><h2>The document requested was not found.</h2></body></html>\n";

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

	/* Parse options */
	if(argc < 2) { help(); exit(0); }

	for(i = 1; i < argc; i++) {
		if(strcmp(argv[i], "-V") == 0) {
			printf("You are using %s.\n", SERVER_NAME);
			exit(0);
		} else if((strcmp(argv[i], "--help") == 0) || (strcmp(argv[i], "-h") == 0)) {
			help();
			exit(0);
		} else if((strcmp(argv[i], "--default-type") == 0) || (strcmp(argv[i], "-d") == 0)) {
			default_type = argv[i+1]; i++;
		} else if((strcmp(argv[i], "--headers") == 0) || (strcmp(argv[i], "-h") == 0)) {
			print_headers = 1;
		} else if((strcmp(argv[i], "-p") == 0) || (strcmp(argv[i], "--port") == 0)) {
			port = atoi(argv[i + 1]); i++;
		} else if((strcmp(argv[i], "-l") == 0) || (strcmp(argv[i], "--log") == 0)) {
			loglevel = 1;
		} else if(strcmp(argv[i], "-v") == 0) { verbose++;
		} else if(strcmp(argv[i], "-g") == 0) { generate_index = 1;
		} else if(strcmp(argv[i], "-s") == 0) { links = 1;
		} else if(strcmp(argv[i], "-b") == 0) { background = 1; }
	}

	dir = argv[argc - 1];
	dirpnt = opendir(dir);
	if(dirpnt == NULL) {
		printf("Invalid directory - %s.\n", dir); help(); exit(1);
	}

	if(background) {
		verbose = 0;
		rv = fork();
		if(rv == -1) {
			crit("Error forking");
		} else if(rv > 0) {
			/* Exit if this is the parent */
			_exit(0);
		}
		if(setsid() == -1) crit("Couldn't create SID session.");
		if(signal(SIGCHLD, SIG_IGN) == SIG_ERR) {
			crit("Couldn't initialize signal handlers.");
		}
		if( (close(0) == -1) || (close(1) == -1) || (close(2) == -1)) {
			crit("Couldn't close streams.");
		}
	}

	while((curdir = readdir(dirpnt))) {
		flength = strlen(curdir->d_name) + strlen(dir) + 4;
		fullpath = (char *)smalloc(flength);
		snprintf(fullpath, flength + 2, "%s/%s", dir, curdir->d_name);
		if(links == 1) {
			rv = lstat(fullpath, &curstat);
		} else {
			rv = stat(fullpath, &curstat);
		}

		if(rv != 0) {
			fprintf(stderr, "Error statting file %s/%s\n", dir, curdir->d_name);
			continue;
		}

		/* Only use this file if it's not a link, directory, etc. */
		if(S_ISREG(curstat.st_mode) && ( curdir->d_name[0] != '.' )) {
			if(files == NULL) {
				curfile = files = (void *)smalloc(sizeof(servable) + 2);
			} else {
				curfile = (void *)smalloc(sizeof(servable) + 2);
			}

			flength = strlen(curdir->d_name);
			curfile->filename = (void *)smalloc(flength + 2);
			strncpy(curfile->filename, curdir->d_name, flength + 1);
			curfile->fullpath = fullpath;
			curfile->next = NULL;
			curfile->content = NULL;

			if(lastfile == NULL) {
				lastfile = curfile;
			} else {
				lastfile->next = curfile;
				lastfile = curfile;
			}

			if(verbose) printf("Parsed %s as %s.\n", fullpath, curfile->filename);
		} else {
			free(fullpath);
		}
	}

	/* Generate index */
	if(generate_index == 1) index_page = gen_index();

#ifdef HAVE_SYS_SENDFILE_H
	if(verbose) printf("Using system's sendfile functionality.\n");
#endif

	/* Trap signals */
	if( (signal(SIGTERM, sigcatch) == SIG_ERR) || (signal(SIGINT, sigcatch) == SIG_ERR)) {
		crit("Couldn't setup signal traps.");
	}

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

	if(verbose) printf("Listening for connections on port %d...\n", port);
	while(1) {
		newfd = accept(sockfd, (struct sockaddr *)&remote_addr, &sin_size);
		if(newfd == -1) crit("Couldn't accept connection!");

		if(verbose) printf("Connected, handling request.\n");

		if(background) {
			fr = fork();
			if(fr != 0) continue;
			handle_connection(newfd, (struct sockaddr_in *)&remote_addr);
			_exit(0);
		}
		handle_connection(newfd, (struct sockaddr_in *)&remote_addr);
	}
}

/* Cygwin doesn't like my time structures. It's on my todo list. */
#ifndef __CYGWIN__
static char * last_modified(time_t * stmtime) {
	struct tm * gmt;
	char * rv = (char *)smalloc(100);
	if((gmt = gmtime(stmtime)) == NULL) crit("gmtime() error.");
	if((strftime(rv, 95, "%a, %d %b %Y %T GMT", gmt)) == 0)
		crit("strftime() error.");
	return rv;
}

static char * curtime() {
	struct tm * gmt;
	char * rv = (char *)smalloc(100);
	time_t t = time(NULL);
	if((gmt = gmtime(&t)) == NULL) crit("gmtime() error.");
	if((strftime(rv, 95, "%d/%b/%Y:%T +0000", gmt)) == 0)
		crit("strftime() error.");
	return rv;
}
#else
static char * last_modified(int mtime) { return NULL; }
static char * curtime() { return "-"; }
#endif

static void handle_connection(int fd, struct sockaddr_in * remote) {
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

		lastmod = last_modified(&curstat.st_mtime);
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
		if(method == METHOD_GET) {
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

	/* Might be slow, but it's only run once. */
	while(ptr != NULL) {
		sz += (strlen(ptr->filename)) * 2;
		sz += 40;
		if(!realloc(content, sz)) { printf("Memory allocation error.\n"); exit(0); }
		strcat(content, "<li><a href=\"");
		strcat(content, ptr->filename);
		strcat(content, "\">");
		strcat(content, ptr->filename);
		strcat(content, "</a></li>\n");
		ptr = ptr->next;
	}

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

	/* Parse filename */
	for(c = 0; c < strlen(req); c++) {
		if(in == 1) uri[ptr++] = req[c];
		if((in == 2) || (ptr > 1000)) break;
		if(req[c] == ' ') in++;
	}
	uri[ptr] = '\0'; u = uri;

	if(strcmp(uri, "/ ") == 0) {
		if(generate_index == 1) { return index_page; }
		else { strncpy(uri, "/index.html", sizeof(uri)); }
	}
	if(uri[0] == '/') u++;
	if(verbose) printf("Checking for \"%s\"\n", u);
	if(files == NULL) { return NULL; }

	rv = files;
	while(1) {
		if(strncmp(u, rv->filename, strlen(rv->filename)) == 0) return rv;
		if(rv->next == NULL) return NULL;
		rv = rv->next;
	}

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

static char * get_mimetype(char * file) {
	char * comp;

	for(comp = file + strlen(file); comp > file; comp--) {
		if(comp[0] == '.') {
			comp++;
			if(strcasecmp(comp, "html") == 0) return "text/html";
			if(strcasecmp(comp, "gif") == 0) return "image/gif";
			if(strcasecmp(comp, "jpg") == 0) return "image/jpeg";
			if(strcasecmp(comp, "png") == 0) return "image/png";
			if(strcasecmp(comp, "css") == 0) return "text/css";
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

static void * smalloc(size_t size) {
	void * rv = malloc(size);
	if(rv == NULL) crit("Memory allocation error.");
	return rv;
}


