#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "account.h"
#include "sftpack.h"
#define BUF_SIZE 1024

/* descried: get account name from input 
 * argument: 
 * 	 account     -- user name or passwd;
 * 	 prompt_info -- prompt information;
 * 	 size        -- size of account;
 * */
int account_input(char *account,char *prompt_info,int size)  
{  
   printf("%s",prompt_info);	/* print prompt information */
   fgets(account,size,stdin);
   int n = strlen(account);
   account[n-1] = '\0';
   return n;  
}  
/* verify user's name and passwd */
int account_verify(sqlite3 *db,char *username,char *passwd)
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
		printf("%s is exist!\n",username);
	if(passwd != NULL)	/* verify passwd */
	{
		for (i = 0;i < (row+1)*col;i++)
		{
			if(strcmp(aResult[i],passwd) == 0)
			{
				/* match the password */
		//		printf("the passwd is:%s\n",aResult[i]);
				return USER_OK;
			}
		}
				printf("the passwd is wrong!\n");
				return PASSWD_ERR;
	}
	sqlite3_free_table(aResult);
	return USER_OK;
}
/* register a new account */
int account_register(sqlite3 *db,char *username,char *passwd)
{
	char sql[BUF_SIZE];
	char *zErrMsg = NULL;
	int ret;
	int n;
	/* check the username is already exist first */
	if(account_verify(db,username,NULL) != USER_OK)	
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
			n = REGISTER_OK;
		}

	}
	else	
	{
		printf("Account is already exits!\n");	
		n = USER_EXIST;  
	}
		return ;
}
int account_modify_passwd(sqlite3 *db,char *username,char *passwd)
{
	char sql[BUF_SIZE];
	char *zErrMsg = NULL;
	int ret;
	int n = -1;
	memset(sql,0,sizeof(sql));
	sprintf(sql,"update user set passwd='%s' where name='%s'",passwd,username);	
	ret = sqlite3_exec(db,sql,NULL,NULL,&zErrMsg);
	if(ret != SQLITE_OK)
	{
		fprintf(stderr,"SQL error:%s\n",zErrMsg);	
		sqlite3_free(zErrMsg);
	}
	else
	{
		printf("Modify password succeed!\n");	
		n = PASSWD_OK;
	}
	return n;
}
/* cut blank character */
void cut_blank(char *src,char *dest)
{
	int i,j,n;
	n = strlen(src);
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
	//dest[j] = '\0';
}
/* send account to server */
int account_send(SSL *ssl,ACCOUNT user,int order)
{
	SFT_PACK sftpack;		

	/* pack data */
	sftpack_wrap(&sftpack,order,INVAILD,"");
	cut_blank(user.name,sftpack.data.user.name);
	strcpy(sftpack.data.user.passwd,user.passwd);
	int n = sftpack_send(ssl,&sftpack);
	if(n > 0)
		return 0;
	return -1;
}
