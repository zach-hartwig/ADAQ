/*
	-----------------------------------------------------------------------------

	               --- CAEN SpA - Computing Systems Division --- 

	-----------------------------------------------------------------------------

	Name		:	CONSOLE.c

	Description :	Console library for Linux & Microsoft VISUAL C++  
					(Win32 implementation).
					Provide a set of function which permit the console 
					screen management.

					Linux Specifications :

					Version:      Linux 1.0
					Platform:     Linux 2.4.x
					Language:     GCC 2.95 and 3.0

	Date		:	November 2004
	Release		:	1.0
	Author		:	C.Landi



	-----------------------------------------------------------------------------

	This file contains the following procedures and functions:               
                                                                          
	con_init         initialize the console                                  
	con_end          close the console                                       
	write_log        write a message into the log file                       
	con_getch        get a char from console without echoing                 
	con_kbhit        read a char from console without stopping the program   
	con_scanf        read formatted data from the console                    
	con_printf       print formatted output to the standard output stream    
	gotoxy           set the cursor position                                 
	con_printf_xy    print formatted output on the X,Y screen position    
    clrscr           clear the screen                                        
	clear_line       clear a line                                            
	delay            wait n milliseconds 
	
	-----------------------------------------------------------------------------
*/

#include "console.h"

#ifdef LINUX
	#include <ncurses.h>
	#include <unistd.h>
//	#include <stdlib.h>
#else
	#include <conio.h>
	#include <stdio.h>
	#include <stdarg.h>
	#include <windows.h>

static HANDLE ocon, icon;
#endif




/****************************************************************************/
/*                              GLOBAL VARIABLES                            */
/****************************************************************************/

/*------------------------------- log file ---------------------------------*/
FILE *log_file = NULL;                /* log file pointer */
char LOG_FILE_NAME[] = "LOG_FILE";    /* log file name */

/*------------------------------- console info -----------------------------*/

#ifndef LINUX
	BOOL t_type = TERM_NOT_INIT;
#endif


/****************************************************************************/
/*                              CON_INIT                                    */
/*--------------------------------------------------------------------------*/
/* Initialize the console                                                   */
/****************************************************************************/

void con_init()
{
#ifdef LINUX

  initscr();
  cbreak();
  noecho();
  nodelay(stdscr, FALSE);
  curs_set(FALSE);

#else

  COORD coordScreen = { 0, 0 };    /* home of the cursor */
  CONSOLE_SCREEN_BUFFER_INFO csbi; /* to get buffer info */  
  BOOL bSuccess;
  DWORD cCharsWritten;
  DWORD dwConSize; /* number of character cells in the current buffer */

 
  ocon = GetStdHandle(STD_OUTPUT_HANDLE); /* handle for the standard output */
  icon = GetStdHandle(STD_INPUT_HANDLE);  /* handle for the standard input */

  t_type =TERM_INIT;
  clrscr();  /* clear the screen */

  /* information about the specified console screen buffer*/
  bSuccess = GetConsoleScreenBufferInfo(ocon, &csbi);
  
  dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

  /* set the buffer's attributes */
  bSuccess = FillConsoleOutputAttribute(ocon, csbi.wAttributes,
             dwConSize, coordScreen, &cCharsWritten);

#endif
}


/****************************************************************************/
/*                              CON_END                                     */
/*--------------------------------------------------------------------------*/
/* Close the console                                                        */
/****************************************************************************/

void con_end()
{
#ifdef LINUX
  
  endwin();

#endif
}


/******************************************************************************/
/*                             WRITE_LOG                                      */
/*----------------------------------------------------------------------------*/
/* parameters:     msg        ->   log message text                           */
/*----------------------------------------------------------------------------*/
/* Write a messege in the log file                                            */
/******************************************************************************/

void write_log(char *msg)
{
  if(log_file == NULL)
    {
    log_file = fopen(LOG_FILE_NAME,"w");   /* open log file */
    if(log_file == NULL)
      {
      printf("\n\nCan't open log file\n\n");
      exit(0);
      }
    }
  fprintf(log_file,"%s\n",msg);   /* write the error message in the log file */
  fflush(stdout);                 /* empty the output buffer */
}


