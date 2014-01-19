/* *********************************************** * 
 *                                                 * 
 *   command.c:   SFTP  Commands                   * 
 *                                                 * 
 * *********************************************** */
#include "command.h"
#include "clnt_parse.h"
#include "../share/sftpack.h"
#include "../share/sftfile.h"
#include <fcntl.h>
#include <sys/stat.h>
/* String to pass to system ().  This is for the LIST,
 * VIEW and RENAME commands. */
static char syscmd[1024];

COMMAND commands[] = {
  { "cd", cmd_cd, "[DIR]\n\t--Change to directory DIR" },
  { "delete", cmd_delete, "--Delete FILE" },
  { "help", cmd_help, "--Display this text" },
  { "?", cmd_help, "--Synonym for `help'" },
  { "ls", cmd_ls, "[DIR]\n\t--List files in DIR" },
  {	"!ls",cmd_ls_srv, "[SRVDIR]\n\t--List server files in SRVDIR"},
  { "pwd", cmd_pwd, "--Print the current working directory" },
  { "quit", cmd_quit, "--Quit this console" },
  { "stat", cmd_stat, "[FILE]\n\t--Print out statistics on FILE" },
  { "view", cmd_view, "[FILE]\n\t--View the contents of FILE" },
  {	"put",cmd_put,"[FILE]\n\t--upload files to server"},
  {	"get",cmd_get,"[FILE]\n\t--download files from server"},
  { (char *)NULL, (Function *)NULL, (char *)NULL }
};
/* Function which tells you that you can't do this. */
int too_dangerous (char *caller)
{
  fprintf (stderr,
           "%s: Too dangerous for me to distribute.  Write it yourself.\n",
           caller);
}

/* Return non-zero if ARG is a valid argument for CALLER, else print
   an error message and return zero. */
int valid_argument (char *caller,char *arg)
{
  if (!arg || !*arg)
    {
      fprintf (stderr, "%s: Argument required.\n", caller);
      return (0);
    }

  return (1);
}
/* List the file(s) named in arg. */
int cmd_ls (char *arg)
{
  if (!arg)
    arg = "";

  sprintf (syscmd, "ls -FClg %s", arg);
  return (system (syscmd));
}
int cmd_ls_srv(char *arg)
{
	if(!arg)
		arg = "";
	sprintf(syscmd,"%s/%s",LOGIN_USER.name,arg);
	list_server(CMDSSL,CSCS,syscmd,"-xF");
	return 0;
}
int cmd_view (char *arg)
{
  if (!valid_argument ("view", arg))
    return 1;

  sprintf (syscmd, "more %s", arg);
  return (system (syscmd));
}

int cmd_stat (char *arg)
{
  struct stat finfo;

  if (!valid_argument ("stat", arg))
    return (1);

  if (stat (arg, &finfo) == -1)
    {
      perror (arg);
      return (1);
    }

  printf ("Statistics for `%s':\n", arg);

  printf ("%s has %d link%s, and is %d byte%s in length.\n", arg,
          finfo.st_nlink,
          (finfo.st_nlink == 1) ? "" : "s",
          finfo.st_size,
          (finfo.st_size == 1) ? "" : "s");
  printf ("Inode Last Change at: %s", ctime (&finfo.st_ctime));
  printf ("      Last access at: %s", ctime (&finfo.st_atime));
  printf ("    Last modified at: %s", ctime (&finfo.st_mtime));
  return (0);
}

int cmd_delete (char *arg)
{
  too_dangerous ("delete");
  return (1);
}

/* Print out help for ARG, or for all of the commands if ARG is
   not present. */
int cmd_help (char *arg)
{
  register int i;
  int printed = 0;

  for (i = 0; commands[i].name; i++)
    {
      if (!*arg || (strcmp (arg, commands[i].name) == 0))
        {
          printf ("%s\t%s.\n", commands[i].name, commands[i].doc);
          printed++;
        }
    }

  if (!printed)
    {
      printf ("No commands match `%s'.  Possibilties are:\n", arg);

      for (i = 0; commands[i].name; i++)
        {
          /* Print in six columns. */
          if (printed == 6)
            {
              printed = 0;
              printf ("\n");
            }

          printf ("%s\t", commands[i].name);
          printed++;
        }

      if (printed)
        printf ("\n");
    }
  return (0);
}

