/*
 * ========================================================================
 *
 *       Filename:  read_cfg.c
 *
 *    Description: read a configure file by key-value model 
 *
 *        Version:  1.0
 *           Date:  04/12/14 05:44:59 PM
 *       Compiler:  gcc-4.6.3
 *
 *         Author:  Populus Huang 
 *          Email:  thoughtsonlive@gmail.com
 *
 * ========================================================================
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "read_cfg.h"
#include "cut_str.h"
#define LINE_SIZE 256
#define REGEXP_SIZE 40


int Fseek(FILE *fp,long offset,int whence)
{
	if(offset < 0)
		return -1;
	if(fseek(fp,offset,SEEK_SET) < 0)
	{
		perror("fseek");
		exit(1);	
	}
	return 0;
}
/* find the comments character from buf. 
 * return: 1--succeed  
 * 		   0--failed
 * */
int is_comment(const char *buf,char comment_char)
{
	int i;
	for (i = 0; i < strlen(buf); i++) {
		if(buf[i] == comment_char)	
			return 1;
	}
	return 0;
}
/* search the key and return it's offset 
 * in the file if succeed.*/
int cfg_searchKey(FILE *fp,sftpcfg_t config)
{
	char src_key[CFG_KEY_SIZE];
	char dest_key[CFG_KEY_SIZE];
	char line[CFG_VALUE_SIZE];
	char regexp[REGEXP_SIZE];
	int offset = 0;
	int len = 0;
	
	memset(line,0,sizeof(line));
	memset(src_key,0,sizeof(src_key));
	memset(dest_key,0,sizeof(dest_key));
	memset(regexp,0,sizeof(regexp));

	sprintf(regexp,"%%[^%c]",config.sep_char);
	long curr_offset = 0;
	curr_offset = ftell(fp);

	/* make sure seek from the head of file */
	Fseek(fp,0,SEEK_SET);
	while(fgets(line,LINE_SIZE,fp))
	{
		len = strlen(line);	
		offset += len;
		/* skip comments */
		if(is_comment(line,config.comment_char))
		{
				continue;
		}
		sscanf(line,regexp,src_key);	
		cut_blank(src_key,dest_key);
		if(strcmp(dest_key,config.key) == 0)
		{
			Fseek(fp,curr_offset,SEEK_SET);
			/* back to the head of line */
			offset -= len;
			return offset;	
		}
	memset(dest_key,0,sizeof(dest_key));
	}
	Fseek(fp,curr_offset,SEEK_SET);
	return -1;
}

/* read the key's value */
int cfg_readValue(FILE *cfg_file,sftpcfg_t config)
{
	char line[LINE_SIZE];
	char src_value[CFG_VALUE_SIZE];
	char dest_value[CFG_VALUE_SIZE];
	char regexp[REGEXP_SIZE];
	int linenum = 0;
	int offset = 0; 
	memset(line,0,sizeof(line));
	memset(src_value,0,sizeof(src_value));
	memset(dest_value,0,sizeof(dest_value));
	memset(regexp,0,sizeof(regexp));
	sprintf(regexp,"%%*[^%c] %c %%s",config.sep_char,config.sep_char);

	offset = cfg_searchKey(cfg_file,config);

	if( offset < 0)
	{
		fprintf(stderr,"'%s' key is not found !\n",config.key);
		exit(1);
	}
	Fseek(cfg_file,offset,SEEK_SET);
	if(fgets(line,LINE_SIZE,cfg_file))
	{
		sscanf(line,regexp,src_value);
		cut_blank(src_value,dest_value);
		/* storage the value */
		strcpy(config.value,dest_value);
		return 0;
	}
	return -1;
}
