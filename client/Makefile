CLIENT_OBJS = client.o clnt_parse.o  echo_mode.o menu.o console.o command.o
SHAREDIR = ../share/
INCPATH = -I$(SHAREDIR)
SHARED_OBJS = $(SHAREDIR)sock_wrap.o \
			  $(SHAREDIR)sftpack.o \
			  $(SHAREDIR)sftfile.o \
			  $(SHAREDIR)list.o	\
			  $(SHAREDIR)ui.o	\
			  $(SHAREDIR)account.o  
ALL_OBJS = $(CLIENT_OBJS) $(SHARED_OBJS)
CC = gcc
CFLAGS = -lssl -lcrypto -lsqlite3 -lreadline -g $(INCPATH) 
TARGET = ssl_client
$(TARGET):$(ALL_OBJS) 
	$(CC) -o $@  $(ALL_OBJS) $(CFLAGS) 
clean:
	rm -rf $(CLIENT_OBJS) $(SHARED_OBJS)
	rm -rf $(TARGET)
