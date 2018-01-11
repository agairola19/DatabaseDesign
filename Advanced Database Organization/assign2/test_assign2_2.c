#include "storage_mgr.h"
#include "buffer_mgr_stat.h"
#include "buffer_mgr.h"
#include "dberror.h"
#include "test_helper.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *testName;

// check whether two the content of a buffer pool is the same as an expected content
// (given in the format produced by sprintPoolContent)
#define ASSERT_EQUALS_POOL(expected,bm,message)			        \
  do {									\
    char *real;								\
    char *_exp = (char *) (expected);                                   \
    real = sprintPoolContent(bm);					\
    if (strcmp((_exp),real) != 0)					\
      {									\
	printf("[%s-%s-L%i-%s] FAILED: expected <%s> but was <%s>: %s\n",TEST_INFO, _exp, real, message); \
	free(real);							\
	exit(1);							\
      }									\
    printf("[%s-%s-L%i-%s] OK: expected <%s> and was <%s>: %s\n",TEST_INFO, _exp, real, message); \
    free(real);								\
  } while(0)

static void clockTest (void);
static void LFUtest (void);
static void createDummyPages(BM_BufferPool *bm, int num);

int
main (void)
{
initStorageManager();
testName = "";
LFUtest();
printf("LFU test completed\n\n\n");
clockTest();
printf("clock test completed\n\n\n");
  return 0;
}

void LFUtest()
{
	// expected results
	  const char *contents[] = {
		//read first three pages and directly unpin them
	    "[0 0],[-1 0],[-1 0]" ,
	    "[0 0],[1 0],[-1 0]",
	    "[0 0],[1 0],[2 0]",
		//Pin page 0 3x, page 1 2x
		"[0 3],[1 2],[2 0]",
		//Pin new page, should replace page 2, the LFU
		"[0 3],[1 2],[4 1]",
		//Pin page 4 3x
		"[0 3],[1 2],[4 4]",
		//Pin new page, should replace the LFU
		"[0 3],[5 1],[4 4]",
		//Unpin page 0 3x, page 5 once and page 4 four times.
		"[0 0],[5 0],[4 0]",
	  };
	  int i;
	  const int reqs[] = {0,1,2,3,4,5,6};
	  const int LinearReqsNum = 3;
	 
	  BM_BufferPool *bm = MAKE_POOL();
	  BM_PageHandle *h = MAKE_PAGE_HANDLE();
	  testName = "Testing LFU page replacement";

	  CHECK(createPageFile("testbuffer.bin"));
	  createDummyPages(bm, 100);
	  CHECK(initBufferPool(bm, "testbuffer.bin", 3, RS_LFU, NULL));
	  for(i = 0; i < LinearReqsNum; i++)
	    {
	      pinPage(bm, h, reqs[i]);
	      unpinPage(bm, h);
	      ASSERT_EQUALS_POOL(contents[i], bm, "check pool content");
	    }
	  pinPage(bm, h, 0);
	  pinPage(bm, h, 0);
	  pinPage(bm, h, 0);
	  pinPage(bm, h, 1);
	  pinPage(bm, h, 1);
	  ASSERT_EQUALS_POOL(contents[i++],bm,"pool content after pin page");
	  pinPage(bm, h, 4);
	  ASSERT_EQUALS_POOL(contents[i++],bm,"pool content after pin page");
	  pinPage(bm,h,4);
	  pinPage(bm,h,4);
	  pinPage(bm,h,4);
	  ASSERT_EQUALS_POOL(contents[i++],bm,"pool content after pin page");
	  pinPage(bm,h,5);
	  ASSERT_EQUALS_POOL(contents[i++],bm,"pool content after pin page");
	  h->data = "Page-0";
	  h->pageNum = 0;
	  unpinPage(bm,h);
	  unpinPage(bm,h);
	  unpinPage(bm,h);
	  h->data = "Page-5";
	  h->pageNum = 5;
	  unpinPage(bm,h);
	  h->data = "Page-4";
	  h->pageNum = 4;
	  unpinPage(bm,h);
	  unpinPage(bm,h);
	  unpinPage(bm,h);
	  unpinPage(bm,h);
 	  forceFlushPool(bm);
	  ASSERT_EQUALS_POOL(contents[i],bm,"pool content after flush");
	  // check num writes, should be 0 because we didnt have any dirty pages
	  ASSERT_EQUALS_INT(0, getNumWriteIO(bm), "check number of write I/Os");
	  ASSERT_EQUALS_INT(5, getNumReadIO(bm), "check number of read I/Os");
	  CHECK(shutdownBufferPool(bm));
	  CHECK(destroyPageFile("testbuffer.bin"));
	  free(bm);
	  free(h);
	  TEST_DONE();
}

