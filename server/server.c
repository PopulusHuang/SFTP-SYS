#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define MAXBUF 1024

int main(int argc, char **argv)
{
  int sockfd, new_fd, fd;
  socklen_t len;
  struct sockaddr_in my_addr, their_addr;
  unsigned int myport, lisnum; //lisnum-Max listen number
  char buf[MAXBUF + 1]; //add one space to save '\0'
  char new_fileName[50]="ClientFile/";
  char *fdir="ClientFile";		//receive file's path
  SSL_CTX *ctx; //SSL Content Text
  mode_t mode;
  char pwd[100];
  char* temp;
	
  mode=0755;
  if(access(fdir,F_OK) != 0)
  	mkdir(fdir,mode);

  if (argv[1])
    myport = atoi(argv[1]);
  else
  {
    myport = 7838;
    argv[2]=argv[3]=NULL;
  }

  if (argv[2])
    lisnum = atoi(argv[2]);
  else
  {
    lisnum = 2;
    argv[3]=NULL;
  }

  
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
  /* 载入用户的数字证书， 此证书用来发送给客户端。 证书里包含有公钥 */
  getcwd(pwd,100);
  if(strlen(pwd)==1)
    pwd[0]='\0';
  if (SSL_CTX_use_certificate_file(ctx, temp=strcat(pwd,"/cacert.pem"), SSL_FILETYPE_PEM) <= 0)
  {
    ERR_print_errors_fp(stdout);
    exit(1);
  }
  /* 载入用户私钥 */
  getcwd(pwd,100);
  if(strlen(pwd)==1)
    pwd[0]='\0';
  if (SSL_CTX_use_PrivateKey_file(ctx, temp=strcat(pwd,"/privkey.pem"), SSL_FILETYPE_PEM) <= 0)
  {
    ERR_print_errors_fp(stdout);
    exit(1);
  }
  /* 检查用户私钥是否正确 */
  if (!SSL_CTX_check_private_key(ctx))
  {
    ERR_print_errors_fp(stdout);
    exit(1);
  }

  /*--------------------- 开启一个 socket 监听---------------------------*/
  if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("socket");
    exit(1);
  }
  else
    printf("socket created\n");

  bzero(&my_addr, sizeof(my_addr));
  my_addr.sin_family = PF_INET;
  my_addr.sin_port = htons(myport);
  if (argv[3])
    my_addr.sin_addr.s_addr = inet_addr(argv[3]);
  else
    my_addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr)) == -1)
  {
    perror("bind");
    exit(1);
  }
  else
    printf("binded\n");

  if (listen(sockfd, lisnum) == -1)
  {
    perror("listen");
    exit(1);
  }
  else
    printf("begin listen\n");

/*-------------------------------------------------------------------*/
  while (1)
  {
    SSL *ssl;
    len = sizeof(struct sockaddr);
    /* 等待客户端连上来 */
    if ((new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &len)) == -1)
    {
      perror("accept");
      exit(errno);
    }
    else
      printf("server: got connection from %s, port %d, socket %d\n", inet_ntoa(their_addr.sin_addr), ntohs(their_addr.sin_port), new_fd);

    /* 基于 ctx 产生一个新的 SSL */
    ssl = SSL_new(ctx);
    /* 将连接用户的 socket 加入到 SSL */
    SSL_set_fd(ssl, new_fd);
    /* 建立 SSL 连接 */
    if (SSL_accept(ssl) == -1)
    {
      perror("accept");
      close(new_fd);
      break;
    }

    /* 接受客户端所传文件的文件名并在特定目录创建空文件 */
	int n = 0;
	n = strlen(new_fileName);
    bzero(buf, MAXBUF + 1);
    bzero(new_fileName+n, n);
    len = SSL_read(ssl, buf, MAXBUF);	/* read the file name */
    if(len == 0)
      printf("Receive File Name Complete !\n");
    else if(len < 0)
      printf("Failure to receive message ! Error code is %d，Error messages are '%s'\n", errno, strerror(errno));
    if((fd = open(strcat(new_fileName,buf),O_CREAT | O_TRUNC | O_RDWR,0666))<0)
    {
      perror("open:");
      exit(1);
    }
	printf("open file:%s succeed\n",new_fileName);
    /* 接收客户端的数据并写入文件 */
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

    /* 关闭文件 */
    close(fd);
    /* 关闭 SSL 连接 */
    SSL_shutdown(ssl);
    /* 释放 SSL */
    SSL_free(ssl);
    /* 关闭 socket */
    close(new_fd);
  }

  /* 关闭监听的 socket */
  close(sockfd);
  /* 释放 CTX */
  SSL_CTX_free(ctx);
  return 0;
}
