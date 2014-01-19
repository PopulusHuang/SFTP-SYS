/*console.c*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include "command.h"
#if 1
COMMAND * find_command (char *name);
void initialize_readline (void);
char * command_generator (char *text,int state);
int execute_line (char *line);
char **sftp_completion (char *text,int start,int end);
char * strip_white (char *string);
/* Execute a command line. */
int execute_line (char *line)
{
  register int i;
  COMMAND *command;
  char *word;

  /* Isolate the command word. */
  i = 0;
  while (line[i] && isblank(line[i]))/*skip the blank*/
    i++;
  word = line + i;

  while (line[i] && !isblank (line[i]))/* travel to the end of command */
    i++;

  if (line[i])
    line[i++] = '\0';/* replace blank with '\0' */

  command = find_command (word);/* fetch the command */

  if (!command)
    {
      fprintf (stderr, "%s: No such command for FileMan.\n", word);
      return (-1);
    }

  /* Get argument to command, if any. */
  while (isblank (line[i]))
    i++;

  word = line + i;

  /* Call the function. */
  return ((*(command->func)) (word));
}
#endif
/* Look up NAME as the name of a command, and return a pointer to that
   command.  Return a NULL pointer if NAME isn't a command name. */
COMMAND * find_command (char *name)
{
  register int i;

  for (i = 0; commands[i].name; i++)
    if (strcmp (name, commands[i].name) == 0)
      return (&commands[i]);

  return ((COMMAND *)NULL);
}

/* Strip blank from the start and end of STRING.  Return a pointer
   into STRING. */
char * strip_white (char *string)
{
  register char *head, *tail;

  for (head = string; isblank (*head); head++)
    ;
    
  if (*head == 0)	/* null string */
    return (head);

  tail = head + strlen (head) - 1;
  while (tail > head && isblank (*tail))
    tail--;
  *++tail = '\0';

  return head;
}
int console(SSL *ssl)
{
  char *line, *s;

  CMDSSL=ssl;
  initialize_readline ();       /* Bind our completer. */

  line = (char *)NULL;
  /* Loop reading and executing lines until the user quits. */
  for ( ; DONE == 0; )
    {

      line = readline ("SFTP>> ");
      if (!line) /* line == NULL */
        break;

      /* Remove leading and trailing whitespace(before command) from the line.
         Then, if there is anything left, add it to the history list
         and execute it. */
      s = strip_white (line);

      if (*s)
        {
          add_history (s);/* add the command to history */
          execute_line (s);
        }

      free (line);
    }
  return 0;
}

/* **************************************************************** */
/*                                                                  */
/*                  Interface to Readline Completion                */
/*                                                                  */
/* **************************************************************** */

/* Tell the GNU Readline library how to complete.  We want to try to complete
   on command names if this is the first word in the line, or on filenames
   if not. */
void initialize_readline ()
{
  /* Allow conditional parsing of the ~/.inputrc file. */
  rl_readline_name = "SFTP";

  /* Tell the completer that we want a crack first. */
  rl_attempted_completion_function = (CPPFunction *)sftp_completion;
}

/* Attempt to complete on the contents of TEXT.  START and END show the
   region of TEXT that contains the word to complete.  We can use the
   entire line in case we want to do some simple parsing.  Return the
   array of matches, or NULL if there aren't any. */
char **sftp_completion (char *text,int start,int end)
{
  char **matches;

  matches = (char **)NULL;

  /* If this word is at the start of the line, then it is a command
     to complete.  Otherwise it is the name of a file in the current
     directory. */
  if (start == 0)
    matches = completion_matches (text, command_generator);

  return (matches);
}

/* Generator function for command completion.  STATE lets us know whether
   to start from scratch; without any state (i.e. STATE == 0), then we
   start at the top of the list. */
char * command_generator (char *text,int state)
{
  static int list_index, len;
  char *name;

  /* If this is a new word to complete, initialize now.  This includes
     saving the length of TEXT for efficiency, and initializing the index
     variable to 0. */
  if (!state)
    {
      list_index = 0;
      len = strlen (text);
    }

  /* Return the next name which partially matches from the command list. */
  while (name = commands[list_index].name)
    {
      list_index++;

      if (strncmp (name, text, len) == 0)
        return (strdup(name));
    }

  /* If no names matched, then return NULL. */
  return ((char *)NULL);
}
