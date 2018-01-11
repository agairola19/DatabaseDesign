#include <stdlib.h>
#include <string.h>

#include "buffer_mgr.h"
#include "dberror.h"

#include "storage_mgr.h"
#include "DataStructs.h"

RC markDirty (BM_BufferPool *const bm, BM_PageHandle *const page) {
	pgAndDirtyBitStruct *temp; //create temp ptr
	pageMap *start = firstpageMapPtr;
	while(start->discPageNum != page->pageNum) //while its not equal to the page num
	{
		start = start->nextpageMap;//go to next
	}
		temp = pgAndDirtyBitMap;
		while(start->pageNum != temp->pageNum) {
		temp = temp->nxtPgDirtyBit;
		}
	if(temp != NULL)//if not null its dirty
	{
		temp->Dirty = 1;
		return RC_OK;
	}
	else 
		numWrites = 0;
		numReads = 0;
	{
		return RC_DIRTY_UPDATE_FAILED; //else we failed to update so return err
	}
}
frameList *createBufferPool(int numPages)
{
int ct = 1;//counter is 1
frameList *currFptr = NULL; //current ptr to null
frameList *prevFptr = NULL; //next ptr to null

while(ct <= numPages) //while count is less than num pages
		{
			currFptr = (frameList *)malloc(sizeof(frameList));
			if(ct == 1) // if its the first page
				firstFptr = currFptr; // firstFptr
			else
			prevFptr->nextFramePtr = currFptr; //the previous pointers next becomes current ptr
			prevFptr = currFptr;//the prev pointer becomes current pointer
			ct++;
		}
		currFptr->nextFramePtr = NULL;
		return firstFptr;
}
pageMap *createpgMap(int numPages)
{
	pageMap *prevPgPtr = NULL; //set all ptrs to null
	pageMap *start = NULL;
	pageMap *currPgPtr = NULL;
	int ct = 0;
	while(ct < numPages) 
	{
		currPgPtr = (pageMap *)malloc(sizeof(pageMap));
		if(ct == 0) //first/start 
		{
			start = currPgPtr; 
			clockPtr = start;
		}
		else
		prevPgPtr->nextpageMap = currPgPtr;
		currPgPtr->pageNum = ct;
		currPgPtr->discPageNum = -1;
		currPgPtr->refBitClock = 0;
		prevPgPtr = currPgPtr;
		ct++;
	}
	currPgPtr->nextpageMap = NULL;
	return start;
}
pgAndDirtyBitStruct *createPgAndDirtyBit(int numPages)
{
	int ct = 0;
	pgAndDirtyBitStruct *start = NULL;
	pgAndDirtyBitStruct *currPgDirtyPtr = NULL;
	pgAndDirtyBitStruct *prevPgDirtyPtr = NULL;
//iterate while count less than num pages
	while(ct < numPages) 
	{
		currPgDirtyPtr = (pgAndDirtyBitStruct *)malloc(sizeof(pgAndDirtyBitStruct));
		if(ct == 0)
			start = currPgDirtyPtr; 
		else
		{
		prevPgDirtyPtr->nxtPgDirtyBit = currPgDirtyPtr;
		}
		currPgDirtyPtr->pageNum = ct;
		currPgDirtyPtr->Dirty = 0;
		prevPgDirtyPtr = currPgDirtyPtr;
		ct++;
	}
	currPgDirtyPtr->nxtPgDirtyBit = NULL;
	return start;
}
numPageFixed *makeNumPgFixMap(int numPages)
{
	numPageFixed *prevNumPgFixPtr = NULL;
	numPageFixed *start = NULL;
	numPageFixed *currNumPgFixPtr = NULL;
	int ct = 0;
	while(ct < numPages) 
		{
		currNumPgFixPtr = (numPageFixed *)malloc(sizeof(numPageFixed));
		if(ct == 0)
			start = currNumPgFixPtr;
		else
		prevNumPgFixPtr->nextFixedPgCt = currNumPgFixPtr;
		currNumPgFixPtr->pageNum = ct;
		currNumPgFixPtr->fixCount = 0;
		prevNumPgFixPtr = currNumPgFixPtr;
		ct++;
	}
	currNumPgFixPtr->nextFixedPgCt = NULL;
	return start;
}
//#initBufferPool:
//creates a new buffer pool with numPages page frames using the page replacement strategy strategy. The pool is used to cache pages from the page file with name pageFileName

RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName,
		  const int numPages, ReplacementStrategy strategy,
		  void *stratData)
{
	dirtyBitPtr = (bool *)malloc(sizeof(bool) * numPages);
	fixCountPtr = (int *)malloc(sizeof(int) * numPages);
	frameContentPtr = (PageNumber *)malloc(sizeof(PageNumber) * numPages);
	numFrames = numPages;
	numWrites = 0;
	numReads  = 0;
	bm->mgmtData = createBufferPool(numPages); 
	pgAndDirtyBitMap = createPgAndDirtyBit(numPages);
	firstpageMapPtr = createpgMap(numPages);
	openPageFile((char *)pageFileName, &fHandle);
	firstnumPageFixedPtr = makeNumPgFixMap(numPages);

	bm->numPages = numPages;
	bm->pageFile = (char *)pageFileName;
	bm->strategy = strategy;
	if(bm->mgmtData != NULL && firstpageMapPtr !=NULL && pgAndDirtyBitMap != NULL && firstnumPageFixedPtr != NULL)
		return RC_OK;
	else
		return RC_BUFFER_POOL_INIT_ERROR;
}

RC isPageInMap(const PageNumber pageNum)
{
	pageMap *start = firstpageMapPtr;
	while(start != NULL)
	{
		if(start->discPageNum == pageNum)
			return start->pageNum;
		start = start->nextpageMap;
	}
	return RC_NO_FRAME;
}

void getFrameData(int frameNumber,BM_PageHandle * page)
{
	frameList *start = firstFptr;
	int ct = 0;
	while(ct < frameNumber)
	{
		start = start->nextFramePtr;
		ct++;
	}
	page->data = start->frameData;
}

void findFirstFreeFrame(int *firstFreeFrame,PageNumber PageNum)
{
	pageMap *start = firstpageMapPtr;
	while(start != NULL && start->discPageNum != -1)
	{
		start = start->nextpageMap;
	}
if(start == NULL)
	{
	*firstFreeFrame = -99;
	}
	else
	{
	*firstFreeFrame = start->pageNum;
	start->discPageNum = PageNum;	
	}
}
RC editFixCt(int flag,int page)
{
	pageMap *startFramePtr = firstpageMapPtr;
	while(startFramePtr != NULL && startFramePtr->discPageNum != page)
	{
		startFramePtr = startFramePtr->nextpageMap;
	}

	numPageFixed *startFixCountPtr = firstnumPageFixedPtr;
	while((startFixCountPtr != NULL) && (startFixCountPtr->pageNum != startFramePtr->pageNum))
	{
		startFixCountPtr = startFixCountPtr->nextFixedPgCt;
	}
	if(flag == 1)
		startFixCountPtr->fixCount++;
	else
		startFixCountPtr->fixCount--;

	return RC_OK;
}
//#forcePage:
//should write the current content of the page back to the page file on disk.

RC forcePage (BM_BufferPool *const bm, BM_PageHandle *const page) {
	if(writeBlock (page->pageNum, &fHandle, page->data) == RC_OK)
	{
		numWrites++;
		return RC_OK;
	}
	else
	{
		return RC_WRITE_FAILED;
	}
}
//#unpinPage:
//unpins the page page. The pageNum field of page should be used to figure out which page to unpin.

RC unpinPage (BM_BufferPool *const bm, BM_PageHandle *const page)
{
	pageMap *startFramePtr = firstpageMapPtr;
	while(startFramePtr != NULL && startFramePtr->discPageNum != page->pageNum)
	{
		startFramePtr = startFramePtr->nextpageMap;
	}
	RC status = editFixCt(2,startFramePtr->discPageNum); 
	int frameNumber = startFramePtr->pageNum;
	pgAndDirtyBitStruct *DirtyPtrStrt = pgAndDirtyBitMap;
	while(DirtyPtrStrt != NULL && DirtyPtrStrt->pageNum != frameNumber)
	{
	DirtyPtrStrt = DirtyPtrStrt->nxtPgDirtyBit;
	}
	if(DirtyPtrStrt->Dirty == 1)
		forcePage(bm,page);
	if(status == RC_OK)
		return RC_OK;
	else
		return RC_UNPIN_FAILED;
}

