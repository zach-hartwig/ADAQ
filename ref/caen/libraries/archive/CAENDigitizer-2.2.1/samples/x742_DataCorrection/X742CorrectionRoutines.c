#include "X742CorrectionRoutines.h"

static void PeakCorrection(CAEN_DGTZ_X742_GROUP_t *dataout) {
	int offset;
	int chaux_en;
	unsigned int i;
	int j;

	chaux_en = (dataout->ChSize[8] == 0)? 0:1;
	for(j=0; j<(8+chaux_en); j++){
		dataout->DataChannel[j][0] = dataout->DataChannel[j][1];
	}
	for(i=1; i<dataout->ChSize[0]; i++){
		offset=0;
		for(j=0; j<8; j++){
			if (i==1){
				if ((dataout->DataChannel[j][2]- dataout->DataChannel[j][1])>30){								  
					offset++;
				}
				else {
					if (((dataout->DataChannel[j][3]- dataout->DataChannel[j][1])>30)&&((dataout->DataChannel[j][3]- dataout->DataChannel[j][2])>30)){								  
						offset++;
					}
				}
			}
			else{
				if ((i==dataout->ChSize[j]-1)&&((dataout->DataChannel[j][dataout->ChSize[j]-2]- dataout->DataChannel[j][dataout->ChSize[j]-1])>30)){  
					offset++;										 							        
				}
				else{
					if ((dataout->DataChannel[j][i-1]- dataout->DataChannel[j][i])>30){ 
						if ((dataout->DataChannel[j][i+1]- dataout->DataChannel[j][i])>30)
							offset++;
						else {
							if ((i==dataout->ChSize[j]-2)||((dataout->DataChannel[j][i+2]-dataout->DataChannel[j][i])>30))
								offset++;
						} 							        
					}
				}
			}
		}								

		if (offset==8){
			for(j=0; j<(8+chaux_en); j++){
				if (i==1){
					if ((dataout->DataChannel[j][2]- dataout->DataChannel[j][1])>30) {
						dataout->DataChannel[j][0]=dataout->DataChannel[j][2];
						dataout->DataChannel[j][1]=dataout->DataChannel[j][2];
					}
					else{
						dataout->DataChannel[j][0]=dataout->DataChannel[j][3];
						dataout->DataChannel[j][1]=dataout->DataChannel[j][3];
						dataout->DataChannel[j][2]=dataout->DataChannel[j][3];
					}
				}
				else{
					if (i==dataout->ChSize[j]-1){
						dataout->DataChannel[j][dataout->ChSize[j]-1]=dataout->DataChannel[j][dataout->ChSize[j]-2];
					}
					else{
						if ((dataout->DataChannel[j][i+1]- dataout->DataChannel[j][i])>30)
							dataout->DataChannel[j][i]=((dataout->DataChannel[j][i+1]+dataout->DataChannel[j][i-1])/2);
						else {
							if (i==dataout->ChSize[j]-2){
								dataout->DataChannel[j][dataout->ChSize[j]-2]=dataout->DataChannel[j][dataout->ChSize[j]-3];
								dataout->DataChannel[j][dataout->ChSize[j]-1]=dataout->DataChannel[j][dataout->ChSize[j]-3];
							}
							else {
								dataout->DataChannel[j][i]=((dataout->DataChannel[j][i+2]+dataout->DataChannel[j][i-1])/2);
								dataout->DataChannel[j][i+1]=( (dataout->DataChannel[j][i+2]+dataout->DataChannel[j][i-1])/2);
							}
						}
					}
				}
			}
		}								
	}
}

static int read_flash_page(int handle, uint8_t gr, int8_t* page, uint32_t pagenum) { 
  uint32_t flash_addr;
  uint16_t	dd;
  uint32_t i,tmp[528];
  uint32_t fl_a[528];
  uint8_t addr0,addr1,addr2;
  int ret;
  CAENComm_ErrorCode err[528];
  
	  flash_addr = pagenum<<9;
	  addr0 = (uint8_t)flash_addr;
	  addr1 = (uint8_t)(flash_addr>>8);
	  addr2 = (uint8_t)(flash_addr>>16);

	  dd=0xffff;
	  while ((dd>>2)& 0x1) 
		if ((ret = CAENComm_Read16(handle, STATUS(gr), &dd)) != CAENComm_Success) return -1;
	  // enable flash (NCS = 0)
	  if ((ret = CAENComm_Write16(handle, SEL_FLASH(gr), (int16_t)1)) != CAENComm_Success) return -1;
	  // write opcode
	   if ((ret = CAENComm_Write16(handle, FLASH(gr), (int16_t)MAIN_MEM_PAGE_READ)) != CAENComm_Success) return -1;
	  // write address
	  dd=0xffff;
	  if ((ret = CAENComm_Write16(handle, FLASH(gr), (int16_t)addr2)) != CAENComm_Success) return -1;
	  dd=0xffff;
	  if ((ret = CAENComm_Write16(handle, FLASH(gr), (int16_t)addr1)) != CAENComm_Success) return -1;
	  dd=0xffff;
	  if ((ret = CAENComm_Write16(handle, FLASH(gr), (int16_t)addr0)) != CAENComm_Success) return -1;
	  // additional don't care bytes
	  for (i=0; i<4; i++) {
		dd=0xffff;
	  if ((ret = CAENComm_Write16(handle, FLASH(gr), (int16_t)0)) != CAENComm_Success) return -1;
	  }
	  for (i=0; i<528; i+=2) {
		 fl_a[i] = FLASH(gr);
		 fl_a[i+1] = STATUS(gr);
	  }	  
	  if ((ret = CAENComm_MultiRead32(handle,fl_a,528,tmp,err)) != CAENComm_Success) 
		return -1;
	  for (i=0; i<528; i+=2) page[(int)(i/2)] = (int8_t) tmp[i];
	  // disable flash (NCS = 1)
	  if ((ret = CAENComm_Write16(handle, SEL_FLASH(gr), (int16_t)0))  != CAENComm_Success) 
	  return -1;  
	return 0;
}

