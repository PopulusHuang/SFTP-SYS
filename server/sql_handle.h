#ifndef _SQL_HANDLE_H_
#define _SQL_HANDLE_H_
#define USER_OK			1
#define USER_ERROR		2
#define PASSWD_ERROR 	3
#include <sqlite3.h>
int register_account(sqlite3 *db,char *username,char *passwd);
int verify_account(sqlite3 *db,char *username,char *passwd);
#endif