/******************************************************************************/
/*                               CON_GETCH                                    */
/*----------------------------------------------------------------------------*/
/* return:        c  ->  ascii code of pressed key                            */
/*----------------------------------------------------------------------------*/
/* Get a char from the console without echoing                                */
/* return the character read                                                  */
/******************************************************************************/

int  con_getch(void)
{
#ifdef LINUX

  int i;
  
  while( ( i = getch() ) == ERR );
  return i;

#else  
    
  return _getch(); 

#endif  
}


/******************************************************************************/
/*                               CON_KBHIT                                    */
/*----------------------------------------------------------------------------*/
/* return:        0  ->  no key pressed                                       */
/*                c  ->  ascii code of pressed key                            */
/*----------------------------------------------------------------------------*/
/* Read the standard input buffer; if it is empty, return 0 else read and     */
/* return the ascii code of the first char in the buffer. A call to this      */
/* function doesn't stop the program if the input buffer is empty.            */
/******************************************************************************/

char con_kbhit()
{
#ifdef LINUX

  int i, g;  
  
  nodelay(stdscr, TRUE);
  i = ( ( g = getch() ) == ERR ? 0 : g );
  nodelay(stdscr, FALSE);

  return i;   

#else
  
  char c=0;

  if(kbhit())            
    c = (char)con_getch();  
  return(c);

#endif
}


/******************************************************************************/
/*                               CON_SCANF                                    */
/*----------------------------------------------------------------------------*/
/* parameters:     fmt       ->   format-control string                       */
/*                 app       ->   argument                                    */
/*----------------------------------------------------------------------------*/
/* return:         number of fields that were successfully converted and      */
/*                 assigned (0  ->  no fields were assigned)                  */
/*----------------------------------------------------------------------------*/
/* Read formatted data from the console into the locations given by argument  */
/******************************************************************************/

int con_scanf(char *fmt, void *app)
{
#ifdef LINUX

  int i;

  echo();
  i = scanw(fmt,app);    
  refresh();
  noecho();
  return i;

#else

  int res;
      
  res = _cscanf(fmt, app) ;

  con_kbhit() ;                 /* Due to input reading problem */

  return res;

#endif
}


/******************************************************************************/
/*                               CON_PRINTF                                   */
/*----------------------------------------------------------------------------*/
/* parameters:     fmt       ->   format string: must contain specifications  */
/*                                that determine the output format for the    */
/*                                argument                                    */
/*----------------------------------------------------------------------------*/
/* return:         number of characters printed                               */
/*                 or a negative value if an error occurs                     */
/*----------------------------------------------------------------------------*/
/* Print formatted output to the standard output stream                       */ 
/******************************************************************************/

int con_printf(char *fmt,...)
{
#ifdef LINUX

  va_list marker;
  int     i;
	
  va_start(marker,fmt);
  i = vwprintw(stdscr,fmt,marker);
  va_end(marker);
  
  refresh();
  return i;

#else

  va_list marker;
  char buf[256];

  va_start(marker, fmt);       /* Initialize variable arguments. */
  vsprintf(buf,fmt,marker);
  va_end(marker);              /* Reset variable arguments.      */

  fflush(stdout);              /* Empty the output buffer */

  return _cprintf(buf);

#endif
}


/****************************************************************************/
/*                               GOTOXY                                     */
/*--------------------------------------------------------------------------*/
/* parameters:     x, y ->   position on the screen                         */ 
/*--------------------------------------------------------------------------*/
/* Place the cursor at position x,y on the screen                           */
/****************************************************************************/

void gotoxy(int x, int y)
{
#ifdef LINUX

  move(y-1, x-1);     
  refresh();

#else

  COORD coord;

  coord.X = x-1;
  coord.Y = y-1;
  SetConsoleCursorPosition(ocon,coord);

#endif
}


/****************************************************************************/
/*                             CON_PRINTF_XY                                */
/*--------------------------------------------------------------------------*/
/* parameters:     xpos, ypos ->   position on the screen                   */ 
/*                 msg        ->   message text                             */
/*--------------------------------------------------------------------------*/
/* return:         number of characters printed                             */
/*                 or a negative value if an error occurs                   */
/*--------------------------------------------------------------------------*/
/* Print formatted output on the X,Y screen position                        */
/****************************************************************************/

