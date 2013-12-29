#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sql_handle.h"
#define BUF_SIZE 1024
/* verify user's name and passwd */
int verify_account(sqlite3 *db,char *username,char *passwd)
{
	char sql[BUF_SIZE];	/* SQL sentene */	
	int ret;
	int row,col;
	char *err = 0;
	char **aResult;

	sprintf(sql,"select name,passwd from user where name='%s';",username);
	/* sqlite3 query, the result are stored in a array 
	 * which was the 'aResult' point to*/
	sqlite3_get_table(db,sql,&aResult,&row,&col,&err);
	int i;

	if(row == 0||col ==0) /* empty result */
	{
		printf("%s is not exist!\n",username);
		return USER_ERROR;
	}
		printf("row:%d column:%d\n",row,col);
		printf("%s is exist!\n",username);
	if(passwd != NULL)	/* verify passwd */
	{
		for (i = 0;i < (row+1)*col;i++)
		{
			if(strcmp(aResult[i],passwd) == 0)
			{
				/* match the password */
				printf("the passwd is:%s\n",aResult[i]);
				return USER_OK;
			}
		}
				printf("the passwd is wrong!\n");
				return PASSWD_ERROR;
	}
	sqlite3_free_table(aResult);
	return USER_OK;
}
/* register a new account */
int register_account(sqlite3 *db,char *username,char *passwd)
{
	char sql[BUF_SIZE];
	char *zErrMsg = NULL;
	int ret;
	/* check the username is already exist first */
	if(verify_account(db,username,NULL) != USER_OK)	
	{
		/* add new account*/
		sprintf(sql,"insert into user values('%s','%s');",
				     username,passwd);
		ret = sqlite3_exec(db,sql,NULL,NULL,&zErrMsg);
		if(ret != SQLITE_OK)
		{
			fprintf(stderr,"SQL error:%s\n",zErrMsg);	
			sqlite3_free(zErrMsg);
		}
		else
		{
			printf("Account add successfully!\n");	
			return 0;
		}

	}
	else	
	{
		printf("Account is already exits!\n");	
	}
		return -1;
}
