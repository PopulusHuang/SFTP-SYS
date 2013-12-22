#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#if 0
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#endif
#include "../../share/ssl_wrap.h"
#include "../../share/sock_wrap.h"
#define MAXBUF 1024
#define SERV_PORT 7838
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
void create_recvdir(char *fdir)
{
  mode_t mode;
  mode=0755;
  /*Is the directly exist*/
  if(access(fdir,F_OK) != 0) 
  {
  	mkdir(fdir,mode);
  }
}
int create_recvfile(SSL *ssl,char *new_fileName,char *buf)
{
	int n = 0;
	int fd;
	n = strlen(new_fileName);
    bzero(buf, MAXBUF + 1);
    bzero(new_fileName+n, n);
    n = SSL_read(ssl, buf, MAXBUF);	/* read the file name */
    if(n == 0)
      printf("Receive File Name '%s'Complete !\n",buf);
    else if(n < 0)
      printf("Failure to receive message ! Error code is %d，Error messages are '%s'\n", errno, strerror(errno));
    if((fd = open(strcat(new_fileName,buf),O_CREAT | O_TRUNC | O_RDWR,0666))<0)
    {
      perror("open:");
      exit(1);
    }
	printf("open file:%s succeed\n",new_fileName);
	return fd;
}
void write_data(SSL *ssl,char *buf,int fd)
{
	int len;
    while(1)
    {
      bzero(buf, MAXBUF + 1);
      len = SSL_read(ssl, buf, MAXBUF); /* read data from client */
      if(len == 0)
      {
        printf("Receive Complete !\n");
        break;
      }
      else if(len < 0)
      {
        printf("Failure to receive message ! Error code is %d，Error messages are '%s'\n", errno, strerror(errno));
        exit(1);
      }
      if(write(fd,buf,len)<0)	/* write data to server */
      {
        perror("write:");
        exit(1);
      }
    }
}
int socket_init(int lisnum,char **argv)
{
  int sockfd;
  struct sockaddr_in servaddr;
  sockfd = Socket(PF_INET, SOCK_STREAM, 0);
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
  int sockfd, new_fd, fd;
  socklen_t len;
  struct sockaddr_in my_addr, their_addr;
  unsigned int myport, lisnum; /*lisnum-Max listen number*/
  char buf[MAXBUF + 1]; /*add one space to save '\0'*/
  char new_fileName[50]="client_file/";
  char *fdir="client_file";		/*receive file's path*/
  SSL_CTX *ctx; /*SSL Content Text*/
  arg_init(argv,&lisnum,&myport);
#if 1
  SSL_library_init();
 
  OpenSSL_add_all_algorithms();
  /* 载入所有 SSL 错误消息 */
  SSL_load_error_strings();
  /* 以 SSL V2 和 V3 标准兼容方式产生一个 SSL_CTX ，即 SSL Content Text */
  ctx = SSL_CTX_new(SSLv23_server_method());
  /* 也可以用 SSLv2_server_method() 或 SSLv3_server_method() 单独表示 V2 或 V3标准 */
  if (ctx == NULL)
  {
    ERR_print_errors_fp(stdout);
    exit(1);
  }
#endif
 // ssl_init_pk(ctx);
  puts("ssl init");
  create_recvdir(fdir);

  puts("recvdir init");
  ssl_load_cert_priv(ctx);
  /*--------------------- 开启一个 socket 监听---------------------------*/
  sockfd = socket_init(lisnum,argv);

  /* Accept client connection */
  int pid;
  while (1)
  {
    SSL *ssl;
    len = sizeof(struct sockaddr);
	  new_fd = Accept(sockfd, (struct sockaddr *) &their_addr, &len);
      printf("server: got connection from %s, port %d, socket %d\n", inet_ntoa(their_addr.sin_addr), ntohs(their_addr.sin_port), new_fd);
	pid = fork();
	if(pid < 0)
	{
		perror("call to fork");
	}else if(pid == 0){

    ssl = SSL_new(ctx);
    /* insert user's socket to SSL */
    SSL_set_fd(ssl, new_fd);
    /* establish SSL connection */
    if (SSL_accept(ssl) == -1)
    {
      perror("accept");
      close(new_fd);
      break;
    }

    /* 接受客户端所传文件的文件名并在特定目录创建空文件 */
	fd = create_recvfile(ssl,new_fileName,buf);
    /* 接收客户端的数据并写入文件 */
	write_data(ssl,buf,fd);

    close(fd); 	/* close file */
    SSL_shutdown(ssl);  /* close ssl connection */
    SSL_free(ssl);		/* free ssl */
    close(new_fd);		/* close client socket */
  }else{
 	close(new_fd); 
  }
}

  /* 关闭监听的 socket */
  close(sockfd);
  /* 释放 CTX */
  SSL_CTX_free(ctx);
  return 0;
}
