#ifdef WIN64
#ifndef WIN32
	"YOU MUST DEFINE WIN32 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
#endif
#endif


#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
)


#if defined(WIN64) || defined(WIN32)
  #include <stdlib.h>
  #include <windows.h>
  #include <winioctl.h>
  #include <setupapi.h>
  #pragma comment(lib, "setupapi.lib")
  typedef unsigned long ULONG;
  typedef unsigned char  UCHAR;
  typedef unsigned short USHORT;
#else
  #include <stdlib.h>
  #include <sys/time.h>                           // Rel. 1.4
  #include <sys/types.h>
  #include <fcntl.h>
  #include <sys/ioctl.h>
  #include <unistd.h>
  #include <stdio.h>
  #include <string.h>
  #include "CAENUSBdrvB.h"
#endif


#ifdef WIN32
// {AE18AA60-7F6A-11d4-97DD-00010229B959}
static GUID CAENUSBDRV_GUID = {0xa4225514, 0x9cec, 0x49cd, 0xbb, 0x1e, 0xe7, 0x77, 0xef, 0xa1, 0xca, 0x6b};

#include "cyioctl.h"

HANDLE file_handle;   // Handle got from CreateFile
#else
int file_handle;   // Handle got from CreateFile
#define		BOOL	int
#define		HANDLE	int
#define		TRUE	1
#define		FALSE	0
#define		INVALID_HANDLE_VALUE	-1		
#endif




/*
        ------------------------------------------------------------------------

        openUSBDriver

        ------------------------------------------------------------------------
*/
#ifdef WIN32
static BOOL openUSBDriver(HANDLE *file_handle, int link)
#else
static int openUSBDriver(int *file_handle, int link)
#endif
{

#ifdef WIN32
	
	{  // Codice per driver windows
		SP_DEVINFO_DATA devInfoData;
		SP_DEVICE_INTERFACE_DATA  devInterfaceData;
		PSP_INTERFACE_DEVICE_DETAIL_DATA functionClassDeviceData;
		unsigned long requiredLength = 0;
		int deviceNumber = link; 
		HDEVINFO hwDeviceInfo = SetupDiGetClassDevs ( (LPGUID) &CAENUSBDRV_GUID,
			NULL,
			NULL,
			DIGCF_PRESENT|DIGCF_INTERFACEDEVICE);
		*file_handle = INVALID_HANDLE_VALUE;

		if (hwDeviceInfo != INVALID_HANDLE_VALUE) {
			devInterfaceData.cbSize = sizeof(devInterfaceData);
			if (SetupDiEnumDeviceInterfaces ( hwDeviceInfo, 0, (LPGUID) &CAENUSBDRV_GUID,
				deviceNumber, &devInterfaceData)) {
					unsigned long predictedLength = 0;

					SetupDiGetInterfaceDeviceDetail ( hwDeviceInfo, &devInterfaceData, NULL, 0,
						&requiredLength, NULL);

					predictedLength = requiredLength;
					functionClassDeviceData = (PSP_INTERFACE_DEVICE_DETAIL_DATA) malloc (predictedLength);
					functionClassDeviceData->cbSize = sizeof (SP_INTERFACE_DEVICE_DETAIL_DATA);

					devInfoData.cbSize = sizeof(devInfoData);

					if (SetupDiGetInterfaceDeviceDetail (hwDeviceInfo,
						&devInterfaceData,
						functionClassDeviceData,
						predictedLength,
						&requiredLength,
						&devInfoData)) {

							*file_handle = CreateFile (functionClassDeviceData->DevicePath,
								GENERIC_WRITE | GENERIC_READ,
								FILE_SHARE_WRITE | FILE_SHARE_READ,
								NULL,
								OPEN_EXISTING,
								FILE_FLAG_OVERLAPPED,
								NULL);
							SetupDiDestroyDeviceInfoList(hwDeviceInfo);
					}
					free(functionClassDeviceData);
			}
		}
	}
	if( file_handle == INVALID_HANDLE_VALUE )
                return FALSE;
        else
                return TRUE;

#else
		{
		char devname[80];
            	sprintf(devname,"/dev/usb/v1718_%d", link); // HACK : to be generic it should have a parameter board_number
		
            	*file_handle = open(devname, O_RDWR);
		if( *file_handle == INVALID_HANDLE_VALUE )
                	return FALSE;
        	else
                	return TRUE;
	
		}
#endif
}

