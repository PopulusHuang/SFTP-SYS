OBJS = server.o \
			  srv_handle.o \
			  myepoll.o \
			  sockssl.o \
			  srv_parse.o \
			  read_cfg.o
LIBPATH = $(shell pwd)/../lib/
INCPATH = -I../include
CC = gcc
CFLAGS = -lssl -lcrypto -lsqlite3 -g -rdynamic $(INCPATH) 
LDFLAGS = -L$(LIBPATH) -lsftpsys
TARGET = ssl_server
$(TARGET):$(OBJS)
	$(CC) -o $@ $(OBJS) $(CFLAGS) $(LDFLAGS) 
clean:
	rm -rf $(OBJS)
	rm -rf $(TARGET)
