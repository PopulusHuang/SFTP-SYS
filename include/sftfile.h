#ifndef _SFTFILE_H_
#define _SFTFILE_H_
#include "ssl_wrap.h"
#define FILENAME_SIZE 100
#define PATH_SIZE	  100
/* file attribute */
typedef struct file_attr{
	int size;	
	char name[FILENAME_SIZE];
}FILE_ATTR;

int sftfile_get_name(char *filename,char *prompt);
int sftfile_get_size(char *filename);
int sftfile_open(char *filename,int flags);
FILE *sftfile_fopen(char *filename,char *mode);
int sftfile_fsend(SSL *ssl,int order,FILE *fp,int file_size);
int sftfile_frecv(SSL *ssl,int order,FILE *fp,int file_size);
void sftfile_userdir(char *fdir);
int sftfile_recv(SSL *ssl,int order,int fd,int file_size);
int sftfile_send(SSL *ssl,int order,int fd,int file_size);
#endif