int32_t LoadCorrectionTables(int handle, DataCorrection_t *Table, uint8_t group, uint32_t frequency) {
	uint32_t pagenum = 0,i,n,j,start;
	int8_t TempCell[264]; // 
	int8_t *p;
	int ret;
	int8_t tmp[0x1000]; // 256byte * 16 pagine
	for (n=0;n<MAX_X742_CHANNELS+1;n++) {
		pagenum = 0;
		pagenum = (group %2) ? 0xC00: 0x800;
		pagenum |= frequency << 8;
		pagenum |= n << 2;
		// load the Offset Cell Correction
		p = TempCell;
		start = 0;
		for (i=0;i<4;i++) {
			int endidx = 256;
			if ((ret =read_flash_page(handle,group,p,pagenum)) != 0) 
				return ret;
			// peak correction
			for (j=start;j<(start+256);j++) {
				if (p[j-start] != 0x7f) {
					Table->cell[n][j] = p[j-start];
				}
				else {
					short cel = (short)((((unsigned char)(p[endidx+1])) << 0x08) |((unsigned char) p[endidx]));
					if (cel == 0) Table->cell[n][j] = p[j-start]; else Table->cell[n][j] = cel;
					endidx+=2;
					if (endidx > 263) endidx = 256;
				}
			}
			start +=256;
			pagenum++;
		}
		start = 0;
		// load the Offset Num Samples Correction
		p = TempCell;
		pagenum &= 0xF00;
		pagenum |= 0x40;
		pagenum |= n << 2;
		
		for (i=0;i<4;i++) {
			if ((ret =read_flash_page(handle,group,p,pagenum)) != 0) 
				return ret;
			for (j=start;j<start+256;j++) Table->nsample[n][j] = p[j-start];
			start +=256;
			pagenum++;
		}
		if (n == MAX_X742_CHANNELS) {
			// load the Time Correction
			p = TempCell;
			pagenum &= 0xF00;
			pagenum |= 0xA0;
			start = 0;
			for (i=0;i<16;i++) {
				if ((ret =read_flash_page(handle,group,p,pagenum)) != 0) 
					return ret;
				for (j=start;j<start+256;j++) tmp[j] = p[j-start];
				start +=256;
				pagenum++;
			}
			for (i=0;i<1024;i++) {
				p = (int8_t *) &(Table->time[i]);
				p[0] = tmp[i*4];
				p[1] = tmp[(i*4)+1];
				p[2] = tmp[(i*4)+2];
				p[3] = tmp[(i*4)+3];
			}
		}
	}
	return 0;
}

void ApplyDataCorrection(DataCorrection_t* CTable, CAEN_DGTZ_DRS4Frequency_t frequency, int CorrectionLevelMask, CAEN_DGTZ_X742_GROUP_t *data) {

    int i, j,rpnt = 0, wpnt = 0, size1, size2,trg = 0,k;
    long samples;
    float Time[1024],t0; 
    float Tsamp; 
	float vcorr; 
    uint16_t st_ind=0; 
    uint32_t freq = frequency;
    float wave_tmp[1024];
	int cellCorrection =CorrectionLevelMask & 0x1;
	int nsampleCorrection = (CorrectionLevelMask & 0x2) >> 1;
	int timeCorrection = (CorrectionLevelMask & 0x4) >> 2;
   
	switch(frequency) {
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

	if (data->ChSize[8] != 0) trg = 1;
	st_ind =(uint16_t)(data->StartIndexCell);
	for (i=0;i<MAX_X742_CHANNEL_SIZE;i++) {
		size1  = data->ChSize[i];

		for (j=0;j<size1;j++) {
			if (cellCorrection) data->DataChannel[i][j]  -=  CTable->cell[i][((st_ind+j) % 1024)]; 
			if (nsampleCorrection) data->DataChannel[i][j] -= CTable->nsample[i][j];
		}
	}
	
	if (cellCorrection) PeakCorrection(data);
	if (!timeCorrection) return;

	t0 = CTable->time[st_ind];                       
	Time[0]=0.0;
		
	for(j=1; j < 1024; j++) {
		 t0= CTable->time[(st_ind+j)%1024]-t0;
		 if  (t0 >0) 
		   Time[j] =  Time[j-1]+ t0;
		 else
		   Time[j] =  Time[j-1]+ t0 + (Tsamp*1024);

		 t0 = CTable->time[(st_ind+j)%1024];
	}
	for (j=0;j<8+trg;j++) {
		data->DataChannel[j][0] = data->DataChannel[j][1];
		wave_tmp[0] = data->DataChannel[j][0];
		vcorr = 0.0;
		k=0;
		i=0;

		for(i=1; i<1024; i++)  {
			while ((k<1024-1) && (Time[k]<(i*Tsamp)))  k++;
			vcorr =(((float)(data->DataChannel[j][k] - data->DataChannel[j][k-1])/(Time[k]-Time[k-1]))*((i*Tsamp)-Time[k-1]));
			wave_tmp[i]= data->DataChannel[j][k-1] + vcorr;
			k--;								
		}
		memcpy(data->DataChannel[j],wave_tmp,1024*sizeof(float));
	}
}