#include "menu.h"
#include "client.h"
#include "parse.h"

#define USER_SIZE 20
#define PASSWD_SIZE 20  
#define BUF_SIZE 1024
#define FILENAME_SIZE 100

char order_set[ORDER_NUM][ORDER_SIZE]={"c00a","c01b","","c02c","c03d","c04e","c05f","c06g"};

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
int is_order_exist(char *order)
{
	int i;
		for (i = 0; i < ORDER_NUM ; i++) {
			if (strcmp(order_set[i],order) == 0)	
			{
				return i;
			}
		}
	return -1;
}
int send_order(SSL *ssl,char *order)
{
	int n;
	n = strlen(order);
	if(n > 10)
	{
			fprintf(stderr,"order is too long !");
			return -1;
	}
	n = SSL_write_pk(ssl,order,n);
	return n;
}
int recv_order(SSL *ssl,char *order)
{
	int n;
	bzero(order,strlen(order));
	n = SSL_read_pk(ssl,order,strlen(order));
	if(n > 10)
	{
			fprintf(stderr,"order is too long !");
			return -1;
	}
	return n;	
}
int parse_clnt_order(SSL *ssl,char *order)
{
	int n;
	int ret;
	n = is_order_exist(order);
	if(n < 0)
	{
		fprintf(stderr,"order is not exist!\n");	
		return -1;
	}
	switch(n)
	{
		case   CIN:  ret = clnt_entry(ssl,order);
					 return ret; 
		case  CREG:  ret = clnt_register(ssl,order);
					 return ret;
		case  CSCL:  scan_local_files();break;
		case  CSCS:  scan_serv_files(ssl,order);break; 
		case   CUP:  upload_files(ssl,order);break;
		case CDOWN:  download_files(ssl,order);break;
		case  COUT:  logout();break;
		default CNULL:  fprintf(stderr,"null order!\n");	
					 	return -1;
	}
	return 0;
}
int parse_serv_order(char *order)
{
	char value[2];
	char buf[ORDER_SIZE];		

	bzero(value,sizeof(value));
	bzero(buf,sizeof(buf));
	/* get the first character */
	strncpy(value,order[0],1);		
	strncpy(buf,order+1,ORDER_SIZE);
	if(strcmp(value,"0") != 0)		
	{
			printf("execute %s failed!\n",buf);	
			return -1;
	}
	return 0;
}
void package_order(char *order,char *data,char *order_pack)
{
	strcat(order_pack,order);
	strcat(order_pack,"*");
	strcat(order_pack,data);
}
int clnt_entry(SSL *ssl,char *order)
{
		char user[USER_SIZE];
		char passwd[PASSWD_SIZE];
		char data_buf[BUF_SIZE];
		char order_pack[BUF_SIZE];

		bzero(order_pack,strlen(BUF_SIZE));
login:	printf("user name:");
		fgets(user,USER_SIZE,stdin);
		user[strlen(user)-1] = '\0';   /* remove the 'Enter' */
		
		printf("passwd:");	
		fgets(passwd,PASSWD_SIZE,stdin);
		passwd[strlen(passwd)-1] = '\0';

		package_order(order,data_buf,order_pack);
		send_order(order_pack);
		bzero(order_pack,strlen(order_pack));
		recv_order(order_pack);
		int n = parse_serv_order(order_pack);
		if(n < 0)
			goto login;
		else
			return 1;

		return 0;
}
void clnt_register(SSL *ssl,char *order)
{
		char user[USER_SIZE];
		char passwd[PASSWD_SIZE];
		char data_buf[BUF_SIZE];
		char order_pack[BUF_SIZE];
		
		bzero(order_pack,strlen(BUF_SIZE));
		printf("new user name:");
		fgets(user,USER_SIZE,stdin);
		user[strlen(user)-1] = '\0';   /* remove the 'Enter' */

pwloop:	printf("new passwd:");
		fgets(passwd,PASSWD_SIZE,stdin);
		passwd[strlen(passwd)-1] = '\0';

		printf("confirm new passwd:");
		fgets(data_buf,PASSWD_SIZE,stdin);
		data_buf[strlen(data_buf)-1] = '\0';

		if(strcmp(passwd,data_buf) != 0)
		{
			fprintf(stderr,"Passwd isn't matching,confirm failed!\n");
			goto pwloop;	
		}
		else
		{
			strcat(data_buf,user);	
			package_order(order,data_buf,order_pack);
			send_order(order_pack);
			bzero(order_pack,sizeof(order_pack));
			recv_order(order_pack);
			int n = parse_serv_order(order_pack);
			if(n == 0)
				printf("%s register sucess!\n",user);
		}
}
void remove_space(char *src,char *dest)
{
	int i,j,n;
	n = strlen(src);
	bzero(dest,strlen(dest));
	for(i = 0,j = 0;i < n;i++)
	{
		if(src[i] == ' ')	
		{
			continue;
		}
		else
		{
			dest[j++] = src[i];
		}
	}
}
void scan_local_files(void)
{
	char path[BUF_SIZE];
	char cmd_buf[BUF_SIZE];
	char buf[BUF_SIZE];

	bzero(path,sizeof(path));
	bzero(buf,sizeof(buf));
	bzero(cmd_buf,sizeof(cmd_buf));
	while(1)
	{
		printf("\033[31m@~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~@ \033[0m\n");
		printf("\n\033[47;32m please input the path:~$ \033[0m");
		fgets(buf,BUF_SIZE,stdin);
		buf[strlen(buf)-1] = '\0';
		remove_space(buf,path);
		if(strcmp(path,"quit")==0||strcmp(path,"exit")==0)
				break;
		strcpy(cmd_buf,"ls -l --color=auto ");
		strcat(cmd_buf,path);
		system(cmd_buf);
		bzero(path,sizeof(path));
		bzero(buf,sizeof(buf));
	}
}
int recv_file_list(SSL *ssl)
{
	char buf[BUF_SIZE+1];
	int n;
	bzero(buf,sizeof(buf));
	while(1)
	{
		n =SSL_read_pk(ssl,buf,BUF_SIZE);
		if (n < BUF_SIZE )
		{
			if(n > 0)
				printf("%s",buf);	
			printf("\nreceive complete!\n");
			return 0;
		}
		else 
		{
			printf("%s",buf);	
		}
	}
	return 0;
}
int scan_serv_files(SSL *ssl,char *order)
{
	if(is_order_exist(order) < 0)
	{
		fprintf(stderr,"order is not exist!");	
		return -1;
	}
	else
	{
		send_order(order);
		recv_order(order);
		if(parse_serv_order(order) == 0)
				recv_file_list(ssl);

	}
	return 0;
}
int upload_files(SSL *ssl,char *order)
{

	if(is_order_exist(order) < 0)
	{
		fprintf(stderr,"order is not exist!");	
		return -1;
	}
	else
	{
		send_order(order);
		recv_order(order);
		if(parse_serv_order(order) == 0)
			send_file(ssl);
	}
}
int create_emptyfile(char *filename)
{
		int fd = open(fileName,O_CREAT | O_TRUNC | O_RDWR,0666)	
		if(fd < 0)
		{
			perror("open:");	
			return -1;
		}
		printf("open file:%s succeed!\n",filename);
		return fd;
}
int recv_file_data(SSL *ssl,int fd)
{
	int n;		
	char buf[BUF_SIZE+1];
	while(1)
	{
		bzero(buf,BUF_SIZE+1);	
		n = SSL_read_pk(ssl,buf,BUF_SIZE);
		if(n == 0)
		{
			puts("Receive Complete!");
			break;
		}
		if(Write(fd,buf,n) < 0)
			return -1;
	}
	return 0;
}
int download_files(SSL *ssl,char *order)
{
	char filename[FILENAME_SIZE];
	char sendFN[FILENAME_SIZE];
	char path[FILENAME_SIZE] = "./recv_dir";
  printf("\033[31m@~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~@ \033[0m\n");
  printf("\nPlease input the filename of you want to download :\n(~_^)'$");
	fgets(filename,FILENAME_SIZE,stdin);
	filename[strlen(filename)-1] = '\0';
	send_order(ssl,order);
	recv_order(ssl,order);
	if(parse_serv_order(order)==0)
	{
		create_recvdir(path);
		strcat(path,filename);
		int fd = create_emptyfile(path);
		recv_file_data(ssl,fd);
	}
	return 0;
}