RC FIFO(BM_BufferPool *const bm, BM_PageHandle *const page, const PageNumber pageNum,SM_PageHandle ph)
{
	//OLDEST page is removed when needed
	numPageFixed *fixCountStart = firstnumPageFixedPtr;
	pageMap *beginning =firstpageMapPtr;
	while(fixCountStart != NULL && fixCountStart->pageNum != beginning->pageNum)
		fixCountStart = fixCountStart->nextFixedPgCt;
	if(fixCountStart != NULL && fixCountStart->fixCount > 0)
	{
		beginning = firstpageMapPtr;
		int newPgFrameNum = beginning->nextpageMap->pageNum;
		pageMap *currPgPtr = (pageMap *)malloc(sizeof(pageMap));
		currPgPtr->discPageNum = pageNum;
		currPgPtr->pageNum = newPgFrameNum;
		currPgPtr->nextpageMap = beginning->nextpageMap->nextpageMap;
		free(beginning->nextpageMap);
		beginning->nextpageMap = currPgPtr;
		int ct = 0;
		frameList *beginFrame = firstFptr;
		while(ct < newPgFrameNum)
		{
			beginFrame = beginFrame->nextFramePtr;
			ct++;
		}
		memset(beginFrame->frameData,'\0',PAGE_SIZE + 1);
		if(ph != NULL)
			strcpy(beginFrame->frameData,ph);
		page->data = beginFrame->frameData;
		page->pageNum = pageNum;
	}
	else
	{
		int newPgFrameNum = firstpageMapPtr->pageNum;
		pageMap *temp;
		temp = firstpageMapPtr;
		firstpageMapPtr = firstpageMapPtr->nextpageMap;
		free(temp);
		temp = NULL;
		pageMap *currPgPtr = (pageMap *)malloc(sizeof(pageMap));
		currPgPtr->discPageNum = pageNum;
		currPgPtr->pageNum = newPgFrameNum;
		currPgPtr->nextpageMap = NULL;
		temp = firstpageMapPtr;
		while(temp->nextpageMap != NULL)
		{
			temp = temp->nextpageMap;
		}
		temp->nextpageMap = currPgPtr;
		int ct = 0;
		frameList *beginFrame = firstFptr;
		while(ct < newPgFrameNum)
		{
			beginFrame = beginFrame->nextFramePtr;
			ct++;
		}
		memset(beginFrame->frameData,'\0',PAGE_SIZE + 1);
		if(ph != NULL)
		strcpy(beginFrame->frameData,ph);
		page->data = beginFrame->frameData;
		page->pageNum = pageNum;
	}
	return RC_OK;
}
void attachToEnd(pageMap *temp)
{
	pageMap *start = firstpageMapPtr;
	while(start->nextpageMap != NULL)
		start = start->nextpageMap;
	start->nextpageMap = temp;
}
//##getDirtyFlags:
//returns an array of bools (of size numPages) where the ith element is TRUE if the page stored in the ith page frame is dirty. Empty page frames are considered as clean.
bool *getDirtyFlags (BM_BufferPool *const bm)
{
	pgAndDirtyBitStruct *start = pgAndDirtyBitMap;
	int i = 0;
	while(start != NULL)
	{
		if(start->Dirty == 1)
			dirtyBitPtr[i++] = true;
		else
			dirtyBitPtr[i++] = false;
		start = start->nxtPgDirtyBit;
	}
	return dirtyBitPtr;
}

void FixCtSort(int *arrayInts, int size)
{
      char flag = 'Y';
      int j = 0;
      int temp;
      while (flag == 'Y')
      {
            flag = 'N';j++;int i;
            for (i = 0; i <size-j; i++)
            {
                  if (arrayInts[i] > arrayInts[i+1])
                  {
                        temp = arrayInts[i];
                        arrayInts[i] = arrayInts[i+1];
                        arrayInts[i+1] = temp;
                        flag = 'Y';
                  }
            }
      }
}
void clockPointerMove()
{
	if(clockPtr->nextpageMap == NULL)
		clockPtr = firstpageMapPtr;
	else
		clockPtr = clockPtr->nextpageMap;
}
//#pinPage:
//pins the page with page number pageNum. The buffer manager is responsible to set the pageNum field of the page handle passed to the method. Similarly, the data field should point to the page frame the page is stored in (the area in memory storing the content of the page).

