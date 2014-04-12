#ifndef _COMMAND_H_
#define _COMMAND_H_
#include "ssl_wrap.h"
#include <readline/readline.h>
#include <readline/history.h>
typedef struct {
  char *name;       /* User printable name of the function. */
  Function *func;   /* Function to call to do the job. */
  char *doc;        /* Documentation for this function.  */
}COMMAND;
int DONE;
SSL *CMDSSL;
extern COMMAND commands[];
int cmd_cd (char *arg);
int cmd_delete (char *arg);
int cmd_get(char *arg);
int cmd_help (char *arg);
int cmd_ls (char *arg);
int cmd_ls_srv(char *arg);
int cmd_put(char *arg);
int cmd_pwd (char *ignore);
int cmd_quit (char *arg);
int cmd_stat (char *arg);
int cmd_view (char *arg);
int too_dangerous (char *caller);
int valid_argument (char *caller,char *arg);
#endif