/*
        ------------------------------------------------------------------------

        closeUSBDriver

        ------------------------------------------------------------------------
*/
static BOOL closeUSBDriver(HANDLE file_handle)
{
        BOOL ret;

#ifdef WIN32
        ret = CloseHandle(file_handle);
#else
        ret = !close(file_handle);
#endif
        file_handle = INVALID_HANDLE_VALUE;
        return ret;
}

#ifdef WIN32
static long releasePins(HANDLE file_handle) {
        unsigned long nBytes;
        BOOLEAN bResult;
		
		SINGLE_TRANSFER singleTransfer;
		memset( &singleTransfer, 0, sizeof( singleTransfer));
		singleTransfer.ucEndpointAddress= 0x00; // Control Endpoint

        singleTransfer.SetupPacket.bmReqType.Recipient = 0; // Device
		singleTransfer.SetupPacket.bmReqType.Type      = 2; // 2=Vendor
        singleTransfer.SetupPacket.bmReqType.Direction = 0; // OUT command (from Host to Device)
        singleTransfer.SetupPacket.bRequest            = 0xB0; //Reset   
        singleTransfer.SetupPacket.wValue              = 4;    
        
		bResult= DeviceIoControl (file_handle,
                IOCTL_ADAPT_SEND_EP0_CONTROL_TRANSFER,
                &singleTransfer, sizeof( singleTransfer),
                &singleTransfer, sizeof( singleTransfer),
                &nBytes, NULL);
        if( !bResult ) return -(long)GetLastError();

	return TRUE;
}
#endif

#ifdef WIN32
static long resetFromUSB(HANDLE file_handle) {
        unsigned long nBytes;
        BOOLEAN bResult;

		SINGLE_TRANSFER singleTransfer;
		memset( &singleTransfer, 0, sizeof( singleTransfer));
		singleTransfer.ucEndpointAddress= 0x00; // Control Endpoint

        singleTransfer.SetupPacket.bmReqType.Recipient = 0; // Device
		singleTransfer.SetupPacket.bmReqType.Type      = 2; // 2=Vendor
        singleTransfer.SetupPacket.bmReqType.Direction = 0; // OUT command (from Host to Device)
        singleTransfer.SetupPacket.bRequest            = 0xB0; //Reset   
        singleTransfer.SetupPacket.wValue              = 5;    
        
		bResult= DeviceIoControl (file_handle,
                IOCTL_ADAPT_SEND_EP0_CONTROL_TRANSFER,
                &singleTransfer, sizeof( singleTransfer),
                &singleTransfer, sizeof( singleTransfer),
                &nBytes, NULL);
        if( !bResult ) return -(long)GetLastError();

	return TRUE;
}
#endif

