/*
 * ========================================================================
 *
 *       Filename:  ui.c
 *
 *    Description:  Design the user interface. 
 *
 *        Version:  1.0
 *           Date:  04/12/14 09:01:14 PM
 *       Compiler:  gcc-4.6.3
 *
 *         Author:  Populus Huang 
 *          Email:  thoughtsonlive@gmail.com
 *
 * ========================================================================
 */

#include "ui.h"
#include <stdio.h>
/* print the logo */
void logo_ui()
{                
printf(PURPLE_BG"+==================================================================+"NONE"\n");           
printf(PURPLE_BG"|                                              "YELLOW"    < hello >       "NONE PURPLE_BG"|\n");           
printf(PURPLE_BG"|   "GREEN"+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+"YELLOW" -----         "NONE PURPLE_BG"|\n");      
printf(PURPLE_BG"|   "GREEN"|     _________________    ______  ______      |"YELLOW"  \\   .--.     "NONE PURPLE_BG"|\n"); 
printf(PURPLE_BG"|   "GREEN"|    / __/ __/_  __/ _ \\  / __/\\ \\/ / __/      |"YELLOW"   \\ |o_o |    "NONE PURPLE_BG"|\n"); 
printf(PURPLE_BG"|   "GREEN"|   _\\ \\/ _/  / / / ___/ _\\ \\   \\  /\\ \\        |"YELLOW"     |:_/ |    "NONE PURPLE_BG"|\n"); 
printf(PURPLE_BG"|   "GREEN"|  /___/_/   /_/ /_/    /___/   /_/___/        |"YELLOW"    //   \\ \\   "NONE PURPLE_BG"|\n");      
printf(PURPLE_BG"|   "GREEN"|                                              |"YELLOW"   (|     | )  "NONE PURPLE_BG"|\n");	
printf(PURPLE_BG"|   "GREEN"+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+"YELLOW"  /'\\_   _/`\\  "NONE PURPLE_BG"|\n");	
printf(PURPLE_BG"|                                                    "YELLOW" \\___)=(___/  "NONE PURPLE_BG"|"NONE"\n");
} 
void divline_ui()
{
printf(GREEN"+------------------------------------------------------------------+"NONE"\n");           
}
void start_ui()
{
printf(GREEN_BG"|                            Start                                 |"NONE"\n");           
divline_ui();
printf(GREEN"|           [1]Login                       [2]Register             |"NONE"\n");
divline_ui();
}
void login_ui()
{
system("clear");
logo_ui();
printf(GREEN_BG"|                            Login                                 |"NONE"\n");           
divline_ui();
}
void register_ui()
{
system("clear");
logo_ui();
printf(GREEN_BG"|                           Register                              |"NONE"\n");           
divline_ui();
}
void modify_passwd_ui()
{
system("clear");
logo_ui();
printf(GREEN_BG"|                       Modify Password                           |"NONE"\n");           
divline_ui();
}
void primary_ui()
{
printf(GREEN_BG"|                            Primary                               |"NONE"\n");           
printf(GREEN"+------------------------------------------------------------------+"NONE"\n");           
printf(GREEN"|   [1]List File Detail    [2]Upload            [3]Download        |"NONE"\n");
printf(GREEN"|   [4]Modify Passwod      [5]Console           [6]Exit            |"NONE"\n");
printf(GREEN"+------------------------------------------------------------------+"NONE"\n");           
}
void list_ui()
{
system("clear");
logo_ui();
printf(GREEN_BG"|                             List                                 |"NONE"\n");           
printf(GREEN"+------------------------------------------------------------------+"NONE"\n");           
printf(GREEN"|         [1]List Server   [2]List Client    [3]List Both          |"NONE"\n");
printf(GREEN"+------------------------------------------------------------------+"NONE"\n");           
}
void upload_ui() 
{
system("clear");
logo_ui();
printf(GREEN_BG"|                             Upload                              |"NONE"\n");           
divline_ui();
}
void download_ui()
{
system("clear");
logo_ui();
printf(GREEN_BG"|                           Download                              |"NONE"\n");           
divline_ui();
}
void console_ui()
{
printf(GREEN_BG"|                           Console                               |"NONE"\n");           
divline_ui();
}
