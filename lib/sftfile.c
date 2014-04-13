/*
 * ========================================================================
 *
 *       Filename:  sftfile.c
 *
 *    Description:  Handle SFTP-SYS's file transport 
 *
 *        Version:  1.0
 *           Date:  04/12/14 08:52:26 PM
 *       Compiler:  gcc-4.6.3
 *
 *         Author:  Populus Huang 
 *          Email:  thoughtsonlive@gmail.com
 *
 * ========================================================================
 */

#include "sftfile.h"
#include "sftpack.h"
#include "ui.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
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
/* create the user directory  */
void sftfile_userdir(char *fdir)
{
  mode_t mode;
  mode=0755;
  /*Is the directory exist*/
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
	
    printf(YELLOW"(~_^)'>%s file name$ "NONE,prompt); 
	fgets(filename,FILENAME_SIZE,stdin);
	filename[strlen(filename)-1] = '\0';
	return 0;
}
/* show the rate of progress */
int sftfile_progress(float size,float total)
{
	float progress;	
	progress = (size*100)/total;
	printf("Progress: %0.0f%%\r",progress);
	fflush(stdout);
	return 0;
}

/* send a file */
int sftfile_send(SSL *ssl,int order,int fd,int file_size)
{
	SFT_PACK pack;	
	int len;
	int nread;
	float send_size = 0;
	printf(CURSOR_HIDE);
	while(1)
	{
		sftpack_init(&pack);
		nread = Read(fd,pack.buf,DATA_SIZE);
		if(nread <= 0)
			  break;
		pack.order = order;
		pack.ack = INVALID;
		pack.data.file_attr.size = nread;
		send_size += nread;
		/* send package */
		sftpack_send(ssl,&pack);	
		/* show the rate of progress */
		sftfile_progress(send_size,file_size);
	}
	printf(CURSOR_SHOW);
	printf("send total %0.1fKB \n",send_size/1024);
	sftpack_wrap(&pack,order,FINISH,"\0");
	sftpack_send(ssl,&pack);
	return 0;
}
/* receive a file */
int sftfile_recv(SSL *ssl,int order,int fd,int file_size)
{
	SFT_PACK pack;
	int n;
	float recv_size = 0;
	sftpack_init(&pack);	
	printf(CURSOR_HIDE);
	while(1)
	{
		sftpack_recv(ssl,&pack);
		/* check the end */
		if((pack.ack==FINISH)&&(pack.order == order))	
		{
			printf("Receive file complete! total %0.1fKB !\n",
							recv_size/1024);	
			break;
		}
		else if(errno == EAGAIN) /* no data come */
		{
			usleep(10000);	/* wait for client's data */
			continue;		
		}
		n = pack.data.file_attr.size;
		recv_size += n;
		if(Write(fd,pack.buf,n) < 0)
				return -1;
		memset(pack.buf,0,DATA_SIZE);
		sftfile_progress(recv_size,file_size);
	}
	printf(CURSOR_SHOW);
	return 0;
}
