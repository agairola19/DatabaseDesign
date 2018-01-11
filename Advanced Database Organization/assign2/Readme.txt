****************************************************************
	1.Personal information
****************************************************************
Name: Akash Gairola
CWID: A20395744
Email: agairola@hawk.iit.edu

Name: Muhammad Shareef
CWID: A20303179
Email: msharee3@hawk.iit.edu

Name: Alvaro Gomez-Hernandez
CWID: A20410966
Email: agomezhernandez@hawk.iit.edu

Name: Ashish
CWID: A20403925
Email: fashish@hawk.iit.edu


****************************************************************
	2. Files
****************************************************************
1.  buffer_mgr.c
2.  buffer_mgr.h
3.  buffer_mgr_stat.c
4.  buffer_mgr_stat.h
5.  DataStructs.h
6.  dberror.c
7.  dberror.h
8.  Makefile
9.  README
10. dt.h
11. storage_mgr.c 
12. storage_mgr.h
13. test_assign2_1.c
14. test_assign2_2.c
15. test_helper.h

****************************************************************
	3.Compilation Instruction
****************************************************************

To compile and execute our code we used codeblocks IDE on windows 10.
We have also prepared make file which was used in Mingw on Windows 10. Inorder to run it use command "mingw32-make"
It will create object files, which can be executed using "test_assign2_1" and "test_asign2_2"

****************************************************************
	4. Functions Used
****************************************************************
##BUFFER POOL FUNCTIONS ##

#initBufferPool 
creates a new buffer pool with numPages page frames using the page replacement strategy strategy. The pool is used to cache pages from the page file with name pageFileName

#shutdownBufferPool  
destroys a buffer pool. This method should free up all resources associated with buffer pool. For example, it should free the memory allocated for page frames

#forceFlushPool 
causes all dirty pages (with fix count 0) from the buffer pool to be written to disk.

##PAGE MANAGEMENT FUNCTIONS##

#pinPage 
pins the page with page number pageNum. The buffer manager is responsible to set the pageNum field of the page handle passed to the method. Similarly, the data field should point to the page frame the page is stored in (the area in memory storing the content of the page).

#unpinPage 
unpins the page page. The pageNum field of page should be used to figure out which page to unpin.

#markDirty 
marks a page as dirty.

#forcePage
should write the current content of the page back to the page file on disk.

##STATISTICS FUNCTIONS##

##getFrameContents 
returns an array of PageNumbers (of size numPages) where the ith element is the number of the page stored in the ith page frame. An empty page frame is represented using the constant NO_PAGE.

##getDirtyFlags  
returns an array of bools (of size numPages) where the ith element is TRUE if the page stored in the ith page frame is dirty. Empty page frames are considered as clean.

##getFixCounts 
function returns an array of ints (of size numPages) where the ith element is the fix count of the page stored in the ith page frame. 

##getNumReadIO 
function returns the number of pages that have been read from disk since a buffer pool has been initialized.

##getNumWriteIO 
returns the number of pages written to the page file since the buffer pool has been initialized.

****************************************************************
	5. Test Cases
****************************************************************

test_asign2_1 file implements several test cases using the buffer_mgr.h interface using the FIFO strategy and LRU.

test_asign2_2 file implements several test cases using the buffer_mgr.h interface using CLOCK and Least Frequently Used Algorithm.

We have also included additional error check

  1) #define RC_BUFFER_POOL_INIT_ERROR 500 which represents Buffer Initialization error

  2) #define RC_NO_FRAME 501 which represents No free frames RC

  3) #define RC_DIRTY_UPDATE_FAILED 502 which represents Dirty update failure

  4) #define RC_UNPIN_FAILED 503 which represents Unpin Failure error

  5) #define RC_SHUT_DOWN_ERROR 504 which represents Buffer Pool Shutdown error

