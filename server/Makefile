SERVER_OBJS = server.o \
			  srv_handle.o \
			  myepoll.o \
			  sockssl.o \
			  srv_parse.o \
			  read_cfg.o
SHAREDIR = $(shell pwd)/../share/
INCPATH = -I$(SHAREDIR)
SHARED_OBJS = $(SHAREDIR)sock_wrap.o \
			  $(SHAREDIR)sftpack.o \
			  $(SHAREDIR)sftfile.o \
			  $(SHAREDIR)list.o	\
			  $(SHAREDIR)ui.o	\
			  $(SHAREDIR)account.o \
			  $(SHAREDIR)ssl_wrap.o
ALL_OBJS = $(SERVER_OBJS) $(SHARED_OBJS)
CC = gcc
CFLAGS = -lssl -lcrypto -lsqlite3 -g -rdynamic $(INCPATH) 
TARGET = ssl_server
$(TARGET):$(ALL_OBJS)
	$(CC) -o $@ $(ALL_OBJS) $(CFLAGS)
clean:
	rm -rf *.o
	rm -rf $(TARGET)
