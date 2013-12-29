#ifndef _CLIENT_H_
#define _CLIENT_H_

void ShowCerts(SSL * ssl);
void extract_name(const char *file_with_path, char *fetch_name);
int open_file(char *fileName);
void send_file(SSL *ssl,int fd);
int send_filename(SSL *ssl,char *fileName,char *sendFN);
//void show_menu();

#endif
