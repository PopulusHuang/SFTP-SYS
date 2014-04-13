# SFTP-SYS
--------
SFTP-SYS is a Secure File Transfer System written in C and running on Linux.This system uses 
C/S model.It is based on SSL protocol and using the openssl library.The 
Server uses epoll I/O multiplexing to implement concurrent processing.
The Client can communicate with the Server via a simple user interface.  
Features
--------
1. Support user register and login.
2. Support files encrypted transmission.
3. Support client explore,upload and download files.
4. Multiple concurrent processing a large number of connections. 
5. Support menu UI and command line two modes of operations. 

Requirements
--------
In order to run the SFTP-SYS,the following program are needed:

1. openssl
2. sqlite3
3. readline

on Ubuntu,you can use ``sudo apt-get install`` command to install them.
You can also install from source code.Download the source package and use the following command to install them.

		$ ./configure 
		$ make 
		$ make install 

Usage
-----

1. Entry the *SFTP-SYS/*(root directory)and execute the ``make`` command.Then it will generate
   **libsftpsys.a**, **ssl_server**, **ssl_client** in *lib/,server/,client/*. 
2. Entry the *server/* directory and edit **sftp-sys.conf**,such as the following:

		#Sever network configure 
		
		# Server will use the localhost IP when 'INADDR_ANY' is on
		
		INADDR_ANY = off 
		
		# Assign a specify IP,and make sure the 'INADDR_ANY' is off.
		
		IP = 127.0.0.1
		PORT = 7838
		# Connection backlog
		
		LISTEN = 1000 
  save the configure,and run ``./ssl_server &`` 
3. Entry the *client/* directory and run ``./ssl_client 127.0.0.1 7838``. 
The default user and password are "**Populus**","**123456**".You can register
a account by yourself.

The are some files in the *SFTP_SYS/resource/* directory,you can use them for test.
Certificate
-----
Entry the *server/certificate/* directory,you will see two files:**privkey.pem**
(private key),**cacert.pem**(digital certificate).You can go back to the *server/* ,run ``./create_cert.sh`` to generate those files and copy to here.  

User Directory
-----
The Server will generate a user directory under the *server/User/* directory 
which according to the user name.This directory is the user space.User can only
explore,upload and download files with this space.

On client,there a *SFT_DOWNLOAD/* directory which use to receive the download files from the server.
