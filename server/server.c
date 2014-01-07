#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include "srv_parse.h"
#include "myepoll.h"
#define MAXBUF 1024
#define SERV_PORT 7838
#define MAX_EVENT 100
void Getcwd(char *pwd,int size)
{
	getcwd(pwd,size);
  	if(strlen(pwd)==1)
	{
		pwd[0]='\0';
	}
}
void arg_init(char **argv,int *lisnum,int *myport)
{
  if (argv[1])
    *myport = atoi(argv[1]);
  else
  {
    *myport = SERV_PORT;
    argv[2]=argv[3]=NULL;
  }

  if (argv[2])
    *lisnum = atoi(argv[2]);
  else
  {
    *lisnum = 2;
    argv[3]=NULL;
  }
}
/* create the receive directly */
void create_recvdir()
{
  char *fdir="client_file";		/*receive file's path*/
  mode_t mode;
  mode=0755;
  /*Is the directly exist*/
  if(access(fdir,F_OK) != 0) 
  {
  	mkdir(fdir,mode);
  }
}
/* receive file name from client,and create an empty file with it*/
int create_recvfile(SSL *ssl,char *new_fileName,char *buf)
{
	int n = 0;
	int fd;
	n = strlen(new_fileName);
    bzero(buf, MAXBUF + 1);
    bzero(new_fileName+n, n);
    n = SSL_read(ssl, buf, MAXBUF);	/* read the file name from client */
    if(n == 0)
      printf("Receive File Name '%s'Complete !\n",buf);
    else if(n < 0)
      printf("Failure to receive message ! Error code is %d，Error messages are '%s'\n", errno, strerror(errno));
    if((fd = open(strcat(new_fileName,buf),O_CREAT | O_TRUNC | O_RDWR,0666))<0)
    {
      perror("open:");
	  return -1;
    }
	printf("open file:%s succeed\n",new_fileName);
	return fd;
}
int socket_init(int lisnum,char **argv)
{
  int sockfd;
  struct sockaddr_in servaddr;
  sockfd = Socket(PF_INET, SOCK_STREAM, 0);
  int opt = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = PF_INET;
  servaddr.sin_port = htons(SERV_PORT);
  if (argv[3])
    servaddr.sin_addr.s_addr = inet_addr(argv[3]);
  else
    servaddr.sin_addr.s_addr = INADDR_ANY;
	Bind(sockfd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr));
	Listen(sockfd, lisnum);
  return sockfd;
}
/* Load the user's digital certificate that 
 * is used to send to the client. 
 * A certificate containing a public key*/
void ssl_load_cert_priv(SSL_CTX *ctx)
{
  char certpwd[100];
  char privpwd[100];
  char *certificate;
  char *privkey;
  Getcwd(certpwd,100);
  certificate=strcat(certpwd,"/cacert.pem");
  Getcwd(privpwd,100);
  privkey=strcat(privpwd,"/privkey.pem");
  ssl_load_pk(ctx,certificate,privkey);

}
int main(int argc, char **argv)
{
  int sockfd, connfd, fd;
  int ret = 0;
  socklen_t len;
  struct sockaddr_in my_addr, their_addr;
  unsigned int myport, lisnum; /*lisnum-Max listen number*/

 /* open database */ 
  sqlite3 *db;
	ret = sqlite3_open("../database/sft_user.db",&db);
	if(ret != SQLITE_OK)
	{
		fputs(sqlite3_errmsg(db),stderr);
		fputs("\n",stderr);
		exit(1);
	}
/* config openssl */
  SSL_CTX *ctx; /*SSL Content Text*/
	
  arg_init(argv,&lisnum,&myport);
#if 1
  SSL_library_init();
 
  OpenSSL_add_all_algorithms();
  /* load all SSL error message */
  SSL_load_error_strings();
  /* With SSL V2 and V3 compliant way to generate a SSL_CTX,
   * You can also use SSLv2_server_method() or SSLv3_server_method() 
   * alone represents V2 or V3 standard*/
  ctx = SSL_CTX_new(SSLv23_server_method());
  if (ctx == NULL)
  {
    ERR_print_errors_fp(stdout);
    exit(1);
  }
#endif
  puts("ssl init");
  create_recvdir();

  ssl_load_cert_priv(ctx);
  /*---------- setup a socket monitor-----------------*/
  sockfd = socket_init(lisnum,argv);
  int i; 
  int epfd;	/* epoll file describes */
  int all_event_num ;/* event number*/
  int cur_event_num = 10;/* current event number */
  SOCKSSL sockssl[SOCKSSL_SIZE]; /* socket's ssl */
  struct epoll_event ev;
  struct epoll_event events[MAX_EVENT];	 
  char data[DATA_SIZE];

  bzero(data,sizeof(data));
  epfd = epoll_create(MAX_EVENT);
  Event_add(epfd,sockfd,&ev);
  /* init sockssl array */
  init_sockssl(sockssl,SOCKSSL_SIZE); 
  /* Accept client connection */
  len = sizeof(struct sockaddr);
  SSL *ssl;
  SFT_PACK clnt_pack;
  sftpack_init(&clnt_pack);
  while (1)
  {
	  /* wait for events */
	  all_event_num = Epoll_wait(epfd,events,cur_event_num,-1);
	  /* handl all events */
	  for(i = 0;i < all_event_num;i++)
	  {
		  if(events[i].data.fd == sockfd)
		  {
	         int new_connfd = Accept(sockfd, (struct sockaddr *) &their_addr, &len);
             printf("server: got connection from %s, port %d, socket %d\n", 
			 inet_ntoa(their_addr.sin_addr), 
			 ntohs(their_addr.sin_port), 
			 new_connfd);
    		ssl = SSL_new(ctx);
		    /* insert user's socket to SSL */
		    SSL_set_fd(ssl, new_connfd);
			add_sockssl(sockssl,SOCKSSL_SIZE,ssl,new_connfd);
		    /* establish SSL connection */
		    if (SSL_accept(ssl) == -1)
		    {
		      perror("accept");
    		  SSL_shutdown(ssl);  /* close ssl connection */
   			  SSL_free(ssl);		/* free ssl */
		      close(new_connfd);
			  remove_sockssl(sockssl,SOCKSSL_SIZE,new_connfd);
			  continue;
		    }
			 setnonblocking(new_connfd);
			 Event_add(epfd,new_connfd,&ev);
			 cur_event_num++;
		  }
		  else 
		  {
			int n,nread;
			 connfd = events[i].data.fd; 
			 n = search_sockssl(sockssl,SOCKSSL_SIZE,connfd);

			 //nread = SSL_read_pk(sockssl[n].ssl,data,DATA_SIZE-1);
			 nread = sftpack_recv(sockssl[n].ssl,&clnt_pack);
			 if(nread > 0)
			 {
			 	ret = parse_clnt_pack(sockssl[n].ssl,&clnt_pack,db);
			 }
			 sftpack_init(&clnt_pack);

			  if((nread <= 0)||(ret ==COUT))
			  {
#if 1
				printf("client logout\n");
				close(connfd);
				SSL_shutdown(sockssl[n].ssl);
				SSL_free(sockssl[n].ssl);
				/* remove the sockssl */
				sockssl[n].ssl = NULL;
				sockssl[n].sockfd = INVAILD;
			 	epoll_ctl(epfd,EPOLL_CTL_DEL,connfd,&events[i]);
				cur_event_num--;
#endif
			  }
		  }
    //close(connfd);		/* close client socket */
	  }
  }

  close(sockfd);
  SSL_CTX_free(ctx);
  return 0;
}
