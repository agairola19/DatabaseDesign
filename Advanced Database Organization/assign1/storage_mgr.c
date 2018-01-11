#include "storage_mgr.h"
#include "dberror.h"
#include <stdio.h> // Defines three variable types, several macros, and various functions for performing input and output
#include <errno.h> // (standard error nums for built in library functions, such as fwrite or fread)
// The errno.h header file of the C Standard Library defines the integer variable errno,
// which is set by system calls and some library functions in the event of an error to indicate what went wrong.
// This macro expands to a modifiable lvalue of type int, therefore it can be both read and modified by a program.
// The errno is set to zero at program startup. Certain functions of the standard C library modify
// its value to other than zero to signal some types of error. You can also modify its value or reset to zero at your convenience.
// The errno.h header file also defines a list of macros indicating different error codes, which will expand to integer constant expressions with type int.
#include <stdlib.h> // Defines four variable types, several macros, and various functions for performing general functions.
#include <string.h> // Defines one variable type, one macro, and various functions for manipulating arrays of characters


int SM_initialized = 0;

typedef struct SM_FileHeader {
  int totalNumPages;
  int curPagePos;
} SM_FileHeader;


/************************************************************
 *                   Interface								*
 ************************************************************/

/**************************************************************************************************
 * Function Name: initStorageManager
 * Description: Initializes the Storage Manager
 **************************************************************************************************/
 void initStorageManager (void){
  SM_initialized=1;
printf("Welcome! Storage manager has been initialized!");

};

RC updateInfo(int totalNumPages, int curPagePos, SM_FileHandle *fHandle){
  // fHandle is updated
  fHandle->totalNumPages = totalNumPages;
  fHandle->curPagePos = curPagePos;

  // a SM_FileHeader struct is created to be written to the file
  struct SM_FileHeader fileInfo;
  fileInfo.totalNumPages = fHandle->totalNumPages;
  fileInfo.curPagePos = fHandle->curPagePos;

  // we want to write at the start of the file
  errno = 0;
  //int fseek(FILE *stream, long int offset, int whence)
  //stream − This is the pointer to a FILE object that identifies the stream.
  //offset − This is the number of bytes to offset from whence.
  //whence − This is the position from where offset is added. It is specified by one of the following constants −

  fseek(fHandle->mgmtInfo, 0, SEEK_SET); //SEEK_SET – It moves file pointer position to the beginning of the file.
  if(errno != 0) return RC_FILE_HANDLE_NOT_INIT;
  errno = 0;
  fwrite(&fileInfo, sizeof(fileInfo), 1, fHandle->mgmtInfo);
  if(errno != 0) return RC_WRITE_FAILED;
  return RC_OK;
}




/**************************************************************************************************
 * Function Name: createPageFile
 * Description:   Open the file in write mode and creates it.Write an emptyblock to the file
 **************************************************************************************************/
 RC createPageFile (char *fileName){
  FILE *myfile = fopen(fileName, "w"); // Creates pointer called myfile
  // If the file exists, then
  if(myfile){
    char myeof = '\0'; // myeof is marked by this character, from specifications

    struct SM_FileHeader myFileInf; // Creates a FileHeader called myFileInf
    myFileInf.totalNumPages = 1; //
    myFileInf.curPagePos = 0;

    // SM_FileHeader struct will be stored at the start of the document
    // It will only contains totalNumPages and curPagePos

    errno = 0; //no error, so err num 0
    //writes data from the array pointed to, by ptr to the given stream.
    //size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
    //ptr − This is the pointer to the array of elements to be written.
    //size − This is the size in bytes of each element to be written.
    //nmemb − This is the number of elements, each one with a size of size bytes.
    //stream − This is the pointer to a FILE object that specifies an output stream.

    fwrite(&myFileInf, sizeof(SM_FileHeader), 1, myfile);

    if(errno != 0){
      fclose(myfile);
      return RC_WRITE_FAILED;
    }
    else{
      int x=0;
      // Writes the first page full of '\0' (null value in C is \0)
      for (x = 0; x < PAGE_SIZE; ++x){
        errno = 0;
        fwrite(&myeof, sizeof(myeof), 1, myfile);
        if(errno != 0){
          fclose(myfile);
          return RC_WRITE_FAILED;
        }
      }
      // to close the file after we write to it successfully
      errno = 0;
      fclose(myfile);
      if(errno == 0){
        return RC_OK;
      }else{
        return RC_WRITE_FAILED;
      }
    }
  }

  else{
  // If the file doesnt exist, so file handle is not initialized
  return RC_FILE_HANDLE_NOT_INIT;
  }
};

