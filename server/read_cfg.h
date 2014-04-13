#ifndef _READ_CFG_H_
#define _READ_CFG_H_
#define CFG_KEY_SIZE 20
#define CFG_VALUE_SIZE 256
typedef struct sftpcfg{
	char comment_char;		
	char sep_char;		/* separate char */
	char *key;
	char *value;
}sftpcfg_t;
int cfg_readValue(FILE *cfg_file,sftpcfg_t config);
#endif
