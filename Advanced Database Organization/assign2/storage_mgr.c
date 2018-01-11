#include "storage_mgr.h"
#include "dberror.h"
#include <stdio.h> // Defines three variable types, several macros, and various functions for performing input and output
#include <errno.h> // (standard error nums for built in library functions, such as fwrite or fread)
// The errno.h header file of the C Standard Library defines the integer variable errno,
// which is set by system calls and some library functions in the event of an error to indicate what went wrong.
// This macro expands to a modifiable lvalue of type int, therefore it can be both read and modified by a program.
// The errno is set to zero at program startup. Certain functions of the standard C library modify
// its Value to other than zero to signal some types of error. You can also modify its Value or reset to zero at your convenience.
// The errno.h header file also defines a list of macros indicating different error codes, which will expand to integer constant expressions with type int.
#include <stdlib.h> // Defines four variable types, several macros, and various functions for performing general functions.
#include <string.h> // Defines one variable type, one macro, and various functions for manipulating arrays of characters

#include "test_helper.h"
#include <sys/stat.h>

typedef struct _metaDataList 
{
		char Key[50];
		char Value[50];
		struct _metaDataList *nextMetaDataNode;
} metadataStruct;

metadataStruct *currentNode = NULL;
metadataStruct *previousNode = NULL;
metadataStruct *firstNode = NULL;

int CreateCalling = 1;
/************************************************************
 *                   Interface								*
 ************************************************************/

/**************************************************************************************************
 * Function Name: initStorageManager
 * Description: Initializes the Storage Manager
 **************************************************************************************************/
 
void initStorageManager()
{

}

void getMetaData(int n, char * string,char *KeyValNthPair)
{
	int PosDel = 0;
	int k;
	int PosCurrent = 0;
	char DelimitPos[1000];
	
	char newString[PAGE_SIZE];
	memset(newString, '\0', PAGE_SIZE);
	newString[0] = ';';
	strcat(newString, string);
for (k = 0; k < strlen(newString); k++)
{
	if (newString[k] == ';')
	{
	DelimitPos[PosDel] = k;
	PosDel++;
	}
}
		for (k = DelimitPos[n - 1] + 1;k <= DelimitPos[n] - 1; k++)
	{
		KeyValNthPair[PosCurrent] = newString[k];
		PosCurrent++;
	}
	KeyValNthPair[PosCurrent] = '\0';
}


metadataStruct * constructMetaDataLinkedList(char *metaInformation,
		int numNodesNeeded)
{
	int k;
	char CurrentKey[50];
	char CurrentValue[50];
	char MetaKeyValue[100];
	memset(CurrentKey,'\0',50);
	memset(CurrentValue,'\0',50);
	for (k = 1; k <= numNodesNeeded; k++)
	{
	memset(MetaKeyValue,'\0',100);
	getMetaData(k, metaInformation,MetaKeyValue);
	char FoundColon = 'N';
	int CounterKey = 0;
	int ValueCounter = 0;
	int i;
		for (i = 0; i < strlen(MetaKeyValue); i++)
		{
		if (MetaKeyValue[i] == ':')
			FoundColon = 'Y';
		if (FoundColon == 'N')
			CurrentKey[CounterKey++] = MetaKeyValue[i];
		else if (MetaKeyValue[i] != ':')
			CurrentValue[ValueCounter++] = MetaKeyValue[i];
		}
		CurrentKey[CounterKey] = '\0';
		CurrentValue[ValueCounter] = '\0';
		currentNode = (metadataStruct *) malloc(sizeof(metadataStruct));
		strcpy(currentNode->Value,CurrentValue);
		strcpy(currentNode->Key,CurrentKey);
		currentNode->nextMetaDataNode = NULL;
		if (k == 1)
		{
			firstNode= currentNode;
			previousNode = NULL;
		}
		else
		{
			previousNode->nextMetaDataNode = currentNode;
		}
		previousNode = currentNode;
	}
	return firstNode;
}
/**************************************************************************************************
 * Function Name: createPageFile
 * Description:   Open the file in write mode and creates it.Write an emptyblock to the file
 **************************************************************************************************/