/**************************************************************************************************
 * Function Name: OpenPageFile
 * Description: Open the file in read+write mode
				Store the totalNumPages, curPagePos, fileName
				The pointer to file is stored in fHandle.mgmtInfo
 **************************************************************************************************/
// Opens an existing page file
 RC openPageFile (char *fileName, SM_FileHandle *fHandle){
  //FILE *fopen(const char *filename, const char *mode)
  /*"r" Opens a file for reading. The file must exist.
  "w" Creates an empty file for writing. If a file with the same name already exists, its content is erased and the file is considered as a new empty file.
  "a" Appends to a file. Writing operations, append data at the end of the file. The file is created if it does not exist.
  "r+"  Opens a file to update both reading and writing. The file must exist.
  "w+"  Creates an empty file for both reading and writing.
  "a+"  Opens a file for reading and appending.
  */
  FILE *myfile = fopen(fileName, "r+"); //fopen is another built in function
  struct SM_FileHeader myfileInf;

  if (myfile){
    // First the metadata (from the start of the file)
    errno = 0;
    fread(&myfileInf, sizeof(SM_FileHeader), 1, myfile);

    if(errno != 0){
      return RC_FILE_HANDLE_NOT_INIT;
    }

    else{
      // Initialization of the the SM_FileHandle struct
      fHandle->fileName = fileName;
      fHandle->totalNumPages = myfileInf.totalNumPages;
      fHandle->curPagePos = myfileInf.curPagePos;
      fHandle->mgmtInfo = myfile;
      // The pointer to file is stored in fHandle.mgmtInfo.
      // So if we want to close the file, we do fclose(fHandle.mgmtInfo)
      // we are using mgmtInfo to store the file handle needed
      // to write, read, close and other I/O operations
      return RC_OK;
    }
  }else{
    return RC_FILE_NOT_FOUND;
  }
};

/**************************************************************************************************
 * Function Name: ClosePageFile
 * Description: Close the file using fclose
				Deinitialize the file handler
 **************************************************************************************************/

// Close an open file
 RC closePageFile (SM_FileHandle *fHandle){
  int var = fclose(fHandle->mgmtInfo);
  // Upon successful completion, fclose() shall return 0; otherwise, it shall return myeof
  if(!var){
    return RC_OK;
  }
  else{
    return RC_FILE_NOT_FOUND;
  }
}
/**************************************************************************************************
 * Function Name: destroyPageFile
 * Description: Delete the file using the built-in function "remove"
 **************************************************************************************************/
// Delete an open file
 RC destroyPageFile (char *fileName){
  // On success, zero is returned. On error, -1 is returned, and errno is set appropriately.
  int var = remove(fileName);
  if (!var){
    return RC_OK;
  }
  else{
    return RC_FILE_NOT_FOUND;
  }
};



//BEGIN MUHAMMAD'S CODE


/**************************************************************************************************
 * Function Name: readBlock
 * Description: Checks if pageNum is inside the bounds
				In case the page exists, we use fseek to move the read pointer of our file where we want to start reading
 				Read the file into PageHandler from the seek location using built-in function "fread"
 **************************************************************************************************/
 RC readBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
{

	FILE *fileStream=fHandle->mgmtInfo;
	int seekpage= PAGE_SIZE*pageNum;
	int offset = sizeof(SM_FileHeader)+seekpage;
		if (SM_initialized == 1)
    {
  // Checks if pageNum is inside the bounds
    if(pageNum < 0 || pageNum >= fHandle->totalNumPages)
    return RC_READ_NON_EXISTING_PAGE;

  // we use fseek to move the read pointer of our file
  // where we want to start reading

  errno = 0;
  fseek(fileStream, offset, SEEK_SET);
  if(errno != 0) return RC_FILE_HANDLE_NOT_INIT;
  errno = 0;
  fread(memPage, PAGE_SIZE, 1, fileStream);
  if(errno != 0) return RC_FILE_HANDLE_NOT_INIT;

  // this function will update metadata in fHandle struct and
  // it will write the metadata to the file
  updateInfo(fHandle->totalNumPages, pageNum, fHandle);
  return RC_OK;


		}
		else
		{
			return RC_STORAGE_MGR_NOT_INIT;
		}
}