/*
** rebootBoard :
**    
*/
static long rebootBoard(HANDLE file_handle, int page) {
#ifdef WIN32
        unsigned long nBytes;
        BOOLEAN bResult; 

		SINGLE_TRANSFER singleTransfer;
		memset( &singleTransfer, 0, sizeof( singleTransfer));

		singleTransfer.ucEndpointAddress               = 0x00; // Control Endpoint
        singleTransfer.SetupPacket.bmReqType.Recipient = 0; // Device
		singleTransfer.SetupPacket.bmReqType.Type      = 2; // 2=Vendor
        singleTransfer.SetupPacket.bmReqType.Direction = 0; // OUT command (from Host to Device)
        singleTransfer.SetupPacket.bRequest            = 0xB0; //Reset   

		switch(page)
        {
          // Force Backup Page Reload 
          case 0 :
		    printf("Sending command to reboot from backup firmware image ...\n");
            singleTransfer.SetupPacket.wValue   = 1;    
            bResult= DeviceIoControl (file_handle,
                IOCTL_ADAPT_SEND_EP0_CONTROL_TRANSFER,
                &singleTransfer, sizeof( singleTransfer),
                &singleTransfer, sizeof( singleTransfer),
                &nBytes, NULL);
            if( !bResult ) return -(long)GetLastError();

            singleTransfer.SetupPacket.wValue   = 0;    
            bResult= DeviceIoControl (file_handle,
                IOCTL_ADAPT_SEND_EP0_CONTROL_TRANSFER,
                &singleTransfer, sizeof( singleTransfer),
                &singleTransfer, sizeof( singleTransfer),
                &nBytes, NULL);
            if( !bResult ) return -(long)GetLastError();
			printf("Command sent ....\n");
            break;

          // Force Standard Page Reload 
          case 1 :   
		    printf("Sending command to reboot from standard firmware image ...\n");
            singleTransfer.SetupPacket.wValue   = 3;    
            bResult= DeviceIoControl (file_handle,
                IOCTL_ADAPT_SEND_EP0_CONTROL_TRANSFER,
                &singleTransfer, sizeof( singleTransfer),
                &singleTransfer, sizeof( singleTransfer),
                &nBytes, NULL);
            if( !bResult ) return -(long)GetLastError();
 
            singleTransfer.SetupPacket.wValue   = 2;    
            bResult= DeviceIoControl (file_handle,
                IOCTL_ADAPT_SEND_EP0_CONTROL_TRANSFER,
                &singleTransfer, sizeof( singleTransfer),
                &singleTransfer, sizeof( singleTransfer),
                &nBytes, NULL);
            if( !bResult ) return -(long)GetLastError();
			printf("Command sent ....\n");
            break;

          default:
            break;
        }

        return 0;
           
#else // LINUX
	switch(page)
        {

          case 0 :
			printf("Sending command to reboot from backup firmware image ...\n");
			ioctl(file_handle, V1718_IOCTL_REBOOTB, NULL);
			printf("Command sent ....\n");
            break;
		  case 1 :   
		    printf("Sending command to reboot from standard firmware image ...\n");
			ioctl(file_handle, V1718_IOCTL_REBOOTF, NULL);
			printf("Command sent ....\n");
            break;
		  default:
            break;
        }

        return 0;
#endif

}

void usage() {

    printf("Usage : DeskBoot [[[--standard] || [-std]] || [[--help] || [-h]] [[--Link l] ||\n");
    printf("        Options:\n");
    printf("          --standard or -std : reboot from standard firmware image\n");
    printf("          --help or -h       : usage help\n");
    printf("          if no argument is passed, board is booted from backup firmware image\n");

}

int main(int argc, const char* argv[] ) {
	int i;
    char str[400];
	int page = 0;
	int link = 0;
    

	for (i = 1; i< argc; i++) {
		strcpy(str, argv[i]);
		if (strstr(str, "--standard") || (strstr(str, "-std"))) {
		   page = 1;
		   continue;
		}
		if (strstr(str, "--help") || (strstr(str, "-h"))) {
			usage();
			return 0;
		}
		if (strstr(str, "--Link") &&  ((i+1)< argc)) {
			link = atoi(argv[i+1]);
			i++;
			continue;
		}
		usage();
		return;
	}
	if(openUSBDriver(&file_handle, link)) {
		rebootBoard(file_handle, page);
       closeUSBDriver(file_handle);
       return 0;
    }
    else {
        printf("Cannot connect to device ...\n");
        return -1;
    }

}
