// CaenVMEUpgrade.c : Defines the entry point for the console application.
//
// 06/06/06 - Cambiato l'ordine dei parametri della Init. Link e BdNum erano
//            invertiti quindi non funzionava l'upgrade con piu' di una A2818.

#ifdef LINUX
	#include <inttypes.h>
	#include <sys/time.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <string.h>
#include <stdarg.h>
#include "CAENVMElib.h"
#include <time.h>
#include <a3818.h>
#include "p30.h"
#include "A3818Upgrade.h"

// Define FIRMWARE regions into flash by using their start address (116-bit word address)
#define FIRST_FIRMWARE_PAGE_BASE_WORD_ADDRESS    0x000000
#define SECOND_FIRMWARE_PAGE_BASE_WORD_ADDRESS   0x200000
#define THIRD_FIRMWARE_PAGE_BASE_WORD_ADDRESS    0x400000
#define FOURTH_FIRMWARE_PAGE_BASE_WORD_ADDRESS   0x600000


/* get time in milliseconds */
static long get_time()
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

int A3818UpgradeFromFile(int32_t A3818_handle, FILE * binfile, int fwcopy, A3818Upgrade_Mode mode) {
  
        uint32_t baseAddress;
        long currentTime, elapsedTime;        
        int finish;
        unsigned char c, c1;
        uint32_t      bp;        
        uint32_t      bufferLength;      

        uint32_t      *buffer;
        unsigned int  verifyErrors = 0;
        buffer = malloc(BITSTREAM_BYTES * sizeof(uint32_t));
        switch(fwcopy) {
          case 0 :
             baseAddress =  FIRST_FIRMWARE_PAGE_BASE_WORD_ADDRESS;
             break;
          case 1 :
             baseAddress =  SECOND_FIRMWARE_PAGE_BASE_WORD_ADDRESS;
             break;
          case 2 :
             baseAddress =  THIRD_FIRMWARE_PAGE_BASE_WORD_ADDRESS;
             break;
          case 3 :
             baseAddress =  FOURTH_FIRMWARE_PAGE_BASE_WORD_ADDRESS;
             break;
          default :
             baseAddress =  SECOND_FIRMWARE_PAGE_BASE_WORD_ADDRESS;     
             break;
                  
        }
        
        bp = 0;
        finish = 0;
        
        // Carica il bitsream in un buffer di memoria
        currentTime = get_time();  
        while( !finish ) {
               
           c = (unsigned char)fgetc(binfile);  // read one byte from file
           c1 = (unsigned char)fgetc(binfile);  // read one byte from file (Strataflash a 16 bit)
      
#ifdef __SWAP__                
           swap = 0;
                
           // Swap primo byte
           for( i = 0; i < 8; i++ )
             if( c & (1 << i) )
                swap = swap | (0x80 >> i);
                
           swap1 = 0;        
           // Swap secondo byte        
           for( i = 0; i < 8; i++ )
             if( c1 & (1 << i) )
               swap1 = swap1 | (0x80 >> i);
                       
           buffer[bp] = (uint32_t) ((swap1 <<8) | swap); // HACK : swap o non swap?
#else               
           buffer[bp] = (uint32_t) ((c1 <<8) | c);
#endif // __SWAP__   

           bp++;
           if( feof(binfile) )
             finish = 1;
        }  // end of while loop     

        bufferLength = (--bp);
        
        if ((bufferLength*2) != BITSTREAM_BYTES) {
           printf("\nERROR: Input BIN file length (%d bytes) is different than expected bitstream size (%d bytes). Exiting.....\n", bufferLength*2, BITSTREAM_BYTES);
           free(buffer);
           return -1;
        }
        
        elapsedTime = get_time() - currentTime;
        printf("\nBitstream (%d bytes) loaded in %ld milliseconds\n", bufferLength*2, elapsedTime);
        
        A3818_EnableBPIAccess(A3818_handle);     

        /* Cancellazione della zona di flash riservata al firmware */
        if ( (mode == A3818_UPGRADE_FULL      ) ||
             (mode == A3818_UPGRADE_ERASE_ONLY) ) {
            printf("Erasing flash ");
            fflush(stdout);            
            currentTime = get_time();         
            eraseFirmware(A3818_handle, baseAddress, fwcopy);
            elapsedTime = get_time() - currentTime;
            printf("\nFlash erased in %ld milliseconds\n", elapsedTime);
        }
        
        /* Programmazione immagine firmware in flash */
        if ( (mode == A3818_UPGRADE_FULL      ) ) {
            currentTime = get_time();      
            writeFlash(A3818_handle, buffer, bufferLength, baseAddress);
            elapsedTime = get_time() - currentTime;
            printf("\n%d 16-bit words programmed in %ld milliseconds\n", bufferLength, elapsedTime);
        }
	    
        /* Verifica immagine firmware in flash */
        if ( (mode == A3818_UPGRADE_FULL      ) ||
             (mode == A3818_UPGRADE_VERIFY_ONLY) ) {
            currentTime = get_time();        
            verifyErrors = verifyFlash(A3818_handle, buffer, bufferLength, baseAddress);
            elapsedTime = get_time() - currentTime;
            printf("\n%d 16-bit words verified in %ld milliseconds\n", bufferLength, elapsedTime);
        }
        
        A3818_EnableSPIAccess(A3818_handle);      
        

        if( verifyErrors > 0 ) {
                printf("\n\n%d errors found during verify!\n",verifyErrors);
        } else {
                printf("\n\nFirmware updated without errors. Written %d words\n",bufferLength);
        }
        free(buffer);
        return 0;
}