RC pinPage (BM_BufferPool *const bm, BM_PageHandle *const page, const PageNumber pageNum)
{
	/* Strategy
	  1.if the disc number is present in our struct,
			Data is in the BufferPool, so get the pageNum and paste content
		else
		Data needs to come from disc.
	   	1. if free nodes
	   		i. Get 1st free node, paste value onto field.
	   		ii. Get pageNum of ith val from disk mapp table(pageNum =x)
	   		iii. Go to xth frame in BufferPool and put contents from readBlock(i) into ith frame
	   		iv. Put xth frame's content from buffer pool
	   		
	   	2. else
			use replacement Strategy(1,2,3,4 depending on which one)
*/
	
	SM_PageHandle ph = (SM_PageHandle)malloc(PAGE_SIZE);
	ensureCapacity((pageNum + 1),&fHandle);
	PageNumber frameNumber = isPageInMap(pageNum);
	if(frameNumber != RC_NO_FRAME)
	{
		page->pageNum = frameNumber;
		getFrameData(frameNumber,page);
		page->pageNum = pageNum;
		if(bm->strategy == RS_LRU)
		{
			pageMap *temp = firstpageMapPtr;
			pageMap *prev = NULL;
			int ct = 0;
			while(temp !=NULL && temp->discPageNum != pageNum)
			{
				prev = temp;
				temp = temp->nextpageMap;
				ct ++;
			}
			if(temp != NULL)
			{
				if(ct == 0)
				{
					prev = firstpageMapPtr;
					firstpageMapPtr = firstpageMapPtr->nextpageMap;
					prev->nextpageMap = NULL;
					attachToEnd(temp);
				}
				else
				{
					prev->nextpageMap = temp->nextpageMap;
					temp->nextpageMap = NULL;
					attachToEnd(temp);
				}
			}
		}
		else if(bm->strategy == RS_CLOCK)
		{
			pageMap *start = firstpageMapPtr;
			while(frameNumber != start->pageNum)
			{
				start = start->nextpageMap;
			}
			start->refBitClock = 1;
		}
	}
	else
	{
		int freeframeNumber = - 99;
		findFirstFreeFrame(&freeframeNumber,pageNum);
		readBlock(pageNum, &fHandle,ph);
		numReads++;
		if(freeframeNumber != -99)
		{
			int ct = 0;
			frameList *start = firstFptr;
			while(ct < freeframeNumber)
			{
				start = start->nextFramePtr;
				ct++;
			}
			memset(start->frameData,'\0',PAGE_SIZE+1);
			if(ph != NULL)
				strcpy(start->frameData,ph);
				page->data = start->frameData;
			page->pageNum = pageNum;
			pageMap *beginning = firstpageMapPtr;
			while(beginning->pageNum != freeframeNumber)
			{
				beginning = beginning->nextpageMap;
			}
			beginning->discPageNum = pageNum;
		if(bm->strategy == RS_CLOCK)
			{
				clockPtr->refBitClock = 0;
				clockPointerMove();
			}
		}
		else
		{
			if(bm->strategy == RS_LRU || bm->strategy == RS_FIFO )
			{
				FIFO(bm,page,pageNum,ph);
			}

			else if(bm->strategy == RS_CLOCK)
			{
				while(clockPtr->refBitClock == 1)
				{
					clockPtr->refBitClock = 0;
					clockPointerMove();
				}
				clockPtr->discPageNum = pageNum;
				clockPointerMove();
				page->pageNum = pageNum;
			}
			
			else if(bm->strategy == RS_LFU)
			{
				numPageFixed *start = firstnumPageFixedPtr;
				int sortedFixCountArray[bm->numPages];
				int index = 0;
				while(start != NULL)
				{
					sortedFixCountArray[index++] = start->fixCount;
					start = start->nextFixedPgCt;
				}
				FixCtSort(sortedFixCountArray,bm->numPages);

				start = firstnumPageFixedPtr;
				while(start->fixCount != sortedFixCountArray[0])
				{
					start = start->nextFixedPgCt;
				}
				start->fixCount = 0;

				pageMap *tempRPM = firstpageMapPtr;
				while(tempRPM->pageNum != start->pageNum)
				{
					tempRPM = tempRPM->nextpageMap;
				}
				tempRPM->discPageNum = pageNum;
				page->pageNum = pageNum;
			}
		}
	}
	editFixCt(1,pageNum);
	free(ph);
	ph = NULL;
	return RC_OK;
}
//##getFixCounts:
//function returns an array of ints (of size numPages) where the ith element is the fix count of the page stored in the ith page frame. 
int *getFixCounts (BM_BufferPool *const bm)
{
	numPageFixed *start = firstnumPageFixedPtr;
	int i = 0;
	while(start != NULL)
	{
		fixCountPtr[i++] = start->fixCount;
		start = start->nextFixedPgCt;
	}
	return fixCountPtr;
}

