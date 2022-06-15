#ifndef CFASEGMENT_H

#include <stdint.h>

#define CFA_SHEBANG 0x00414643
#define TYPE_2_HEADER_VERSIONS 99

typedef enum {
	CFA_SEGMENT_TYPE_1 = 1,
	CFA_SEGMENT_TYPE_2 = 2
} CFASegmentType;

typedef struct CFAModelType1_t {
	int32_t modelId;
	int32_t numChecks;
	struct { uint32_t address, value; } registerChecks[]; // Flexi member -- C99
} CFAModelType1;

typedef struct CFAModelType2_t {
	int32_t modelId;
} CFAModelType2;


typedef struct CFASegmentStub_t {
	int32_t shebang;
	char description[32];
	int32_t segmentTypeId;
	int32_t specsVersion;
	int32_t headerSizeBytes;
	int32_t numModels;
	int32_t fwSizeBytes;
	int32_t lastHeader;
} CFASegmentStub;

typedef struct CFASegmentType1_t {
	int32_t shebang;
	char description[32];
	int32_t segmentTypeId;
	int32_t specsVersion;
	int32_t headerSizeBytes;
	int32_t numModels;
	int32_t fwSizeBytes;
	int32_t lastHeader;

	int32_t accessType;
	int32_t flashCsAddress;
	int32_t flashCwAddress;
	int32_t flashPageSize;
	int32_t flashStdAddress;
	int32_t flashBckAddress;
	int32_t flashEnLevel;

	CFAModelType1** models;
	char* fwData;
} CFASegmentType1;

#define CFA_SEGMENT_T1_FAMILY_FIELDS_SIZE 28

typedef struct CFASegmentType2_t {
	int32_t shebang;
	char description[32];
	int32_t segmentTypeId;
	int32_t specsVersion;
	int32_t headerSizeBytes;
	int32_t numModels;
	int32_t fwSizeBytes;
	int32_t lastHeader;

	CFAModelType2** models;
	void* fwData;
} CFASegmentType2;


typedef void* CFASegmentPtr;

CFASegmentPtr newSegment(FILE* fin, int* last);
void deleteSegment(CFASegmentPtr segm);

int getSegmentType(CFASegmentPtr segm);

#define toStub(x)  ((CFASegmentStub*)x)
#define toType1(x) ((CFASegmentType1*)x)
#define toType2(x) ((CFASegmentType2*)x)


#endif