/* Change to the directory ARG. */
int cmd_cd (char *arg)
{
  if (chdir (arg) == -1)
    {
      perror (arg);
      return 1;
    }

  cmd_pwd ("");
  return (0);
}

/* Print out the current working directory. */
int cmd_pwd (char *ignore)
{
  char dir[1024];

  if (!getwd(dir))
    {
	  perror("getwd:");
      return 1;
    }

  printf ("Current directory is: %s\n", dir);
  return 0;
}

/* The user wishes to quit using this program.  Just set DONE non-zero. */
int cmd_quit (char *arg)
{
  DONE = 1;
  return (0);
}
int cmd_put(char *arg)
{
	int fd;
	SFT_PACK pack;
	SFT_DATA data;
	SSL *ssl=CMDSSL;
	char filename[FILENAME_SIZE];
	int file_size;
	int order = CUP;
	/* get file name*/
	strcpy(filename,arg);
    /* open the local file */
	fd = sftfile_open(filename,O_RDONLY);
	if(fd < 0)
	{
		return -1;	
	}
	file_size = sftfile_get_size(filename);
	cut_path(filename);
	sprintf(data.file_attr.name,"%s/%s",LOGIN_USER.name,filename);
	data.file_attr.size = file_size;
	/* package data and send */
	sftpack_wrap(&pack,order,ASK,"\0");	
	pack.data = data;
	strcpy(pack.buf,LOGIN_USER.name);
	sftpack_send(ssl,&pack);
	/* get respond */
	int n = sftpack_recv_ack(ssl,order);
	if(n == ACCEPT)
	{
	/* send file to server */ 
	 sftfile_send(ssl,order,fd,file_size);
	/* get ack code */
	 n = sftpack_recv_ack(ssl,order);
	 if(n == FINISH)
	 {
	 	printf("upload %s to sever succeed,total %0.1fKB\n",
	   	filename,(float)file_size/1024);
		list_server(ssl,CSCS,LOGIN_USER.name,"-xF");	
	 }
	 else
	 {
	 	printf("upload %s failure!\n",filename);	
	 }
	}
	else
		fprintf(stderr,"request upload failure!\n");
  	close(fd);
	return 0;
}
int cmd_get(char *arg)
{
	int size;
	int ack,fd;
	int order = CDOWN;
	char filename[FILENAME_SIZE];
	char localname[FILENAME_SIZE];
	SFT_PACK pack;
	SFT_DATA data;
	SSL *ssl=CMDSSL;
	/* scan local and server files */
	scan_all(ssl);
	sftfile_userdir(DOWNLOAD_DIR);
	bzero(localname,sizeof(localname));
	/* input filename on server to download */
	strcpy(filename,arg);
	if(strstr(filename,"..")!=NULL)
	{
		fprintf(stderr,"filename error: deny contain '..'\n");	
		return -1;
	}
	sprintf(data.file_attr.name,"%s/%s",LOGIN_USER.name,filename);

	cut_path(filename);
	sprintf(localname,"%s%s",DOWNLOAD_DIR,filename);
	/*send file information to server */
	sftpack_wrap(&pack,order,ASK,"");	
	pack.data = data;
	sftpack_send(ssl,&pack);
	/* receive server respond */
	sftpack_recv(ssl,&pack);
	ack =pack.ack;
	if(ack == ACCEPT)
	{
		size = pack.data.file_attr.size;
		fd = sftfile_open(localname,O_CREAT | O_TRUNC | O_RDWR);
		/* receive file */
		if(sftfile_recv(ssl,order,fd,size) == 0)
		{
			printf("Downlad %s succeed\n",filename);
			list_client(DOWNLOAD_DIR," --color=auto ");
		}	
		else
		{
			printf("Download %s failure!\n",filename);	
			return -1;
		}
	}
	return 0;
}