/**************************************************************************************************
 * Function Name: getBlockPos
 * Description: returns the current block position using curPagePos variable of the file handler
 **************************************************************************************************/
 int getBlockPos (SM_FileHandle *fHandle)
{
	if(fHandle) //first we check if there is fHandle
		return fHandle->curPagePos;
	else return RC_FILE_HANDLE_NOT_INIT;
};
/**************************************************************************************************
 * Function Name: readFirstBlock
 * Description: Calls the readBlock function with pageNum=0
 **************************************************************************************************/
 RC readFirstBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
//just call readBlock with pageNum as 0 because first page is page 0
return readBlock(0, fHandle, memPage);
};

/**************************************************************************************************
 * Function Name: readPreviousBlock
 * Description: Calls the readBlock function with pageNum=curPagePos-1
 **************************************************************************************************/
 RC readPreviousBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
//call readBlock with curPagePos - 1, to ge thte previous
return readBlock(fHandle->curPagePos-1, fHandle, memPage);
};
/**************************************************************************************************
 * Function Name: readCurrentBlock
 * Description: Calls the readBlock function with pageNum=curPagePos
 **************************************************************************************************/
 RC readCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
//call readBlock with curPagePos
return readBlock(fHandle->curPagePos, fHandle, memPage);
};

/**************************************************************************************************
 * Function Name: readNextBlock
 * Description: Calls the readBlock function with pageNum=curPagePos+1
 **************************************************************************************************/
 RC readNextBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
//call readBlock with curPagePos +1, to get the next
return readBlock(fHandle->curPagePos+1, fHandle, memPage);
};

/**************************************************************************************************
 * Function Name: readLastBlockBlock
 * Description: Calls the readBlock function with pageNum=totalNumPages-1
 **************************************************************************************************/
 RC readLastBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
//call readBlock with pageNum = totalNumPages-1(last page)
 return readBlock(fHandle->totalNumPages-1, fHandle, memPage);
};


/* writing blocks to a page file
 RC writeBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage);
 RC writeCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage);
 RC appendEmptyBlock (SM_FileHandle *fHandle);
 RC ensureCapacity (int numberOfPages, SM_FileHandle *fHandle);
*/



//BEGIN AKASH'S CODE



/**************************************************************************************************
 * Function Name: writeBlock
 * Description: Checks if pageNum is inside the bounds
				In case the above condition is true, seek the file pointer until that page using built-in function "fseek"
				Write the data from the PageHandler to the seeked location using built-in function "fwite"
 **************************************************************************************************/

RC writeBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage){
  // Checks if pageNum is inside the bounds
  if(pageNum < 0 || pageNum >= fHandle->totalNumPages)
    return RC_READ_NON_EXISTING_PAGE;

  // we use fseek to move the read pointer of our file
  // where we want to start writing
  errno = 0;
  int seekLb1 = fseek(fHandle->mgmtInfo, sizeof(SM_FileHeader)+pageNum*PAGE_SIZE, SEEK_SET); //moving to the start of the data and returns value to label
  if (seekLb1 != 0) {
		return RC_WRITE_FAILED;
	}
 // edited this line and used above condition if(errno != 0) return RC_FILE_HANDLE_NOT_INIT;


  errno = 0;
  fwrite(memPage, PAGE_SIZE, 1, fHandle->mgmtInfo); //just use fwrite to write to that page
  if(errno != 0) return RC_WRITE_FAILED;
  return RC_OK;
};