void clockTest()
{
	//expected output
	  const char *contents[] = {
			  "[0 0],[-1 0],[-1 0]",
			  	  "[0 0],[4 0],[-1 0]",
			  	  "[0 0],[4 0],[1 0]",
			  	  "[0 0],[4 0],[1 0]",
			  	  "[2 0],[4 0],[1 0]",
			  	  "[2 0],[4 0],[1 0]",
			  	  "[2 0],[4 0],[3 0]",
			  	  "[2 0],[4 0],[3 0]",
			  	  "[2 0],[4 0],[3 0]",
			  	  "[2 0],[4 0],[3 0]",
			  	  "[2 0],[4 0],[0 0]",
			  	  "[2 0],[4 0],[0 0]",
			  	  "[1 0],[4 0],[0 0]",
			  	  "[1 0],[4 0],[0 0]",
			  	  "[1 0],[4 0],[2 0]",
			  	  "[1 0],[4 0],[2 0]",
			  	  "[3 0],[4 0],[2 0]",
			  	  "[3 0],[4 0],[2 0]",
				  "[3 0],[4 0],[2 0]",
	  };
	  const int reqs[] = {0,4,1,4,2,4,3,4,2,4,0,4,1,4,2,4,3,4};
	  int i;
	  BM_BufferPool *bm = MAKE_POOL();
	  BM_PageHandle *h = MAKE_PAGE_HANDLE();
	  testName = "Testing CLOCK strategy";
	  CHECK(createPageFile("testbuffer.bin"));
	  createDummyPages(bm, 100);
	  CHECK(initBufferPool(bm, "testbuffer.bin", 3, RS_CLOCK, NULL));
//iterating through and pinning and unpinning pages
 for(i = 0; i < 18; i++)
   {
     pinPage(bm, h, reqs[i]);
     unpinPage(bm, h);
     ASSERT_EQUALS_POOL(contents[i], bm, "check pool content");
   }
 forceFlushPool(bm);
 ASSERT_EQUALS_POOL(contents[i],bm,"pool content after flush");
 // numWrite check
 ASSERT_EQUALS_INT(0, getNumWriteIO(bm), "check writeNum");
 ASSERT_EQUALS_INT(9, getNumReadIO(bm), "check readNum");
 CHECK(shutdownBufferPool(bm));
 CHECK(destroyPageFile("testbuffer.bin"));
 free(bm);
 free(h);
 TEST_DONE();
}

void
createDummyPages(BM_BufferPool *bm, int num)
{
  int i;
  BM_PageHandle *h = MAKE_PAGE_HANDLE();

  CHECK(initBufferPool(bm, "testbuffer.bin", 3, RS_FIFO, NULL));

  for (i = 0; i < num; i++)
    {
      CHECK(pinPage(bm, h, i));
      sprintf(h->data, "%s-%i", "Page", h->pageNum);
      CHECK(markDirty(bm, h));
      CHECK(unpinPage(bm,h));
    }

  CHECK(shutdownBufferPool(bm));

  free(h);
}
