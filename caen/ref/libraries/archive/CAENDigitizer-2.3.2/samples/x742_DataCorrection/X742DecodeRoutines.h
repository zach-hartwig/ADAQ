#include <CAENDigitizer.h>

#define EVENT_HEADER_SIZE		0x10
#define X742_MAX_GROUPS			0x04
#define MAX_X742_CHANNEL_SIZE 	0x09
#define X742_FIXED_SIZE			0x400

/*****************************************************************************
* GetNumEvents(char *buffer, uint32_t buffsize, uint32_t *numEvents)
* Gets current number of event stored in the acquisition buffer
*
* [IN] buffer     : Address of the acquisition buffer 
* [IN] bufferSize : Size of the data stored in the acquisition buffer
* [OUT] numEvents : Number of events stored in the acquisition buffer
* 				  : return  0 = Success; 
******************************************************************************/
int32_t GetNumEvents(char *buffer, uint32_t buffsize, uint32_t *numEvents);

/*****************************************************************************
* GetEventPtr(char *buffer, uint32_t buffsize, int32_t numEvent, char **EventPtr)
* Retrieves the event pointer of a specified event in the acquisition buffer
*
* [IN] buffer     : Address of the acquisition buffer 
* [IN] bufferSize : Acquisition buffer size (in samples)
* [IN] numEvents  : Number of events stored in the acquisition buffer
* [OUT] EventPtr  : Pointer to the requested event in the acquisition buffer
* 				  : return  0 = Success; 
******************************************************************************/
int32_t GetEventPtr(char *buffer, uint32_t buffsize, int32_t numEvent, char **EventPtr);

/******************************************************************************
* X742_DecodeEvent(char *evtPtr, void **Evt)
* Decodes a specified event stored in the acquisition buffer writing data in Evt memory
* Once used the Evt memory MUST be deallocated by the caller!
*
* [IN]  EventPtr : pointer to the requested event in the acquisition buffer (MUST BE NULL)
* [OUT] Evt      : event structure with the requested event data
* 				 : return  0 = Success; 
******************************************************************************/
int32_t X742_DecodeEvent(char *evtPtr, void **Evt);