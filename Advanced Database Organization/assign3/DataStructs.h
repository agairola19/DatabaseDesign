#include "storage_mgr.h"

typedef struct numPageFixed
{
	int pageNum;
	int fixCount;
	struct numPageFixed *nextFixedPgCt;

}numPageFixed;



//Frame Struct in memory.
typedef struct frame
{
		char frameData[PAGE_SIZE + 1]; //+1 to store /0 character
		struct frame *nextFramePtr;
}frameList;

typedef struct pgAndDirtyBitStruct
{
	int pageNum;
	char  Dirty;
	struct pgAndDirtyBitStruct *nxtPgDirtyBit;

}pgAndDirtyBitStruct;


//struct with pageNum in mem and discPageNum
typedef struct pageMap
{
	int pageNum;
	int discPageNum;
	bool refBitClock;
	struct pageMap *nextpageMap;

}pageMap;



//Vars
frameList *firstFptr = NULL;
pageMap *firstpageMapPtr = NULL;
pgAndDirtyBitStruct *pgAndDirtyBitMap = NULL;
numPageFixed *firstnumPageFixedPtr = NULL;
pageMap *clockPtr = NULL;
SM_FileHandle fHandle;
int numFrames = 0;
PageNumber *frameContentPtr;
bool *dirtyBitPtr;
int *fixCountPtr;
int numWrites = 0;
int numReads = 0;
