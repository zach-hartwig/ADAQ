
//*********************************************************************
//                                                                     
//                ------    C. A. E. N.   S.p.A.    ------             
//                                                                     
//   Nome        : A639Dwld.c                                        
//   Progetto    : A639DownLoad      
//                                                                     
//   Descrizione : Programma per il download del firmware di gestione
//                 del modulo A639 (DT5780 HV controller) all'interno
//                 della Flash del uC.
//                 Si interfaccia con il codice di Boot del uC.
//                                                                     
//   Data        : Dicembre 2012                                       
//   Release     : 1.0                                                 
//   Autore      : C.Landi                                             
//
//*********************************************************************

#include<stdlib.h>
#include<stdio.h>

#include<CAENComm.h>

//------------------------
// Indirizzo Board Info 
//------------------------

#define	BD_INFO_ADDR		(UINT32)0x8140

#define DT5780				0x7							// ID DT5580

#define DT55XX				0xA							// ID DT55XX
#ifdef LINUX
#define UINT32		unsigned int
#define UINT16		unsigned short

void Sleep( int x ) {
 usleep( 1000 * x);
} 
#endif

//------------------------

// #define per sincronizzazione con Boot uC.

#define	OK					(UINT32)0xf0
#define	STARTDLD			(UINT32)0xf1
#define	DATAREADY			(UINT32)0xf2
#define	FERASE				(UINT32)0xf3
#define	FUPDATEL			(UINT32)0x55
#define	FUPDATEH			(UINT32)0xaa

#define	CR					0xd

//*****************************************************************

unsigned char	flash_buff[0x7800] ;			// Array delle dimensioni della memoria del uC 
												// riservata al codice di gestione del modulo.
												// Da 0x7800 inizia il codice di Boot.

UINT32	HV_VSET0_ADDR,HV_ISET0_ADDR,HV_RUP0_ADDR,HV_RDWN0_ADDR ;
UINT32	HV_VMAX0_ADDR,HV_VMON0_ADDR,HV_IMON0_ADDR,HV_CTRL0_ADDR,HV_STATUS0_ADDR ;

UINT32	HV_VSET1_ADDR,HV_ISET1_ADDR,HV_RUP1_ADDR,HV_RDWN1_ADDR ;
UINT32	HV_VMAX1_ADDR,HV_VMON1_ADDR,HV_IMON1_ADDR,HV_CTRL1_ADDR,HV_STATUS1_ADDR ;

//*****************************************************************

//*****************************************************************
// Nome        : ReadFileHex.
// 
// Descrizione : La procedura legge il file tipo Hex ('a639.xxx' o
//				 'DT55XX.xxx') e riempie l'array 'FlashBuff[]' con
//				 i valori convertiti in binario. 
//				 L'array 'FlashBuff[]' viene inizializzato con il
//				 valore 0xff (byte erased).
//
// Input       : Nessuno.
// Output      : 0 -> Ok, -1 -> problemi
// Globali     : FlashBuff[]
// Release     : 1.0.
// 
//*****************************************************************

int ReadFileHex(char *fname) 
{
FILE			*f;
char			record[80] ;
unsigned char	buff[80],dato,ndata,rectype,checksum ;
long				i,j,numrec=0 ;
unsigned long	addr ;

if ((f = fopen(fname, "rb")) == NULL)
	{
	printf("\n\n !!! File not found. Press any key to exit ... ");
	return -1 ;
	}

for (i=0; i<sizeof(flash_buff); i++)
	flash_buff[i] = 0xff ;

for (;;)
	{
	if (fgets(record,80,f) == NULL)
		break ;

	numrec++ ;
	checksum = 0 ;
	buff[0] = record[0] ;
	i = 1 ;
	j = 1 ;
	while (record[i] != CR)
		{
		if (record[i] >= 'A')
			dato = record[i] - 55 ;
		else
			dato = record[i] - '0' ;
		dato = dato << 4 ;

		if (record[i+1] >= 'A')
			dato |= record[i+1] - 55 ;
		else
			dato |= record[i+1] - '0' ;

		buff[j++] = dato ;
		i += 2;
		checksum += dato ;
		}

	if ((record[0] != ':') || checksum)
		{
		printf("\n\n !!! Invalid record Format.");
		return -1 ;
		}
	else
		{
		ndata = buff[1] ;
		rectype = buff[4] ;
		addr = ((unsigned long)buff[2] << 8) | (unsigned long)buff[3] ;
		rectype = buff[4] ;
		
		if (rectype == 0)							// Record di dati
			{
			for (i=0; i<ndata; i++)
				flash_buff[addr++] = buff[i+5] ;
			}
		}
	}

fclose(f);

return 0 ;
}


