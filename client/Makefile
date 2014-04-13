OBJS = client.o \
     	  clnt_parse.o \
     	  echo_mode.o \
     	  command.o \
     	  console.o \
     	  menu.o 
LIBPATH = $(shell pwd)/../lib/
INCPATH = -I../include
CC = gcc
CFLAGS = -lssl -lcrypto -lsqlite3 -lreadline -g $(INCPATH) 
LDCFLAGS=-L$(LIBPATH)  -lsftpsys 
TARGET = ssl_client
$(TARGET):$(OBJS) 
	#Remain LDCFLAGS is before CFLAGS
	$(CC) -o $@  $(OBJS)  $(LDCFLAGS) $(CFLAGS)
clean:
	rm -rf $(OBJS) 
	rm -rf $(TARGET)
