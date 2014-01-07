#ifndef _ECHO_MODE_
#define _ECHO_MODE_
#include <termios.h>   
#include <unistd.h>   
#include <errno.h>   
#define ECHOFLAGS (ECHO | ECHOE | ECHOK | ECHONL)   
#define ECHO_ON 1
#define ECHO_OFF 0
int echo_mode(int fd,int option);
#endif
