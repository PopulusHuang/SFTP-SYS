#ifndef _ACCOUNT_H_
#define _ACCOUNT_H_
#define USER_OK			1
#define REGISTER_OK 	3
#define USER_ERROR		4
#define NAME_SIZE 		20
#define PASSWD_SIZE		20
#include <sqlite3.h>

typedef struct Account{
	char name[NAME_SIZE+1];
	char passwd[PASSWD_SIZE+1];
}ACCOUNT;
int fetch_name(char *data,char *name);
int fetch_passwd(char *data,char *passwd);
int input_account(char *account,char *prompt_info,int size);
int register_account(sqlite3 *db,char *username,char *passwd);
int verify_account(sqlite3 *db,char *username,char *passwd);
#endif