//*****************************************************************
// Nome        : FirmwareUpgrade
// 
// Descrizione : La procedura gestisce l'aggiornamento del 
//				 firmware sulla A639(A) interagendo con il  
//				 Boot. 
//				 L'array 'FlashBuff[]' viene inizializzato con il
//				 valore 0xff (byte erased).
//
// Input       : Nessuno.
// Output      : 0 -> Ok, -1 -> problemi
// Globali     : FlashBuff[]
// Release     : 1.0.
// 
//*****************************************************************

void FirmwareUpgrade(int handle)
{
UINT16 d16,i ;
int res;


// Sequenza per trasferimento dati

CAENComm_Write16(handle, HV_CTRL1_ADDR, STARTDLD) ;			// Segnalo Start Download

do
	CAENComm_Read16(handle, HV_STATUS0_ADDR, &d16) ;		// Aspetto segnalazione di Flash Erasing
while (d16 != FERASE) ;

printf ("\n\n Erasing ... ") ;
CAENComm_Write16(handle, HV_CTRL1_ADDR, 0x0) ;				// Segnalo Start Download
do
	CAENComm_Read16(handle, HV_STATUS0_ADDR, &d16) ;			// Aspetto Ok per partire
while (d16 == FERASE) ;

printf ("\n\n Programming ...\n\n") ;

CAENComm_Write16(handle, HV_CTRL0_ADDR, 0) ;					// Reset segnalazioni
CAENComm_Write16(handle, HV_CTRL1_ADDR, 0) ;

for (i=0; i<sizeof(flash_buff); i+=8)

	{
	if (i == 0)
		printf (" *") ;
	else
		if ((i % 640) == 0)
			printf ("*") ;


	d16 = ((UINT16)flash_buff[i+1] << 8) | (UINT16)flash_buff[i] ;		// Riempio area dati
	CAENComm_Write16(handle, HV_VSET0_ADDR, d16) ;
		
	d16 = ((UINT16)flash_buff[i+3] << 8) | (UINT16)flash_buff[i+2] ;
	CAENComm_Write16(handle, HV_ISET0_ADDR, d16) ;

	d16 = ((UINT16)flash_buff[i+5] << 8) | (UINT16)flash_buff[i+4] ;
	CAENComm_Write16(handle, HV_VSET1_ADDR, d16) ;
		
	d16 = ((UINT16)flash_buff[i+7] << 8) | (UINT16)flash_buff[i+6] ;
	CAENComm_Write16(handle, HV_ISET1_ADDR, d16) ;

	CAENComm_Write16(handle, HV_CTRL0_ADDR, DATAREADY) ;		// Segnalo dati disponibili
	
	do
		CAENComm_Read16(handle, HV_STATUS0_ADDR, &d16) ;		// Aspetto conferma dati prelevati
	while (d16 != OK) ;

	CAENComm_Write16(handle, HV_CTRL0_ADDR, 0) ;				// Reset segnalazione
	do
		CAENComm_Read16(handle, HV_STATUS0_ADDR, &d16) ;		// Aspetto reset segnalazione
	while (d16 == OK) ;
}

CAENComm_Write16(handle, HV_CTRL0_ADDR, 0) ;					// Reset Control Register
CAENComm_Write16(handle, HV_CTRL1_ADDR, 0) ;

}

