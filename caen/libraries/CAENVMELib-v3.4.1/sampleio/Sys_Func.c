/******************************************************************************
*
*	CAEN SpA - ATM Division
*	Via Vetraia, 11 - 55049 - Viareggio ITALY
*	+39 0594 388 398 - www.caen.it
*
***************************************************************************//*!
*
*	\file		Sys_Func.c
*	\brief
*	\author		Matteo Bianchini
*	\version
*	\date		2022
*	\copyright	GNU Lesser General Public License
*
******************************************************************************/

#include <stdlib.h>

#ifdef _WIN32
#include <Windows.h>
#include <conio.h>
#else 
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#endif


void clr_scr() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}


void Sleep_(int t) {
#ifdef _WIN32
    Sleep(t);
#else
    usleep(t * 1000);
#endif
}


int kbhit_(void) {
#ifdef _WIN32
    return _kbhit();
#else
    struct termios oldattr;
    if (tcgetattr(STDIN_FILENO, &oldattr) == -1) perror(NULL);
    struct termios newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);
    newattr.c_cc[VTIME] = 0;
    newattr.c_cc[VMIN] = 1;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &newattr) == -1) perror(NULL);
    /* check stdin (fd 0) for activity */
    fd_set read_handles;
    FD_ZERO(&read_handles);
    FD_SET(0, &read_handles);
    struct timeval timeout;
    timeout.tv_sec = timeout.tv_usec = 0;
    int status = select(0 + 1, &read_handles, NULL, NULL, &timeout);
    if (status < 0) perror(NULL);
    if (tcsetattr(STDIN_FILENO, TCSANOW, &oldattr) == -1) perror(NULL);
    return status;
#endif
}


int getch_() {
    int key;
#ifdef _WIN32
    key = _getch();
#else
    struct termios oldattr;
    if (tcgetattr(STDIN_FILENO, &oldattr) == -1) perror(NULL);
    struct termios newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSANOW, &newattr) == -1) perror(NULL);
    key = getchar();
    if (tcsetattr(STDIN_FILENO, TCSANOW, &oldattr) == -1) perror(NULL);
#endif
    return key;
}