int con_printf_xy(uint xpos, uint ypos, char *fmt,...)
{
#ifdef LINUX

  va_list marker;
  int     i;

  move(ypos-1, xpos-1);     
  refresh();
	
  va_start(marker,fmt);
  i = vwprintw(stdscr,fmt,marker);
  va_end(marker);
  
  refresh();
  return i;

#else

  va_list marker;
  char buf[256];

  gotoxy(xpos, ypos);            /* Set the cursor position */
 
  va_start(marker, fmt);         /* Initialize variable arguments. */
  vsprintf(buf,fmt,marker); 
  va_end(marker);                /* Reset variable arguments.      */

  fflush(stdout);                /* Empty the output buffer */

  return _cprintf(buf); 

#endif
}


/****************************************************************************/
/*                               CLRSCR                                     */
/*--------------------------------------------------------------------------*/
/* Clear the screen                                                         */
/****************************************************************************/

void clrscr(void)
{
#ifdef LINUX

  clear();  
  move(0,0); 
  refresh();

#else

  COORD coordScreen = { 0, 0 };    /* home of the cursor */
  CONSOLE_SCREEN_BUFFER_INFO csbi; /* to get buffer info */
  BOOL bSuccess;
  DWORD cCharsWritten;
  DWORD dwConSize; /* number of character cells in the current buffer */

  /* information about the specified console screen buffer*/
  bSuccess = GetConsoleScreenBufferInfo(ocon, &csbi);
  
  dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
 
  /* fill the entire screen with blanks */
  bSuccess = FillConsoleOutputCharacter(ocon, (TCHAR) ' ',
             dwConSize, coordScreen, &cCharsWritten);
   
  /* set the buffer's attributes */
  bSuccess = FillConsoleOutputAttribute(ocon, csbi.wAttributes,
             dwConSize, coordScreen, &cCharsWritten);
    
  /* put the cursor at (0, 0) */
  bSuccess = SetConsoleCursorPosition(ocon, coordScreen);

#endif
}


/****************************************************************************/
/*                              CLEAR_LINE                                  */
/*--------------------------------------------------------------------------*/
/* parameters:     line ->   line to clear                                  */ 
/*--------------------------------------------------------------------------*/
/* Clear a line of the screen                                               */
/****************************************************************************/

void clear_line(uint line)
{
#ifdef LINUX
	
  gotoxy(1, line);
  clrtoeol();

#else

  COORD coordScreen;
  CONSOLE_SCREEN_BUFFER_INFO csbi; /* to get buffer info */
  BOOL bSuccess;
  DWORD cCharsWritten;
  DWORD dwLineSize; /* number of character cells in the line to clear */
  DWORD dwConSize; /* number of character cells in the current buffer */

  /* information about the specified console screen buffer*/
  bSuccess = GetConsoleScreenBufferInfo(ocon, &csbi);
  
  dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
  dwLineSize = csbi.dwSize.X;
 
  coordScreen.X=0;
  coordScreen.Y=line-1;

  /* fill the entire screen with blanks */
  bSuccess = FillConsoleOutputCharacter(ocon, (TCHAR) ' ',
             dwLineSize, coordScreen, &cCharsWritten);  

  coordScreen.X=0;
  coordScreen.Y=csbi.dwCursorPosition.Y;

  /* set the buffer's attributes */
  bSuccess = FillConsoleOutputAttribute(ocon, csbi.wAttributes,
             dwConSize, coordScreen, &cCharsWritten);

#endif
}


/****************************************************************************/
/*                             DELAY                                        */
/*--------------------------------------------------------------------------*/
/* parameters:     del        ->   delay in millisecond                     */ 
/*--------------------------------------------------------------------------*/
/* Wait msec milliseconds                                                   */
/****************************************************************************/

void delay(int msec)
{
#ifdef LINUX
  
  usleep(msec*1000); 
	
#else

  Sleep(msec);

#endif
}