RC createPageFile(char *filename)
{
	FILE *fp;
	fp = fopen(filename, "a+b"); 
	if (CreateCalling == 1) 
	{
		if (fp != NULL)
		{
	char nullString2[PAGE_SIZE]; 
	char nullString3[PAGE_SIZE]; 
	char stringPageSize[5];
	sprintf(stringPageSize, "%d", PAGE_SIZE);
	char strMetaInfo[PAGE_SIZE * 2];
	strcpy(strMetaInfo, "PS:"); 
	strcat(strMetaInfo, stringPageSize);
	strcat(strMetaInfo, ";");
	strcat(strMetaInfo, "NP:0;"); 
	int i;
	for (i = strlen(strMetaInfo); i < (PAGE_SIZE * 2); i++)
		strMetaInfo[i] = '\0';
	memset(nullString2, '\0', PAGE_SIZE);
	memset(nullString3, '\0', PAGE_SIZE);
	fwrite(strMetaInfo, PAGE_SIZE, 1, fp);
	fwrite(nullString2, PAGE_SIZE, 1, fp);
	fwrite(nullString3, PAGE_SIZE, 1, fp);
	fclose(fp);
	return RC_OK;
		} else
		{
			return RC_FILE_NOT_FOUND;
		}
}
else
	{
	if (fp != NULL)
		{
		char nullString[PAGE_SIZE];
		memset(nullString, '\0', PAGE_SIZE);
		fwrite(nullString, PAGE_SIZE, 1, fp);
		fclose(fp);
		return RC_OK;
	} else
	{
		return RC_FILE_NOT_FOUND;
	}
}

}
/**************************************************************************************************
 * Function Name: OpenPageFile
 * Description: Open the file in read+write mode
				Store the totalNumPages, curPagePos, fileName
				The pointer to file is stored in fHandle.mgmtInfo
 **************************************************************************************************/

RC openPageFile(char *fileName, SM_FileHandle *fHandle)
{
	struct stat statistics;
	FILE *fp;
 //FILE *fopen(const char *filename, const char *mode)
  /*"r" Opens a file for reading. The file must exist.
  "w" Creates an empty file for writing. If a file with the same name already exists, its content is erased and the file is considered as a new empty file.
  "a" Appends to a file. Writing operations, append data at the end of the file. The file is created if it does not exist.
  "r+"  Opens a file to update both reading and writing. The file must exist.
  "w+"  Creates an empty file for both reading and writing.
  "a+"  Opens a file for reading and appending.
  */
	fp = fopen(fileName, "r");
	if (fp != NULL)
	{
		fHandle->fileName = fileName;
		fHandle->curPagePos = 0;
		stat(fileName, &statistics);
		fHandle->totalNumPages = (int) statistics.st_size / PAGE_SIZE;
		fHandle->totalNumPages -= 2; 
		char metaDataInformationString[PAGE_SIZE * 2];
		fgets(metaDataInformationString, (PAGE_SIZE * 2), fp);
		int k;
		int numNodes = 0;
		for (k = 0; k < strlen(metaDataInformationString); k++)
			if (metaDataInformationString[k] == ';')
				numNodes++;
		fHandle->mgmtInfo = constructMetaDataLinkedList(
				metaDataInformationString, numNodes);
		fclose(fp);
		return RC_OK;
	}
	else
	{
		return RC_FILE_NOT_FOUND;
	}
}

void convertToString(int someNumber,char * arrayReverse)
{
	int j=0;
	int i = 0;
	int x;
	char temp1[4];
	memset(temp1, '\0', 4);
	while (someNumber != 0)
	{
		temp1[i++] = (someNumber % 10) + '0';
		someNumber /= 10;
	}
	temp1[i] = '\0';
	for(x = strlen(temp1)-1;x>=0;x--)
	{
		arrayReverse[j++] = temp1[x];
	}
	arrayReverse[j]='\0';
}

RC writeToFile(SM_FileHandle *fHandle,char *dataToWrite)
{
	FILE *fp = fopen(fHandle->fileName,"r+b");

	if(fp != NULL)
	{
		fwrite(dataToWrite,1,PAGE_SIZE,fp);
		fclose(fp);
		return RC_OK;
	}
	else
	{
		return RC_WRITE_FAILED;
	}
}
void freeMemory()
{
metadataStruct *previousNode;
metadataStruct *current  = firstNode;
previousNode = firstNode;
while(current != NULL)
	{
	current = current->nextMetaDataNode;
	if(previousNode!=NULL)
	free(previousNode);
	previousNode = current;
	}
	previousNode = NULL;
	firstNode = NULL;
}
/**************************************************************************************************
 * Function Name: ClosePageFile
 * Description: Close the file using fclose
				Deinitialize the file handler
 **************************************************************************************************/

