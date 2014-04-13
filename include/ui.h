/* ui.h */
#ifndef _UI_H_
#define _UI_H_
/* force color */
#define NONE         "\033[0m"
#define RED          "\033[1;31m"
#define LIGHT_RED    "\033[1;31m"
#define GREEN        "\033[1;32m"
#define BLUE         "\033[0;34m"
#define LIGHT_BLUE   "\033[1;34m"
#define DARY_GRAY    "\033[1;30m"
#define CYAN         "\033[0;36m"
#define LIGHT_CYAN   "\033[1;36m"
#define PURPLE       "\033[0;35m"
#define LIGHT_PURPLE "\033[1;35m"
#define BROWN        "\033[0;33m"
#define YELLOW       "\033[1;33m"
#define LIGHT_GRAY   "\033[0;37m"
#define WHITE        "\033[1;37m"
/* background color */
#define BLACK_BG	 "\033[0;40m"
#define WHITE_BG	 "\033[0;47m"
#define PURPLE_BG	 "\033[0;45m"
#define YELLOW_BG	 "\033[0;43m"
#define GREEN_BG	 "\033[0;42m"
/* control attribute */
#define CURSOR_HIDE	 "\033[?25l"
#define CURSOR_SHOW	 "\33[?25h"
#define CLEAR		 "\033[2J"
#define UNDERLINE	 "\03[4m"
void console_ui();
void download_ui();
void list_ui();
void login_ui();
void logo_ui();
void primary_ui();
void register_ui();
void start_ui();
void upload_ui() ;
void modify_passwd_ui();
#endif
