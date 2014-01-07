#ifndef _SFTFILE_H_
#define _SFTFILE_H_
#include "ssl_wrap.h"
#define FILENAME_SIZE 50
/* file attribute */
typedef struct file_attr{
	int size;	
	char name[FILENAME_SIZE];
}FILE_ATTR;

int sftfile_get_name(char *filename,char *prompt);
int sftfile_get_size(char *filename);
int sftfile_open(char *filename,int flags);
void sftfile_recvdir(char *fdir);
int sftfile_recv(SSL *ssl,int order,int fd,int file_size);
int sftfile_send(SSL *ssl,int order,int fd,int file_size);
#endif
