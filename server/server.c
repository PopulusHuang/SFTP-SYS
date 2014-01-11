#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include "srv_parse.h"
#include "../share/ssl_wrap.h"
#include "../share/sock_wrap.h"
#include "myepoll.h"
#include "sockssl.h"
#define MAXBUF 1024
#define SERV_PORT 7838
#define MAX_EVENT 100
void arg_init(char **argv,int *lisnum,int *myport)
{
  /* set port */
  if (argv[1])
    *myport = atoi(argv[1]);
  else
  {
    *myport = SERV_PORT;
    argv[2]=argv[3]=NULL;
  }
  /*set listen backlog */
  if (argv[2])
    *lisnum = atoi(argv[2]);
  else
  {
    *lisnum = 2;
    argv[3]=NULL;
  }
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
int clnt_close(SSL *ssl,SOCKSSL *sockssl,int connfd)
{

	close(connfd);
	SSL_shutdown(ssl);
	SSL_free(ssl);
  	sockssl_unbind(sockssl,SOCKSSL_SIZE,connfd);
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
  sftfile_userdir("./User");

  ssl_load_cert_priv(ctx);
  /*---------- setup a socket monitor-----------------*/
  sockfd = socket_init(lisnum,argv);
  int i; 
  int epfd;	/* epoll file describes */
  int all_event_num ;/* event number*/
  int cur_event_num = 1;/* current event number */
  SOCKSSL sockssl[SOCKSSL_SIZE]; /* socket's ssl */
  struct epoll_event ev;
  struct epoll_event events[MAX_EVENT];	 

  epfd = epoll_create(MAX_EVENT);
  Event_add(epfd,sockfd,&ev);
  /* init sockssl array */
  sockssl_init(sockssl,SOCKSSL_SIZE); 
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
			/* bind the socket and ssl */
			sockssl_bind(sockssl,SOCKSSL_SIZE,ssl,new_connfd);
		    /* establish SSL connection */
		    if (SSL_accept(ssl) == -1)
		    {
		      perror("accept");
			  clnt_close(ssl,sockssl,new_connfd);
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
			 sftpack_init(&clnt_pack);
			 n = sockssl_search(sockssl,SOCKSSL_SIZE,connfd);
			 /* receive the package */
			 nread = sftpack_recv(sockssl[n].ssl,&clnt_pack);
			 if(nread > 0)	/* parse client's package */
			 {
			 	ret = parse_clnt_pack(sockssl[n].ssl,&clnt_pack,db);
			 }
			 if(ret == COUT || nread <= 0)	/* client logout*/
			 {
				printf("client logout\n");
			    clnt_close(ssl,sockssl,connfd);
			 	epoll_ctl(epfd,EPOLL_CTL_DEL,connfd,&events[i]);
				cur_event_num--;
			 }
		  }
	  }
  }

  close(sockfd);
  SSL_CTX_free(ctx);
  return 0;
}
