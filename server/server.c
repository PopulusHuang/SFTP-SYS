#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include "srv_parse.h"
#include "ssl_wrap.h"
#include "sock_wrap.h"
#include "myepoll.h"
#include "sockssl.h"
#include "read_cfg.h"
#define DB_PATH	  "../database/sftp_user.db"
#define	CERTIFICATE_DIR   "certificate/"
#define CACERT	"cacert.pem"
#define PRIVKEY	"privkey.pem"
#define MAXBUF 1024
#define MAX_EVENT 100
#define CONFIG_SIZE 5

#define CFG_LOCALADDR 0
#define CFG_IP  1
#define CFG_PORT 2 
#define CFG_LISTEN 3 
int LISTEN_NUM = 20;
char KEY[CONFIG_SIZE][CFG_KEY_SIZE] = {"INADDR_ANY","IP","PORT","LISTEN"};
char VALUE[CONFIG_SIZE][CFG_VALUE_SIZE];
void server_config()
{
	FILE *cfg_file = NULL;	
	sftpcfg_t config;
	char key[CFG_KEY_SIZE];
	char value[CFG_VALUE_SIZE];

	memset(key,0,sizeof(key));
	memset(value,0,sizeof(value));

	cfg_file = fopen("sftp-sys.conf","r");
	if(cfg_file == NULL)
	{
		fprintf(stderr,"Open configure file error!\n");		
		exit(1);
	}
	config.comment_char = '#';
	config.sep_char = '=';
	config.key = key;
	config.value = value;
	int i;
	for (i = 0; i < (CONFIG_SIZE - 1) ; i++) {
		strcpy(key,KEY[i]);
		cfg_readValue(cfg_file,config);
		strcpy(VALUE[i],config.value);
		memset(value,0,sizeof(value));
	}
	LISTEN_NUM = atoi(VALUE[CFG_LISTEN]);
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
  servaddr.sin_port = htons(atoi(VALUE[CFG_PORT]));
#if 1
  if (strcmp(VALUE[CFG_LOCALADDR],"off") == 0)
    servaddr.sin_addr.s_addr = inet_addr(VALUE[CFG_IP]);
  else
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
#endif
	Bind(sockfd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr));
	Listen(sockfd,LISTEN_NUM);
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
	ret = sqlite3_open(DB_PATH,&db);
	if(ret != SQLITE_OK)
	{
		fputs(sqlite3_errmsg(db),stderr);
		fputs("\n",stderr);
		exit(1);
	}
/* config openssl */
  SSL_CTX *ctx; /*SSL Content Text*/
	
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

  ssl_load_cert_priv(ctx,CERTIFICATE_DIR,CACERT,PRIVKEY);
  /*---------- setup a socket monitor-----------------*/
  server_config();
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
	  /* handle all events */
	  for(i = 0;i < all_event_num;i++)
	  {
		  if(events[i].data.fd == sockfd) /*new connection*/
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
  
  ssl_close_pk(ctx, ssl,sockfd);
  sqlite3_close(db);
  SSL_CTX_free(ctx);
  return 0;
}
