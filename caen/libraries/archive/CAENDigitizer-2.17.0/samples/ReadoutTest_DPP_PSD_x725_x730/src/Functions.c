/******************************************************************************
*
* CAEN SpA - Front End Division
* Via Vetraia, 11 - 55049 - Viareggio ITALY
* +390594388398 - www.caen.it
*
***************************************************************************//**
* \note TERMS OF USE:
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the Free Software
* Foundation. This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. The user relies on the
* software, documentation and results solely at his own risk.
******************************************************************************/

#include "keyb.h"
#include "Functions.h"

#include <stdio.h>
#ifdef WIN32

    #include <time.h>
    #include <sys/timeb.h>
    #include <conio.h>
    #include <process.h>
	#define getch _getch     /* redefine POSIX 'deprecated' */
	#define kbhit _kbhit     /* redefine POSIX 'deprecated' */

#else
    #include <unistd.h>
    #include <stdint.h>   /* C99 compliant compilers: uint64_t */
    #include <ctype.h>    /* toupper() */
    #include <sys/time.h>
#endif

/* ###########################################################################
*  Functions
*  ########################################################################### */
/*! \fn      static long get_time()
*   \brief   Get time in milliseconds
*   \return  time in msec */ 
long get_time()
{
    long time_ms;
#ifdef WIN32
    struct _timeb timebuffer;
    _ftime( &timebuffer );
    time_ms = (long)timebuffer.time * 1000 + (long)timebuffer.millitm;
#else
    struct timeval t1;
    struct timezone tz;
    gettimeofday(&t1, &tz);
    time_ms = (t1.tv_sec) * 1000 + t1.tv_usec / 1000;
#endif
    return time_ms;
}

/* --------------------------------------------------------------------------------------------------------- */
/*! \fn      int DataConsistencyCheck(uint32_t *buff32, int NumWords)
*   \brief   Do some data consistency check
*   \return  0=success; -1=error */
/* --------------------------------------------------------------------------------------------------------- */
int DataConsistencyCheck(uint32_t *buff32, int NumWords)
{
    int i, zcnt=0, pnt=0;
    uint32_t EventSize;

    if (NumWords == 0)
        return 0;

    // Check for events integrity
    do {
        EventSize = buff32[pnt] & 0x0FFFFFFF;
        pnt += EventSize;  // Jump to next event
    } while (pnt<NumWords);
    if (pnt != NumWords) {
        printf("Data Error: Event truncation\n");
        return -1;
    }

    // Check for burst of zeroes (more than 2 consecutive zeroes)
    for(i=0; i<NumWords; i++) {
        if (buff32[i] == 0)   zcnt++;
        else                  zcnt=0;
        if (zcnt > 2) {
            printf("Data Error: Burst of zeroes\n");
            return -1;
        }
    }
    return 0;
}


/* --------------------------------------------------------------------------------------------------------- */
/*! \fn      SaveHistograms(uint32_t EHisto[8][1<<MAXNBITS])
*   \brief   Save Histograms to output files
*   \return  0=success; -1=error */
/* --------------------------------------------------------------------------------------------------------- */

int SaveHistogram(char *basename, int b, int ch, uint32_t *EHisto)
{
	/*
	This function saves the first 16384 bin of each EHisto[ch] array in separate text files
	each array value is a number representing the histogram height in the corresponding bin
	NB: each EHisto[ch] must be a pointer already initialized with at least 16384 values
	*/
    FILE *fh;
    int i;
    char filename[20];
    sprintf(filename, "%s_%d_%d.txt", basename, b, ch);
    fh = fopen(filename, "w");
    if (fh == NULL)
		return -1;
    for(i=0; i<(1<<14); i++) {
		fprintf(fh, "%d\n", EHisto[i]);
	}
    fclose(fh);
    printf("Histograms saved to '%s_<board>_<channel>.txt'\n", basename);

    return 0;
}

/* --------------------------------------------------------------------------------------------------------- */
/*! \fn      SaveWaveforms(int b, int ch, CAEN_DGTZ_DPP_TF2_Waveforms_t *Waveforms)
*   \brief   Save Waveforms to output files
*   \return  0=success; -1=error */
/* --------------------------------------------------------------------------------------------------------- */
int SaveWaveform(int b, int ch, int trace, int size, int16_t *WaveData)
{
	/*
	This function saves the waveform in a textfile as a sequence of number representing the wave height
	*/
    FILE *fh;
    int i;
    char filename[20];

    sprintf(filename, "Waveform_%d_%d_%d.txt", b, ch, trace);
    fh = fopen(filename, "w");
    if (fh == NULL)
        return -1;
    for(i=0; i<size; i++)
        fprintf(fh, "%d\n", WaveData[i]); //&((1<<MAXNBITS)-1)
    fclose(fh);
    return 0;
}

/* --------------------------------------------------------------------------------------------------------- */
/*! \fn      SaveWaveforms(int b, int ch, CAEN_DGTZ_DPP_TF2_Waveforms_t *Waveforms)
*   \brief   Save Waveforms to output files
*   \return  0=success; -1=error */
/* --------------------------------------------------------------------------------------------------------- */
int SaveDigitalProbe(int b, int ch, int trace, int size, uint8_t *WaveData)
{
	/*
	This function saves the digital waveform in a textfile as a sequence of number representing the wave height
	*/
    FILE *fh;
    int i;
    char filename[20];

    sprintf(filename, "DWaveform_%d_%d_%d.txt", b, ch, trace);
    fh = fopen(filename, "w");
    if (fh == NULL)
        return -1;
    for(i=0; i<size; i++)
        fprintf(fh, "%d\n", WaveData[i]); //&((1<<MAXNBITS)-1)
    fclose(fh);
    return 0;
}

/* --------------------------------------------------------------------------------------------------------- */
/*! \fn      PrintInterface()
*   \brief   Print the interface to screen
*   \return  none
/* --------------------------------------------------------------------------------------------------------- */
void PrintInterface() {
	printf("\ns ) Start acquisition\n");
	printf("S ) Stop acquisition\n");
	printf("r ) Restart acquisition\n");
	printf("T ) Read ADC temperature\n");
	printf("C ) Start ADC calibration\n");
	printf("q ) Quit\n");
	printf("t ) Send a software trigger\n");
	printf("h ) Save Histograms to file\n");
	printf("w ) Save waveforms to file\n\n\n");
}