//##getFrameContents:
//returns an array of PageNumbers (of size numPages) where the ith element is the number of the page stored in the ith page frame. An empty page frame is represented using the constant NO_PAGE.
PageNumber *getFrameContents (BM_BufferPool *const bm)
{
	pageMap *start = firstpageMapPtr;
	while(start != NULL)
	{
		frameContentPtr[start->pageNum] = start->discPageNum;
		start = start->nextpageMap;
	}
	return frameContentPtr;
}

//#shutdownBufferPool :
//destroys a buffer pool. This method should free up all resources associated with buffer pool. For example, it should free the memory allocated for page frames
RC shutdownBufferPool(BM_BufferPool *const bm)
{
	bool FixNonZeroFlag = false;
	numPageFixed *start = firstnumPageFixedPtr;
	while(start != NULL)
	{
		if(start->fixCount >0)
		{
			FixNonZeroFlag = true;
			break;
		}
		start = start->nextFixedPgCt;
	}
	start = NULL;

	if(FixNonZeroFlag == false)
	{
		clockPtr = NULL;
		forceFlushPool(bm);
		free(frameContentPtr);
		free(dirtyBitPtr);
		free(fixCountPtr);
		frameList *firstFptr = NULL;
		int ct = 0;
		numPageFixed *tempB = NULL;
		pgAndDirtyBitStruct *tempA = NULL;
		frameList *tempC = NULL;
		pageMap *RPMtemp = NULL;
		firstFptr = (frameList*) bm->mgmtData;
		bm->mgmtData = NULL;
		
		while(ct < bm->numPages)
		{
			tempB = firstnumPageFixedPtr;
			firstnumPageFixedPtr = firstnumPageFixedPtr->nextFixedPgCt;
			free(tempB);

			RPMtemp = firstpageMapPtr;
			firstpageMapPtr = firstpageMapPtr->nextpageMap;
			free(RPMtemp);

			tempA = pgAndDirtyBitMap;
			pgAndDirtyBitMap = pgAndDirtyBitMap->nxtPgDirtyBit;
			free(tempA);

			tempC = firstFptr;
			firstFptr = firstFptr->nextFramePtr;
			free(tempC);

			ct++;
		}
		
		pgAndDirtyBitMap = NULL;
		firstpageMapPtr = NULL;
		firstnumPageFixedPtr = NULL;
		closePageFile(&fHandle);
	}
	if(firstpageMapPtr == NULL && firstnumPageFixedPtr == NULL 
			  && pgAndDirtyBitMap == NULL &&  FixNonZeroFlag == false && bm->mgmtData ==NULL)
		return RC_OK;
	else
		return RC_SHUT_DOWN_ERROR;
}
//##getNumReadIO:
//function returns the number of pages that have been read from disk since a buffer pool has been initialized.
int getNumReadIO(BM_BufferPool *const bm)
{
	return numReads;
}

//##getNumWriteIO:
//returns the number of pages written to the page file since the buffer pool has been initialized.
int getNumWriteIO(BM_BufferPool *const bm)
{
	return numWrites;
}

//#forceFlushPool :
//causes all dirty pages (with fix count 0) from the buffer pool to be written to disk.
RC forceFlushPool(BM_BufferPool *const bm)
{
	//Check for dirty frames and write them to disc.
	pgAndDirtyBitStruct *dirtyTempPtr = pgAndDirtyBitMap;
	frameList *frameTempPtr = firstFptr;
	while (dirtyTempPtr != NULL)
	{
		if(dirtyTempPtr->Dirty == 1)
		{
			pageMap *pageTempPtr = firstpageMapPtr;
			while(pageTempPtr->pageNum != dirtyTempPtr->pageNum)
			{
				pageTempPtr = pageTempPtr->nextpageMap;
			}
			writeBlock (pageTempPtr->discPageNum, &fHandle,frameTempPtr->frameData);
			dirtyTempPtr->Dirty = 0;
		}
		
		frameTempPtr = frameTempPtr->nextFramePtr;
		dirtyTempPtr = dirtyTempPtr->nxtPgDirtyBit;
	}
	return RC_OK;
}