// Close an open file
RC closePageFile(SM_FileHandle *fHandle)
{
	if (fHandle != NULL)
	{
		metadataStruct *temp = firstNode;
		char string[4];
		memset(string,'\0',4);
		while (1 == 1)
		{
			if(temp != NULL)
			{
				if(temp->Key != NULL)
				{
					if (strcmp(temp->Key, "NP") == 0)
					{
						convertToString(fHandle->totalNumPages,string);
						strcpy(temp->Value,string);
						break;
					}
				}
				temp = temp->nextMetaDataNode;
			}
			else
				break;
		}
		temp = firstNode;
		char metaData[2 * PAGE_SIZE];
		memset(metaData, '\0', 2 * PAGE_SIZE);
		int i = 0;
		while (temp != NULL)
		{
			int valueCounter = 0;
			int CounterKey = 0;
			while (temp->Key[CounterKey] != '\0')
			metaData[i++] = temp->Key[CounterKey++];
			metaData[i++] = ':';
			while (temp->Value[valueCounter] != '\0')
			metaData[i++] = temp->Value[valueCounter++];
			metaData[i++] = ';';
			temp = temp->nextMetaDataNode;
		}
		writeToFile(fHandle,metaData);
		fHandle->curPagePos = 0;
		fHandle->fileName = NULL;
		fHandle->mgmtInfo = NULL;
		fHandle->totalNumPages = 0;
		fHandle = NULL;
		freeMemory();
		return RC_OK;
	}
	else
	{
		return RC_FILE_HANDLE_NOT_INIT;
	}
}
/**************************************************************************************************
 * Function Name: destroyPageFile
 * Description: Delete the file using the built-in function "remove"
 **************************************************************************************************/
// Delete an open file
 RC destroyPageFile(char *fileName)
// On success, zero is returned. On error, -1 is returned, and errno is set appropriately.
{	if (remove(fileName) == 0)
		return RC_OK;
	else
		return RC_FILE_NOT_FOUND;
}
/**************************************************************************************************
 * Function Name: writeBlock
 * Description: Checks if pageNum is inside the bounds
				In case the above condition is true, seek the file pointer until that page using built-in function "fseek"
				Write the data from the PageHandler to the seeked location using built-in function "fwite"
 **************************************************************************************************/
RC writeBlock(int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	// Checks if pageNum is inside the bounds
	if (pageNum < 0 || pageNum > fHandle->totalNumPages)//
		return RC_WRITE_FAILED;
	else
	{
		int startPosition = (pageNum * PAGE_SIZE) + (2 * PAGE_SIZE);
		FILE *fp = fopen(fHandle->fileName, "r+b");
		if (fp != NULL)
		{
			 // we use fseek to move the read pointer of our file
			 // where we want to start writing
			if (fseek(fp, startPosition, SEEK_SET) == 0)
			{
				fwrite(memPage, 1, PAGE_SIZE, fp);
				if (pageNum > fHandle->curPagePos)
					fHandle->totalNumPages++;
				fHandle->curPagePos = pageNum;
				fclose(fp);
				return RC_OK;
			}
			else
			{
				return RC_WRITE_FAILED;
			}
		} 
		else
		{
			return RC_FILE_HANDLE_NOT_INIT;
		}
	}
}
/**************************************************************************************************
 * Function Name: appendEmptyBlock
 * Description:   Appends a new page full of '\0' character
 **************************************************************************************************/
RC appendEmptyBlock(SM_FileHandle *fHandle)
{
	CreateCalling = 2;
	if (createPageFile(fHandle->fileName) == RC_OK)
	{
		//Update value of totalNumPages and curPagePos 
		fHandle->totalNumPages++;
		fHandle->curPagePos = fHandle->totalNumPages - 1;
		CreateCalling = 1;
		return RC_OK;
	}
	else
	{
		CreateCalling = 1;
		return RC_WRITE_FAILED;
	}
}
/**************************************************************************************************
 * Function Name: writeCurrentBlock
 * Description: Calls the writeBlock function with pageNum=curPagePos
 **************************************************************************************************/
 RC writeCurrentBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	// Current block has pageNum = curPagePos
  // readBlock function will check if pageNum is inside the bounds
	if (writeBlock(fHandle->curPagePos, fHandle, memPage) == RC_OK)
		return RC_OK;
	else
		return RC_WRITE_FAILED;
}

