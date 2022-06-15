#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "CFASegment.h"


CFASegmentPtr newSegment(FILE* fin, int* last) {
	static const int stubSize = sizeof(CFASegmentStub);
	CFASegmentPtr segm;
	CFASegmentStub* stub = malloc(stubSize);
	fread(stub, 1, sizeof(CFASegmentStub), fin);

	if (stub->shebang != CFA_SHEBANG) {
		free(stub);
		return NULL;
	}

	stub = realloc(stub, stub->headerSizeBytes + stub->fwSizeBytes);
	fread((char*)stub + stubSize, 1, stub->headerSizeBytes + stub->fwSizeBytes - stubSize, fin);
	if (getSegmentType(stub) == CFA_SEGMENT_TYPE_1) {
		int i, jump = 0;
		CFASegmentType1* t1s = malloc(sizeof(CFASegmentType1));
		memcpy(t1s, stub, sizeof(CFASegmentType1));

		t1s->models = calloc(t1s->numModels, sizeof(CFAModelType1*));
		for (i=0; i < t1s->numModels; i++) {
			CFAModelType1* first = (CFAModelType1*)((char*)stub+stubSize+CFA_SEGMENT_T1_FAMILY_FIELDS_SIZE);
			int size = ((CFAModelType1*)((char*)first + jump))->numChecks * 8 + 8;
			t1s->models[i] = malloc(size);
			memcpy(t1s->models[i], (char*)first + jump, size);
			jump += size;
		}
		t1s->fwData = malloc(t1s->fwSizeBytes);
		memcpy(t1s->fwData, (char*)stub+stub->headerSizeBytes, t1s->fwSizeBytes);
		segm = t1s;
	} else {
		int i, jump = 0;
		CFASegmentType2* t2s = malloc(sizeof(CFASegmentType2));
		memcpy(t2s, stub, sizeof(CFASegmentType2));

		t2s->models = calloc(t2s->numModels, sizeof(CFAModelType2*));
		for (i=0; i < t2s->numModels; i++) {
			CFAModelType2* first = (CFAModelType2*)((char*)stub+stubSize);
			int size = 4;
			t2s->models[i] = malloc(size);
			memcpy(t2s->models[i], (char*)first + jump, size);
			jump += size;
		}
		t2s->fwData = malloc(t2s->fwSizeBytes);
		memcpy(t2s->fwData, (char*)stub+stub->headerSizeBytes, t2s->fwSizeBytes);
		segm = t2s;
	}

	if (last != NULL) *last = stub->lastHeader;

	free(stub);

	return segm;
}


void deleteSegment(CFASegmentPtr segm) {
	int i;
	if (getSegmentType(segm) == CFA_SEGMENT_TYPE_1) {
		CFASegmentType1* t1s = toType1(segm);
		for (i=0; i<t1s->numModels; i++)
			free(t1s->models[i]);
		free(t1s->models);
	} else {
		CFASegmentType2* t2s = toType2(segm);
		for (i=0; i<t2s->numModels; i++)
			free(t2s->models[i]);
		free(t2s->models);
	}
	free(segm);
}

int getSegmentType(CFASegmentPtr segm) {
	return toStub(segm)->segmentTypeId > TYPE_2_HEADER_VERSIONS ? 1 : 2;
}
