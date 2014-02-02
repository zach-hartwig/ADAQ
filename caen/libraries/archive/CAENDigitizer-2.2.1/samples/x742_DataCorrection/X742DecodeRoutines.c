
static int32_t getNumberOfBits(uint8_t byte) {
	uint32_t i,count;
	count = 0;
	for (i=0;i<8;i++) {
		if ((byte >> i) & 0x1) count++;
	}
	return count;
}

static int V1742UnpackEventGroup(uint32_t group, uint32_t *datain, CAEN_DGTZ_X742_GROUP_t *dataout) {

    int i, j,rpnt = 0, wpnt = 0, size1, size2,trg = 0,k;
    long samples;
    float Time[1024],t0; 
    float Tsamp; 
    float vcorr; 
    uint16_t st_ind=0; 
    uint32_t freq;
    float wave_tmp[1024]; 
   
   freq = (datain[0] >> 16 ) & 0x3; 
	switch(freq) {
		case CAEN_DGTZ_DRS4_2_5GHz:
			Tsamp =(float)((1.0/2500.0)*1000.0);
			break;
		case CAEN_DGTZ_DRS4_1GHz:
			Tsamp =(float)((1.0/1000.0)*1000.0);
			break;
		default:
			Tsamp =(float)((1.0/5000.0)*1000.0);
			break;
	}

	st_ind =(uint16_t)((datain[0]>>20)& 0x3FF);
	size1  = datain[0]& 0xFFF;
	if ((trg = (datain[0]>>12)& 0x1) == 1) 
	  size2  =(datain[0]>>3)& 0x1FF;
	else
	  size2=0;

	dataout->TriggerTimeTag =datain[size1+size2+1] & 0x3FFFFFFF;

	samples = ((long) (size1 /3)); 

    while (rpnt < size1) {

        switch (rpnt % 3) {
          case 0 :
				dataout->DataChannel[0][wpnt]  =  (float) (datain[rpnt+1] & 0x00000FFF);            /* S0[11:0] - CH0 */
				dataout->DataChannel[1][wpnt]  =  (float) ((datain[rpnt+1] & 0x00FFF000) >> 12);    /* S0[11:0] - CH1 */
				dataout->DataChannel[2][wpnt]  =  (float) ((datain[rpnt+1] & 0xFF000000) >> 24);    /* S0[ 7:0] - CH2 */
              break;
          case 1 : 
				dataout->DataChannel[2][wpnt] +=  (float) ((datain[rpnt+1] & 0x0000000F) << 8);
				dataout->DataChannel[3][wpnt]  =  (float) ((datain[rpnt+1] & 0x0000FFF0) >> 4);     /* S0[11:0] - CH3 */
				dataout->DataChannel[4][wpnt]  =  (float) ((datain[rpnt+1] & 0x0FFF0000) >> 16);    /* S0[11:0] - CH4 */
				dataout->DataChannel[5][wpnt]  =  (float) ((datain[rpnt+1] & 0xF0000000) >> 28);    /* S0[3:0]  - CH5 */
              break;
          case 2 :
				dataout->DataChannel[5][wpnt] +=  (float) ((datain[rpnt+1] & 0x000000FF) << 4);
				dataout->DataChannel[6][wpnt]  =  (float) ((datain[rpnt+1] & 0x000FFF00) >> 8) ;    /* S0[11:0] - CH6 */
				dataout->DataChannel[7][wpnt]  =  (float) ((datain[rpnt+1] & 0xFFF00000) >> 20);    /* S0[11:0] - CH7 */		
              wpnt++;
              break;
        }
        rpnt++;
    }
    rpnt++; 
    for (k=0;k<8;k++) dataout->ChSize[k] = wpnt; 
	wpnt=0;

	for (i=0; i < size2; i++) {
        switch (i % 3) {
          case 0 :
				dataout->DataChannel[8][wpnt]   =  (float)  ( datain[rpnt+i] & 0x00000FFF);          /* S0 - CH8 */
				dataout->DataChannel[8][++wpnt] =  (float)  ((datain[rpnt+i] & 0x00FFF000) >> 12) ;  /* S1 - CH8 */
				dataout->DataChannel[8][++wpnt] =  (float)  ((datain[rpnt+i] & 0xFF000000) >> 24);   /* S2[ 7:0] - CH8 */
              break;
          case 1 :
				dataout->DataChannel[8][wpnt]  +=  (float)  ((datain[rpnt+i] & 0x0000000F) << 8);
				dataout->DataChannel[8][++wpnt] =  (float)  ((datain[rpnt+i] & 0x0000FFF0) >> 4);    /* S3 - CH8*/
				dataout->DataChannel[8][++wpnt] =  (float)  ((datain[rpnt+i] & 0x0FFF0000) >> 16);   /* S4 - CH8 */
				dataout->DataChannel[8][++wpnt] =  (float)  ((datain[rpnt+i] & 0xF0000000) >> 28);   /* S5[3:0]  - CH8 */
              break;
          case 2 :
				dataout->DataChannel[8][wpnt]  +=  (float)  ((datain[rpnt+i] & 0x000000FF) << 4);    /* S5[11:4] - CH8 */
				dataout->DataChannel[8][++wpnt] =  (float)  ((datain[rpnt+i] & 0x000FFF00) >> 8);    /* S6[11:0] - CH8 */
				dataout->DataChannel[8][++wpnt] =  (float)  ((datain[rpnt+i] & 0xFFF00000) >> 20);   /* S7[11:0] - CH8 */
              wpnt++;
              break;
        }
    }
    dataout->ChSize[8] = wpnt; 
	dataout->StartIndexCell = (uint16_t)st_ind;
	return (size1+size2+2);
}

