#ifndef _ACCOUNT_H_
#define _ACCOUNT_H_
#define USER_OK			1
#define USER_ERROR		4
#define NAME_SIZE 		20
#define PASSWD_SIZE		20
#include <sqlite3.h>
#include "ssl_wrap.h"
typedef struct account{
	char name[NAME_SIZE];
	char passwd[PASSWD_SIZE];
}ACCOUNT;
int account_input(char *account,char *prompt_info,int size);
int account_register(sqlite3 *db,char *username,char *passwd);
int account_send(SSL *ssl,ACCOUNT user,int order);
int account_verify(sqlite3 *db,char *username,char *passwd);
int account_modify_passwd(sqlite3 *db,char *username,char *passwd);
#endif