/**************************************************************************************************
 * Function Name: writeCurrentBlock
 * Description: Calls the writeBlock function with pageNum=curPagePos
 **************************************************************************************************/
RC writeCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){
  // Current block has pageNum = curPagePos
  // readBlock function will check if pageNum is inside the bounds
  RC rc = writeBlock(fHandle->curPagePos, fHandle, memPage); //return code, just added rc and then returning rc
  return rc;
};

/**************************************************************************************************
 * Function Name: appendEmptyBlock
 * Description:   Appends a new page full of '\0' character
 **************************************************************************************************/
RC appendEmptyBlock (SM_FileHandle *fHandle){
  // the file is opened as 'r+'
  // we need to close and reopen as 'a'
 /*
  fclose(fHandle->mgmtInfo);

  FILE *file = fopen(fHandle->fileName, "a"); //a option appends

	"r"	Opens a file for reading. The file must exist.
	"w"	Creates an empty file for writing. If a file with the same name already exists, its content is erased and the file is considered as a new empty file.
	"a"	Appends to a file. Writing operations, append data at the end of the file. The file is created if it does not exist.
	"r+"	Opens a file to update both reading and writing. The file must exist.
	"w+"	Creates an empty file for both reading and writing.
	"a+"	Opens a file for reading and appending.

  if(file){
    char eof = '\0';
    int i;
    // we append a new page full of '\0' character
    for(i = 0; i < PAGE_SIZE; i++){
      errno = 0;
      fwrite(&eof, sizeof(eof), 1, file);
      if(errno != 0) return RC_WRITE_FAILED;
    }

    // close file
    errno = 0;
    fclose(file);
    if(errno != 0) return RC_WRITE_FAILED;
    // we reopen again as 'r+'
    openPageFile(fHandle->fileName, fHandle);
    // we need to update metadata information, totalNumPages has been increased
    updateInfo(fHandle->totalNumPages+1, fHandle->curPagePos, fHandle);
    return RC_OK;
  }
  else
  {
    return RC_FILE_NOT_FOUND;
  }
   */

//	Other code logic which we can consider. Using a calloc function, it has created a empty page as calloc function intializes block dynamically and stores value as null
	RC rc = -9999; // return code, initialized to -9999

	// pointer to file being handled
	FILE *fp = fHandle->mgmtInfo;

	if (fp == NULL) {
		return RC_FILE_NOT_FOUND;
	}
	// create  empty page
	SM_PageHandle emptyPage = (SM_PageHandle) calloc(PAGE_SIZE, sizeof(char));

	// move  pointer to EOF
	int seekLabel = fseek(fp, 0L, SEEK_END);

	if (seekLabel != 0) {
		return RC_SEEK_POSITION_ERROR;
	}
	// write into the file, return the written size
	// TODO - what if only part of the stream is written to the file?
	int writtenSize = fwrite(emptyPage, sizeof(char), PAGE_SIZE,
			fHandle->mgmtInfo);

	if (writtenSize != PAGE_SIZE) {
		rc = RC_WRITE_FAILED;
	} else {
		fHandle->curPagePos = fHandle->totalNumPages++;
		rc = RC_OK;
	}
	// free memory
	free(emptyPage);
	return rc;

};


/**************************************************************************************************
 * Function Name: ensureCapacity
 * Description: how many pages we need to append,
				if the number is negative or 0, we dont need to append any page
 **************************************************************************************************/
RC ensureCapacity (int numberOfPages, SM_FileHandle *fHandle){
   if (fHandle->totalNumPages >= numberOfPages) {
		return RC_NO_NEED_TO_ENSURE;
	}

  // how many pages we need to append
  int numPagesToEnsure = numberOfPages - fHandle->totalNumPages;
  // if the number is negative or 0, we dont need to append any page
  if(numPagesToEnsure > 0){
    int i;
    for(i = 0; i < numPagesToEnsure; i++){
      RC res = appendEmptyBlock(fHandle);
      if(res != RC_OK) return res;
    }
  }
  return RC_OK;

};