int32_t GetNumEvents(char *buffer, uint32_t buffsize, uint32_t *numEvents) {
	uint32_t i = 0,evtSize;
	int ret;
	int32_t counter = -1;
    if ((buffsize == 0) || (buffer == NULL)){
        *numEvents = 0;
        return 0;
    }
	if (buffsize < EVENT_HEADER_SIZE) return -1;
	do {
		counter++;
		evtSize = *(long *)(buffer+i) & 0x0FFFFFFF;
		i += (uint32_t) (evtSize * 4);
	} while ((i + EVENT_HEADER_SIZE) < buffsize);
	*numEvents = counter + 1;
	return 0;
}

int32_t GetEventPtr(char *buffer, uint32_t buffsize, int32_t numEvent, char **EventPtr) {
	uint32_t i = 0;
	int32_t counter = -1;
	int ret;
	int evtSize;
	
	if ((buffer == NULL) || (buffsize < EVENT_HEADER_SIZE)) return -1;
	do {
		counter++;
		evtSize = *(long *)(buffer+i) & 0x0FFFFFFF;
		if (counter == numEvent) {
			if ((i + (uint32_t) evtSize) < buffsize) {
				*EventPtr =(buffer+i);
				return 0;
			}
			else return -1;
		}
		i += (uint32_t) (evtSize *4);
	} while ((i + EVENT_HEADER_SIZE) < buffsize);
	return -1;
}

int32_t X742_DecodeEvent(char *evtPtr, void **Evt) {
	CAEN_DGTZ_X742_EVENT_t *Event;
	uint32_t *buffer;
	char chanMask;
	uint32_t j,g,size;
	uint32_t *pbuffer;
	uint32_t eventSize;
	int evtSize,h;

	evtSize = *(long *)evtPtr & 0x0FFFFFFF;
	chanMask = *(long *)(evtPtr+4) & 0x0000000F;
	evtPtr += EVENT_HEADER_SIZE;
	buffer = (uint32_t *) evtPtr;
	pbuffer = (uint32_t *) evtPtr;
	eventSize = (evtSize * 4) - EVENT_HEADER_SIZE;
	if (eventSize == 0) return -1;
	Event = (CAEN_DGTZ_X742_EVENT_t *) malloc(sizeof(CAEN_DGTZ_X742_EVENT_t));
	if (Event == NULL) return -1;
	memset( Event, 0, sizeof(CAEN_DGTZ_X742_EVENT_t));
	for (g=0; g<X742_MAX_GROUPS; g++) {
		if ((chanMask >> g) & 0x1) {
		for (j=0; j<MAX_X742_CHANNEL_SIZE; j++) {
			Event->DataGroup[g].DataChannel[j]= malloc(X742_FIXED_SIZE * sizeof (float));
			if (Event->DataGroup[g].DataChannel[j] == NULL) {
				for (h=j-1;h>-1;h++) free(Event->DataGroup[g].DataChannel[h]);
				return -1;
			}
		}
		size=V1742UnpackEventGroup(g,pbuffer,&(Event->DataGroup[g]));
		pbuffer+=size;
		Event->GrPresent[g] = 1;	
		} 
		else {
			Event->GrPresent[g] = 0;
			for (j=0; j<MAX_X742_CHANNEL_SIZE; j++) {
				Event->DataGroup[g].DataChannel[j] = NULL;
			}
		}
	}
    *Evt = Event;
	return 0;
}