/**************************************************************************************************
 * Function Name: ensureCapacity
 * Description: how many pages we need to append,
				if the number is negative or 0, we dont need to append any page
 **************************************************************************************************/
RC ensureCapacity(int numberOfPages, SM_FileHandle *fHandle)
{
	int k;
	int PageAdded = numberOfPages - (fHandle->totalNumPages);
	
	if (PageAdded > 0)
	{
		for (k = 0; k < PageAdded; k++)
		{
			CreateCalling = 3;
			createPageFile(fHandle->fileName);
			CreateCalling = 1;
			fHandle->totalNumPages++;
			fHandle->curPagePos = fHandle->totalNumPages - 1;
		}
		return RC_OK;
	} else
		return RC_READ_NON_EXISTING_PAGE; 
}

/**************************************************************************************************
 * Function Name: readBlock
 * Description: Checks if pageNum is inside the bounds
				In case the page exists, we use fseek to move the read pointer of our file where we want to start reading
 				Read the file into PageHandler from the seek location using built-in function "fread"
 **************************************************************************************************/

 RC readBlock(int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	
	if (fHandle->totalNumPages < pageNum)
	{
		return RC_READ_NON_EXISTING_PAGE;
	}
	else
	{
		FILE *fp;
		fp = fopen(fHandle->fileName, "r");
		//FILE *fopen(const char *filename, const char *mode)
		/*"r" Opens a file for reading. The file must exist.
		"w" Creates an empty file for writing. If a file with the same name already exists, its content is erased and the file is considered as a new empty file.
		"a" Appends to a file. Writing operations, append data at the end of the file. The file is created if it does not exist.
		"r+"  Opens a file to update both reading and writing. The file must exist.
		"w+"  Creates an empty file for both reading and writing.
		"a+"  Opens a file for reading and appending.
		*/
		if (fp != NULL)
		{
			if (fseek(fp, ((pageNum * PAGE_SIZE) + 2 * PAGE_SIZE), SEEK_SET)== 0)
			{
				fread(memPage, PAGE_SIZE, 1, fp);
				fHandle->curPagePos = pageNum;
				fclose(fp);
				return RC_OK;
			}
			else
			{
				return RC_READ_NON_EXISTING_PAGE;
			}
		} else
		{
			return RC_FILE_NOT_FOUND;
		}
	}
}
/**************************************************************************************************
 * Function Name: readFirstBlock
 * Description: Calls the readBlock function with pageNum=0
 **************************************************************************************************/

RC readFirstBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	// because first page is page 0
	
	if (readBlock(0, fHandle, memPage) == RC_OK)
		return RC_OK;
	else
		return RC_READ_NON_EXISTING_PAGE;
}

/**************************************************************************************************
 * Function Name: getBlockPos
 * Description: returns the current block position using curPagePos variable of the file handler
 **************************************************************************************************/
int getBlockPos(SM_FileHandle *fHandle)
{
	return fHandle->curPagePos;
}

/**************************************************************************************************
 * Function Name: readCurrentBlock
 * Description: Calls the readBlock function with pageNum=curPagePos
 **************************************************************************************************/

RC readCurrentBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	if (readBlock(getBlockPos(fHandle), fHandle, memPage) == RC_OK)
		return RC_OK;
	else
		return RC_READ_NON_EXISTING_PAGE;
}

/**************************************************************************************************
 * Function Name: readPreviousBlock
 * Description: Calls the readBlock function with pageNum=curPagePos-1
 **************************************************************************************************/

RC readPreviousBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	if (readBlock(getBlockPos(fHandle) - 1, fHandle, memPage) == RC_OK)
		return RC_OK;
	else
		return RC_READ_NON_EXISTING_PAGE;
}


/**************************************************************************************************
 * Function Name: readLastBlockBlock
 * Description: Calls the readBlock function with pageNum=totalNumPages-1
 **************************************************************************************************/
RC readLastBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	if (readBlock(fHandle->totalNumPages - 1, fHandle, memPage) == RC_OK)
		return RC_OK;
	else
		return RC_READ_NON_EXISTING_PAGE;
}

/**************************************************************************************************
 * Function Name: readNextBlock
 * Description: Calls the readBlock function with pageNum=curPagePos+1
 **************************************************************************************************/
RC readNextBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	if (readBlock(getBlockPos(fHandle) + 1, fHandle, memPage) == RC_OK)
		return RC_OK;
	else
		return RC_READ_NON_EXISTING_PAGE;
}


