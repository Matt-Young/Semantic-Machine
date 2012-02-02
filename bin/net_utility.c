#ifdef NETIO_TEST
#define NTHREAD 4  // a simple test
  // Magic header, we reject everything else
#define JSON_TYPE "POST\r\nContent-Type:text/json\n\rContent-Length:"
#define BSON_TYPE "POST\r\nContent-Type:text/bson\n\rContent-Length:"
#define OK_MSG    "HTTP/1.0 200 OK\r\n\r\n"
#define BAD_MSG "HTTP/1.0 404 Not Found\r\n\r\n"
#define PORT_MSG    "HTTP/1.0 989 No Ports\r\n\r\n"
#define MAGIC_SIZE sizeof(JSON_TYPE)
#define HEADER_SIZE (MAGIC_SIZE+10)
 
int header_magic(int newfd,int * count) {
    char inbuffer[HEADER_SIZE];
    int rv; int type;int i;
    type = -1;
    rv = read(newfd, inbuffer, HEADER_SIZE);
   fwrite(inbuffer, 1, MAGIC_SIZE, stdout);
    fwrite("\n", 1, 1, stdout);
    //printf("%d %d",inbuffer,MAGIC_SIZE);
    for(i=MAGIC_SIZE;i<HEADER_SIZE;i++) printf("%2x|",(unsigned char) inbuffer[i]);
      fwrite("\n", 1, 1, stdout);
    if(rv != -1 && rv == HEADER_SIZE) {
     if(!strncmp(inbuffer,JSON_TYPE,MAGIC_SIZE)) type = 1;
     else if(!strncmp(inbuffer,BSON_TYPE,MAGIC_SIZE)) type = 0;
    }
    if(type != -1)
      *count = (int) inbuffer[MAGIC_SIZE];
    return (type);
    }

typedef struct { 
  int newfd;
  void * remote_addr; 
  int count;
  int type;
  const sem_t * sem;
} Pending;
typedef void * (*SocketHandler)(void *);
void * handle_request(void * arg) {
    Triple t;
    Pending *p = (Pending *) arg;
    int fd = p->newfd;
    struct sockaddr_in * remote = p->remote_addr;
    char * data_buff;
    int rv;
    t.key = (char *) calloc(p->count,1);
    rv = recv(p->newfd, t.key, sizeof(p->count), 1);
     message_accepted(p);  // let the connection go away
     t.link = p->type | 0x80;
     t.pointer = p->count;
     triple(&t,0);
     free(t.key);
     delete_thread((void *) 1);
}
void recv_test ()  {
  Pending pendings[NTHREAD];
  int sockfd = -1;
  int port = TEST_PORT;
  struct sockaddr_in my_addr;
  struct sockaddr_in remote_addr;
  int newfd,count,type;
  int i, rv,sin_size;
  memset(pendings,0,sizeof(pendings));
  sockfd = socket (AF_INET, SOCK_STREAM, 0);
  if(sockfd == -1) printf("Couldn't create socket.");
  my_addr.sin_family = AF_INET;
  my_addr.sin_port = htons (port);
  my_addr.sin_addr.s_addr = INADDR_ANY;
  bzero (&(my_addr.sin_zero), 8);

  if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof (struct sockaddr)) == -1)
    printf("Stern: Couldn't bind to specified port.");

  sin_size = sizeof(struct sockaddr_in);
  if(listen(sockfd, 25) == -1) printf("Couldn't listen on specified port.");
  //

  printf("Listening for connections on port %d...\n", port);
  while(1) {
    newfd = accept(sockfd, (struct sockaddr *)&remote_addr, &sin_size);
    if(newfd == -1) printf("Couldn't accept connection!");
    pthread_t *thread;
    int status;

    type = header_magic(newfd,&count);
    printf("Connection %d\n",type);
    if(type < 0) {
      if((rv = send(newfd, BAD_MSG, strlen(BAD_MSG), 0)) == -1) 
          warn("Error sending data to client.");
    } else if(type == 0){
            if((rv = send(newfd, BSON_TYPE, strlen(BSON_TYPE), 0)) == -1) 
          warn("Error sending data to client.");
  } else if(type == 1){
            if((rv = send(newfd, JSON_TYPE, strlen(JSON_TYPE), 0)) == -1) 
          warn("Error sending data to client.");
    }
        i=0;
    while(pendings[i].newfd && i < NTHREAD) i++;
    if(i==NTHREAD) {
      if((rv = send(newfd, PORT_MSG, strlen(PORT_MSG), 0)) == -1) 
          warn("Error sending data to client.");
         close(newfd);
    } else {
    pendings[i].newfd = newfd; 
    pendings[i].sem =  sem_open("TestName", O_CREAT,S_IRUSR|S_IWUSR,0);
    pendings[i].remote_addr =(struct sockaddr_in *)&remote_addr;
    }
#ifdef THREAD_TEST
    status = pthread_create(thread,0,handle_request, &pendings[i]);
#else
    close(newfd);
#endif
    printf("Done %d\n",status);
  }
}
// Little sender
int send_test()
{

    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];

    portno = TEST_PORT;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(TEST_ADDR);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    printf("Please enter the message: ");
    bzero(buffer,256);
#if 0
    fgets(buffer,255,stdin);
#else
    strncpy(buffer,JSON_TYPE,MAGIC_SIZE);
    strcpy(buffer+MAGIC_SIZE,"HelloDolly");
#endif
    n = write(sockfd,buffer,HEADER_SIZE);
    if (n < 0) 
         error("ERROR writing to socket");
    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0) 
         error("ERROR reading from socket");
    printf("%s\n",buffer);
    close(sockfd);

    return 0;
}
#endif