// Globali     : Nessuna.
// Release     : 1.0.
// 
//*****************************************************************

int main(int argc, char *argv[])
{
CAENComm_ConnectionType type;
int t;
int link;
int bdnum;
int handle ;
UINT16 d16 ;

if (argc != 5 ) {
	printf ("\n !!! To few parameters.") ;
}

 type  = (CAENComm_ConnectionType) atoi(argv[1]);
 link  = (int) atoi(argv[2]);
 bdnum = (int) atoi(argv[3]);

if (CAENComm_OpenDevice(type, link, bdnum, 0, &handle) != CAENComm_Success)
	{
	printf ("\n !!! Communication Error.") ;
	exit(0) ;
	}

if (CAENComm_Read16(handle, BD_INFO_ADDR, &d16) != CAENComm_Success)		// Leggo il tipo di modulo
	{
	printf ("\n !!! Error on board type reading. Press any key to exit ... ") ;
	exit(0) ;
	}

if (d16 == DT55XX)
	{

	//----------------------------------------
	// DT55XX : programmazione delle 2 A639A
	//----------------------------------------

	HV_STATUS0_ADDR = (UINT32)0x1038 ;
	HV_CTRL0_ADDR =	(UINT32)0x1034 ;
	HV_CTRL1_ADDR =	(UINT32)0x1134 ;

	CAENComm_Read16(handle, HV_STATUS0_ADDR, &d16) ;
	if (d16 != OK)
		{
		CAENComm_Write16(handle, HV_CTRL0_ADDR, FUPDATEL) ;			// Invio richiesta Firmware Update per A639A CH0..1
		CAENComm_Write16(handle, HV_CTRL1_ADDR, FUPDATEH) ;
		Sleep(1000);
		}

	HV_STATUS0_ADDR = (UINT32)0x1238 ;
	HV_CTRL0_ADDR =	(UINT32)0x1234 ;
	HV_CTRL1_ADDR =	(UINT32)0x1334 ;

	CAENComm_Read16(handle, HV_STATUS0_ADDR, &d16) ;
	if (d16 != OK)
		{
		CAENComm_Write16(handle, HV_CTRL0_ADDR, FUPDATEL) ;			// Invio richiesta Firmware Update A639A CH2..3
		CAENComm_Write16(handle, HV_CTRL1_ADDR, FUPDATEH) ;
		Sleep(1000);
		}

	if (ReadFileHex(argv[4]) != 0)											// Leggo il file .hex
		{
		CAENComm_CloseDevice(handle) ;
		exit(0) ;
		}

	HV_VSET0_ADDR = (UINT32)0x1020 ;
	HV_ISET0_ADDR =	(UINT32)0x1024 ;
	HV_RUP0_ADDR = (UINT32)0x1028 ;
	HV_RDWN0_ADDR = (UINT32)0x102C ;
	HV_VMAX0_ADDR = (UINT32)0x1030 ;
	HV_VMON0_ADDR =	(UINT32)0x1040 ;
	HV_IMON0_ADDR =	(UINT32)0x1044 ;
	HV_CTRL0_ADDR =	(UINT32)0x1034 ;
	HV_STATUS0_ADDR = (UINT32)0x1038 ;

	HV_VSET1_ADDR = (UINT32)0x1120 ;
	HV_ISET1_ADDR =	(UINT32)0x1124 ;
	HV_RUP1_ADDR = (UINT32)0x1128 ;
	HV_RDWN1_ADDR = (UINT32)0x112C ;
	HV_VMAX1_ADDR = (UINT32)0x1130 ;
	HV_VMON1_ADDR =	(UINT32)0x1140 ;
	HV_IMON1_ADDR =	(UINT32)0x1144 ;
	HV_CTRL1_ADDR =	(UINT32)0x1134 ;
	HV_STATUS1_ADDR = (UINT32)0x1138 ;


	printf ("\n CH0..1 Firmware Upgrade ") ;

	FirmwareUpgrade(handle) ;

	HV_VSET0_ADDR = (UINT32)0x1220 ;
	HV_ISET0_ADDR =	(UINT32)0x1224 ;
	HV_RUP0_ADDR = (UINT32)0x1228 ;
	HV_RDWN0_ADDR = (UINT32)0x122C ;
	HV_VMAX0_ADDR = (UINT32)0x1230 ;
	HV_VMON0_ADDR =	(UINT32)0x1240 ;
	HV_IMON0_ADDR =	(UINT32)0x1244 ;
	HV_CTRL0_ADDR =	(UINT32)0x1234 ;
	HV_STATUS0_ADDR = (UINT32)0x1238 ;

	HV_VSET1_ADDR = (UINT32)0x1320 ;
	HV_ISET1_ADDR =	(UINT32)0x1324 ;
	HV_RUP1_ADDR = (UINT32)0x1328 ;
	HV_RDWN1_ADDR = (UINT32)0x132C ;
	HV_VMAX1_ADDR = (UINT32)0x1330 ;
	HV_VMON1_ADDR =	(UINT32)0x1340 ;
	HV_IMON1_ADDR =	(UINT32)0x1344 ;
	HV_CTRL1_ADDR =	(UINT32)0x1334 ;
	HV_STATUS1_ADDR = (UINT32)0x1338 ;

	printf ("\n\n CH2..3 Firmware Upgrade ") ;

	FirmwareUpgrade(handle) ;

	}
else
	{
	//----------------------------------------
	// DT5780 o DT5790	
	//----------------------------------------

	HV_VSET0_ADDR = (UINT32)0x1220 ;
	HV_ISET0_ADDR =	(UINT32)0x1224 ;
	HV_RUP0_ADDR = (UINT32)0x1228 ;
	HV_RDWN0_ADDR = (UINT32)0x122C ;
	HV_VMAX0_ADDR = (UINT32)0x1230 ;
	HV_VMON0_ADDR =	(UINT32)0x1240 ;
	HV_IMON0_ADDR =	(UINT32)0x1244 ;
	HV_CTRL0_ADDR =	(UINT32)0x1234 ;
	HV_STATUS0_ADDR = (UINT32)0x1238 ;

	HV_VSET1_ADDR = (UINT32)0x1320 ;
	HV_ISET1_ADDR =	(UINT32)0x1324 ;
	HV_RUP1_ADDR = (UINT32)0x1328 ;
	HV_RDWN1_ADDR = (UINT32)0x132C ;
	HV_VMAX1_ADDR = (UINT32)0x1330 ;
	HV_VMON1_ADDR =	(UINT32)0x1340 ;
	HV_IMON1_ADDR =	(UINT32)0x1344 ;
	HV_CTRL1_ADDR =	(UINT32)0x1334 ;
	HV_STATUS1_ADDR = (UINT32)0x1338 ;

	CAENComm_Read16(handle, HV_STATUS0_ADDR, &d16) ;
	if (d16 != OK)
		{
		CAENComm_Write16(handle, HV_CTRL0_ADDR, FUPDATEL) ;			// Invio richiesta Firmware Update
		CAENComm_Write16(handle, HV_CTRL1_ADDR, FUPDATEH) ;
		Sleep(1000);
		}

	if (ReadFileHex(argv[4]) != 0)											// Leggo il file .hex
		{
		CAENComm_CloseDevice(handle) ;
		exit(0) ;
		}

	FirmwareUpgrade(handle) ;
	}

CAENComm_CloseDevice(handle) ;

printf ("\n\n Ok") ;
exit(0) ;
}
