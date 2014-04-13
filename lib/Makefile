SRCS = account.c \
	   cut_str.c \
	   list.c	\
	   sftfile.c \
	   sftpack.c \
	   sock_wrap.c \
	   ssl_wrap.c \
	   ui.c
OBJS = $(patsubst %.c,%.o,$(SRCS))
CC = gcc
INCPATH = -I../include
LIB = libsftpsys.a
CFLAGS = $(INCPATH) 
AR = ar  
$(LIB):$(OBJS)
	$(AR) rcs $(LIB) $(OBJS) 
$(OBJS):$(SRCS)
	$(CC) -c $(SRCS) $(CFLAGS)
clean:
	rm -rf *.o
	rm -rf $(LIB)
