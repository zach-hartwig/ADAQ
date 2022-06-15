/*
kbhit() and getch() for Linux/UNIX
Chris Giese <geezer@execpc.com>	http://my.execpc.com/~geezer
*/

#ifndef READOUT_TEST_KEYB
#define READOUT_TEST_KEYB

#ifdef linux
    #include <sys/time.h> /* struct timeval, select() */
    #include <termios.h> /* tcgetattr(), tcsetattr() */
    #include <stdlib.h> /* atexit(), exit() */
    #include <unistd.h> /* read() */
    #include <stdio.h> /* printf() */
    #include <string.h> /* memcpy() */
	
	#define CLEARSCR "clear"

/*****************************************************************************/
/*  SLEEP  */
/*****************************************************************************/
void Sleep(int t);

/*****************************************************************************/
/*  GETCH  */
/*****************************************************************************/
int getch(void);

/*****************************************************************************/
/*  KBHIT  */
/*****************************************************************************/
int kbhit();


#else  // Windows

    #include <conio.h>
	
	#define CLEARSCR "cls"
#endif

#endif