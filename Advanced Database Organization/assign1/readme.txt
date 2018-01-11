****************************************************************
	1.Personal information
****************************************************************
Name: Akash Gairola
CWID: A20403925
Email id: agairola@hawk.iit.edu

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
1. storage_mgr.c
2. storage_mgr.h
3. test_assign1.c
4. test_helper.h
5. dberror.c
6. dberror.h
7. Makefile
8. README

****************************************************************
	3.Compilation Instruction
****************************************************************

To compile and execute our code we used codeblocks IDE on windows 10.
We have also prepared make file which was used in Mingw on Windows 10. Inorder to run it use command "mingw32-make"
It will create object files, which can be executed using "test_assign1"

****************************************************************
	4. Functions Used
****************************************************************
#initStoreManager:

	Initializes the Storage Manager

#createPageFile:

	This function Opens the file in write mode and creates it.Write an emptyblock to the file

#openPageFile:

	This function Opens the file in read+write mode.Stores the totalNumPages, curPagePos, fileName. The pointer to file is stored in fHandle.mgmtInfo

#closePageFile:

	This function Closes the file using fclose. Deinitialize the file handler.

#destroyPageFile:

	This function Delete the file using the built-in function "remove"
#readBlock:

	This function Checks if pageNum is inside the bounds. In case the page exists, we use fseek to move the read pointer of our file where we want to start reading. Read the file into PageHandler from the seeked location using built-in function "fread"

#getBlockPos:

	This function returns the current block position using curPagePos variable of the file handler.

#readFirstBlock:

	This function calls the readBlock function with pageNum=curPagePos.

#readLastBlock:

	This function calls the readBlock function with pageNum=totalNumPages-1

#readPreviousBlock:

	It reads the previous page in the file. It receives the metadata(fHandle) and the variable where to save the page(memPage).

#readCurrentBlock:

	This functions calls the readBlock function with pageNum=curPagePos

#readNextBlock:

	This functions calls the readBlock function with pageNum=curPagePos+1
#writeBlock:

	This function Checks if pageNum is inside the bounds. In case the above condition is true, seek the file pointer until that page using built-in function "fseek". Write the data from the PageHandler to the seeked location using built-in function "fwite"

#writeCurrentBlock:

	This function calls the writeBlock function with pageNum=curPagePos


#appendEmptyBlock:

	This function appends a new page full of '\0' character

#EnsureCapacity:

	If the file has less than numberOfPages pages this function increases the size to numberOfPages value received. Additionally, this function receives the file metadata.

#UpdateCurrentInformation:

	This function updates totalNumPages, curPagePos.


****************************************************************
	5. Test Cases
****************************************************************
Additionally, we include an extra tests to check the validity more thoroughly for the store manager coded. We added the following test case

#Test testAppendEmptyBlockEnsureCapacity: Checks if appendEmptyBlock and ensureCapacity functions works as expected.
