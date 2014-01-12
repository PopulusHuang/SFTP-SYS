/* echo_mode.c*/
#include <stdio.h>   
#include <string.h>
#include <termios.h>   
#include <unistd.h>   
#include <errno.h>   
#include "echo_mode.h"
/* describes: control whether to open entry Echo 
 * argument:
 *    fd      -- a file describes you can use 
 *            STDIN_FILENO or STDOUNT_FILENO 
 *    option  -- a flag to set echo on or off
 * */
int echo_mode(int fd,int option)  
{  
   int err;  
   struct termios term;  
   /* get terminal attribution */
   if(tcgetattr(fd,&term)==-1){  
     perror("Cannot get the attribution of the terminal");  
     return 1;  
   }  
   /* set terminal attribution */
   if(option)  
        term.c_lflag|=ECHOFLAGS;  
   else  
        term.c_lflag &=~ECHOFLAGS;  
   err=tcsetattr(fd,TCSAFLUSH,&term);  
   if(err==-1 && err==EINTR){  
        perror("Cannot set the attribution of the terminal");  
        return 1;  
   }  
   return 0;  
}  
