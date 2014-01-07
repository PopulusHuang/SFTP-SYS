#include "sftfile.h"
#include "sftpack.h"
#include <sys/stat.h>
#include <sys/types.h>
/* open file */
int sftfile_open(char *filename,int flags)
{
	int fd;
	fd = open(filename,flags,0666);
	if(fd < 0)
	{
		perror("sftfile open");	
	}
	return fd;
}
/* create the receive directly */
void sftfile_recvdir(char *fdir)
{
  mode_t mode;
  mode=0755;
  /*Is the directly exist*/
  if(access(fdir,F_OK) != 0) 
  {
  	mkdir(fdir,mode);
  }
}
/* get file size */
int sftfile_get_size(char *filename)
{
	int size;
	struct stat file_stat;
	if(stat(filename,&file_stat) == -1)
	{
		perror("file stat:");
		return -1;
	}
	size = file_stat.st_size; 
	return size;
}
/* get file name */
int sftfile_get_name(char *filename,char *prompt)
{
	
    printf("\033[31m@~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~@ \033[0m\n");
    printf("\n\033[31m@Please input the filename:\033[0m\n");
    printf("\033[33m(~_^)'>%s$ \033[0m",prompt); 
	fgets(filename,FILENAME_SIZE,stdin);
	filename[strlen(filename)-1] = '\0';
	return 0;
}
/* send a file */
int sftfile_send(SSL *ssl,int order,int fd)
{
	SFT_PACK pack;	
	char buf[DATA_SIZE];
	int len;
	bzero(buf,DATA_SIZE);
	while((Read(fd,buf,DATA_SIZE-1)) > 0)
	{
		sftpack_wrap(&pack,order,INVAILD,buf);
		/* send package */
		len = sftpack_send(ssl,&pack);	
		if(len < 0)
			return -1;	
		/* Don't forget to clear the buf,it will cause a 
		 * big problem which make me crazy for a long time.
		 * some message still stay in the buf,it can cause 
		 * redundance message(file's size are multiple of 
		 * DATA_SIZE),even distortion code
		 * */
		memset(buf,0,sizeof(buf));
	}
	sftpack_wrap(&pack,order,FINISH,"\0");
	sftpack_send(ssl,&pack);
	return 0;
}
/* receive a file */
int sftfile_recv(SSL *ssl,int order,int fd)
{
	SFT_PACK pack;
	int n;
	sftpack_init(&pack);	
	while((n = sftpack_recv(ssl,&pack)) > 0)
	{
		/* check the end */
		if((pack.ack==FINISH)&&(pack.order == order))	
		{
			printf("Receive file complete!\n");	
			break;
		}
		n = strlen(pack.buf);
		if(Write(fd,pack.buf,n) < 0)
				return -1;
		memset(pack.buf,0,DATA_SIZE);
	}
	return 0;
}