int A3818UpgradeFromMem(int32_t A3818_handle, char* fwdata, int fwsize, int fwcopy, A3818Upgrade_Mode mode) {
		int fi = 0;
        uint32_t baseAddress;
        long currentTime, elapsedTime;        
        int finish;
        unsigned char c, c1;
        uint32_t      bp;        
        uint32_t      bufferLength;      

        uint32_t      *buffer;
        unsigned int  verifyErrors = 0;
        buffer = malloc(BITSTREAM_BYTES * sizeof(uint32_t));
        switch(fwcopy) {
          case 0 :
             baseAddress =  FIRST_FIRMWARE_PAGE_BASE_WORD_ADDRESS;
             break;
          case 1 :
             baseAddress =  SECOND_FIRMWARE_PAGE_BASE_WORD_ADDRESS;
             break;
          case 2 :
             baseAddress =  THIRD_FIRMWARE_PAGE_BASE_WORD_ADDRESS;
             break;
          case 3 :
             baseAddress =  FOURTH_FIRMWARE_PAGE_BASE_WORD_ADDRESS;
             break;
          default :
             baseAddress =  SECOND_FIRMWARE_PAGE_BASE_WORD_ADDRESS;     
             break;
                  
        }
        
        bp = 0;
        finish = 0;
        
        // Carica il bitsream in un buffer di memoria
        currentTime = get_time();  
        while( !finish ) {
               
           c = (unsigned char)fwdata[fi++];  // read one byte from file
           c1 = (unsigned char)fwdata[fi++];  // read one byte from file (Strataflash a 16 bit)
      
#ifdef __SWAP__                
           swap = 0;
                
           // Swap primo byte
           for( i = 0; i < 8; i++ )
             if( c & (1 << i) )
                swap = swap | (0x80 >> i);
                
           swap1 = 0;        
           // Swap secondo byte        
           for( i = 0; i < 8; i++ )
             if( c1 & (1 << i) )
               swap1 = swap1 | (0x80 >> i);
                       
           buffer[bp] = (uint32_t) ((swap1 <<8) | swap); // HACK : swap o non swap?
#else               
           buffer[bp] = (uint32_t) ((c1 <<8) | c);
#endif // __SWAP__   

           bp++;
           if( fi >= fwsize )
             finish = 1;
        }  // end of while loop     

        bufferLength = (--bp);
        
        if ((bufferLength*2) != BITSTREAM_BYTES) {
           printf("\nERROR: Input BIN file length (%d bytes) is different than expected bitstream size (%d bytes). Exiting.....\n", bufferLength*2, BITSTREAM_BYTES);
           free(buffer);
           return -1;
        }
        
        elapsedTime = get_time() - currentTime;
        printf("\nBitstream (%d bytes) loaded in %ld milliseconds\n", bufferLength*2, elapsedTime);
        
        A3818_EnableBPIAccess(A3818_handle);     

        /* Cancellazione della zona di flash riservata al firmware */
        if ( (mode == A3818_UPGRADE_FULL      ) ||
             (mode == A3818_UPGRADE_ERASE_ONLY) ) {
            printf("Erasing flash ");
            fflush(stdout);            
            currentTime = get_time();         
            eraseFirmware(A3818_handle, baseAddress, fwcopy);
            elapsedTime = get_time() - currentTime;
            printf("\nFlash erased in %ld milliseconds\n", elapsedTime);
        }
        
        /* Programmazione immagine firmware in flash */
        if ( (mode == A3818_UPGRADE_FULL      ) ) {
            currentTime = get_time();      
            writeFlash(A3818_handle, buffer, bufferLength, baseAddress);
            elapsedTime = get_time() - currentTime;
            printf("\n%d 16-bit words programmed in %ld milliseconds\n", bufferLength, elapsedTime);
        }
	    
        /* Verifica immagine firmware in flash */
        if ( (mode == A3818_UPGRADE_FULL      ) ||
             (mode == A3818_UPGRADE_VERIFY_ONLY) ) {
            currentTime = get_time();        
            verifyErrors = verifyFlash(A3818_handle, buffer, bufferLength, baseAddress);
            elapsedTime = get_time() - currentTime;
            printf("\n%d 16-bit words verified in %ld milliseconds\n", bufferLength, elapsedTime);
        }
        
        A3818_EnableSPIAccess(A3818_handle);      
        

        if( verifyErrors > 0 ) {
                printf("\n\n%d errors found during verify!\n",verifyErrors);
        } else {
                printf("\n\nFirmware updated without errors. Written %d words\n",bufferLength);
        }
        free(buffer);
        return 0;
}

