#include "tables.h"
#include "storage_mgr.h"
#include "record_mgr.h"
#include <stdlib.h>
#include <string.h>


//      Init global value

int scanRecNum = 0;
int TombstoneID[10000];
SM_FileHandle rec_mgr_FileHandle;


Schema *
deSerializeSchema(char *string, Schema *schema)
{
	int i;
    char numAttribs[6];
    int j = 0;

    printf("\n");
	while(string[i] != '\0')
	{
        if (string[i] == '<') 
		{
            i ++;
            for(j = 0; string[i] != '>'; j ++, i ++) {
                numAttribs[j] = string[i];
            }
        }
		i++;
    }
    schema->numAttr = atoi(numAttribs);

    schema->attrNames = (char **)malloc(schema->numAttr * sizeof(char *));
    schema->dataTypes = (DataType *)malloc(schema->numAttr * sizeof(DataType));
    schema->typeLength = (int *)malloc(schema->numAttr * sizeof(int));

    for (i = 0; string[i] != '\0'; i ++) {
        if (string[i] == '(') {
            i ++;
            for (j = 0; j < schema->numAttr; j ++, i ++) {
                char tempString[20];
                memset(tempString,0,strlen(tempString));
                int k = 0;
                while (string[i] != ',')
					{
                    tempString[k ++] = string[i ++];
                    if (string[i] == ')')
                        break;
                }
                i ++;
                int z = 0;
                char attrName[20];
                char type;
                memset(attrName,0,strlen(attrName));
                int counter = 0;

                for (; tempString[z] != ':'; z ++) {
                    attrName[counter ++] = tempString[z];
                }
                type = tempString[z + 2];
                schema->attrNames[j] = (char *) malloc(sizeof(attrName));
                strcpy(schema->attrNames[j], attrName);
                switch (type) {
					
					
					
					if (type ==  'F')
					{
                        schema->dataTypes[j] = DT_FLOAT;
                        schema->typeLength[j] = 0;
                    }   
                    
                    if (type == 'B')
					{
                        schema->dataTypes[j] = DT_BOOL;
                        schema->typeLength[j] = 0;
					}   
						
					
                    
					if (type ==  'I')
					{
                        schema->dataTypes[j] = DT_INT;
                        schema->typeLength[j] = 0;
					}  
						
					
                    
                }
                if (string[i - 1] == ')')
                    break;
            }
        }
        if (string[i - 1] == ')')
            break;
    }

    char key[20];

    for (i = 0; string[i] != '\0'; i ++) {
        if ((string[i] == 's') && (string[i + 1] == ':') && (string[i + 2] == ' ') && (string[i + 3] == '(')) {

            i += 4;
            int count = 0;
            while (string[i] != ')') {
                key[count] = string[i];
                count ++;
                i ++;
            }
            schema->keySize = 1;
            schema->keyAttrs = (int *)malloc(schema->keySize * sizeof(int));
            int iter;
            for (iter = 0; iter < schema->numAttr; iter ++) {
                if (key == schema->attrNames[iter])
                    schema->keyAttrs[0] = iter;

            }
            break;
        }
    }
    return schema;
}





//   ## TABLE AND RECORD MANAGER FUNCTIONS ##


/* 
	#initRecordManager 
	Intializes Record manager.
*/

RC initRecordManager (void *mgmtData)
{
	int i;
	for(i=0;i<10000;i++)
	{
		TombstoneID[i] = -99;
	}
    return RC_OK;
}

/*
	#shutdownRecordManager  
	Shutdowns the record manager and frees the resources 
	assigned to record manager

 */
 
RC shutdownRecordManager ()
{
    return RC_OK;
}

/*
	#createTable 
	It creates a table. If it already exists throws error message. It 
	stores information about the schema, free-space in the table information pages.
		
 */
 
RC createTable (char *name, Schema *schema)
{
    SM_PageHandle pageHandle;
    pageHandle = (SM_PageHandle) malloc(PAGE_SIZE);
    createPageFile (name);                      
    openPageFile (name, &rec_mgr_FileHandle);    
    memset(pageHandle,'\0',PAGE_SIZE);
    strcpy(pageHandle,serializeSchema(schema)); 
    
    writeBlock (0, &rec_mgr_FileHandle, pageHandle);

    memset(pageHandle,'\0',PAGE_SIZE);
    writeBlock (1, &rec_mgr_FileHandle, pageHandle);

    free(pageHandle);
    
    return RC_OK;
}


/*
		#openTable
		It opens the file with give table name.

 */
 
RC openTable (RM_TableData *rel, char *name)
{
	
	SM_PageHandle pageHandle;
	pageHandle = (SM_PageHandle) malloc(PAGE_SIZE);

	
	int schemaSize = sizeof(Schema);
	int nameSize = sizeof(char);
	rel->name = (char *)malloc(nameSize*100);
	rel->schema = (Schema *)malloc(schemaSize);
    
    if(rec_mgr_FileHandle.fileName == NULL)
	{
    	openPageFile(rel->name,&rec_mgr_FileHandle);
	}
    strcpy(rel->name,name);
    readBlock(0, &rec_mgr_FileHandle, pageHandle);
    deSerializeSchema(pageHandle, rel->schema);
    rel->mgmtData = NULL; 
    
    free(pageHandle);

    if(rel->schema != NULL)
	{
		if(rel != NULL)
		{
        return RC_OK;
		}
		else 
			return RC_OPEN_TABLE_FAILED;
			
	}
    else
    	return RC_OPEN_TABLE_FAILED;

    closePageFile(&rec_mgr_FileHandle);

    return RC_OK;
}
/*
	
		#closeTable
		It frees all the resources assigned with table.
*/
 
RC closeTable (RM_TableData *rel)
{
    SM_PageHandle pageHandle = (SM_PageHandle)malloc(PAGE_SIZE);
	char FreePageNumber[10];
	char nullString[PAGE_SIZE];
	char ListPageFree[PAGE_SIZE];
	memset(ListPageFree,'\0',PAGE_SIZE);
	readBlock(1,&rec_mgr_FileHandle,pageHandle);
	strcpy(ListPageFree,pageHandle);
	memset(FreePageNumber,'\0',10);
	memset(nullString,'\0',PAGE_SIZE);
	
	int x =0;
	while(TombstoneID[x] != -99 )
	{
		sprintf(FreePageNumber,"%d",(TombstoneID[x]));
		strcat(ListPageFree,FreePageNumber);
		strcat(ListPageFree,";");

		writeBlock(1,&rec_mgr_FileHandle,ListPageFree);
		writeBlock(TombstoneID[x]+2,&rec_mgr_FileHandle,nullString);

		memset(FreePageNumber,'\0',10);
		x++;
	}

    closePageFile(&rec_mgr_FileHandle);

    free(rel->schema);
    free(rel->name);
    return RC_OK;
}

/*
		#deleteTable
		It deletes the table.
 */

 RC deleteTable (char *name)
{
    destroyPageFile(name);
    return RC_OK;
}

/*
	#getNumTuples
	It  returns the number of tuples in the table.
 */

 int getNumTuples (RM_TableData *rel)
{
    int freePageCount = 0;
    int i=0;
    
    SM_PageHandle pageHandle;
    
    pageHandle = (SM_PageHandle) malloc(PAGE_SIZE);
    
    readBlock(1, &rec_mgr_FileHandle, pageHandle);
    
     
	while(pageHandle[i] != '\0')
	{
        if (pageHandle[i] == ';')
            freePageCount ++;
		i++;
    }
    
    closePageFile(&rec_mgr_FileHandle);
    openPageFile(rel->name, &rec_mgr_FileHandle);
    return (rec_mgr_FileHandle.totalNumPages - freePageCount);
}



//    ## RECORD FUNCTIONS ##

/*
	#insertRecord
	It inserts a new record.New record will be assigned a RID and the record attributes are updated. 

 */

 RC insertRecord (RM_TableData *rel, Record *record)
{
	if(rec_mgr_FileHandle.fileName == NULL)
	{
		openPageFile(rel->name,&rec_mgr_FileHandle);
	}
	int RcdID;
	char *WriteString = (char *)malloc(sizeof(char) *PAGE_SIZE);
	int i = 0;
	char PageFree = 'N';
	char PageFreeNumber[10];
	char *FreePageListNew = (SM_PageHandle) malloc(PAGE_SIZE);;   
    memset(WriteString,'\0',PAGE_SIZE);
    strcpy(WriteString,record->data);
	char ListPageFree[PAGE_SIZE];
	memset(ListPageFree,'\0',PAGE_SIZE);
	SM_PageHandle pageHandle;
	pageHandle = (SM_PageHandle) malloc(PAGE_SIZE);
	readBlock(1, &rec_mgr_FileHandle, pageHandle);
	strcpy(ListPageFree,pageHandle); 
	free(pageHandle);
	int temp = 0;
	if(ListPageFree != NULL)
	{
		
		while(temp<strlen(ListPageFree))
		{
			if(ListPageFree[temp] == ';')
			{
				PageFree = 'Y';
				break;
			}
			temp++;
		}
	}
	if(PageFree != 'Y')
	{
		writeBlock(rec_mgr_FileHandle.totalNumPages,&rec_mgr_FileHandle,WriteString);
		RcdID = rec_mgr_FileHandle.totalNumPages - 3;
		///		
	}
	else
	{
		memset(PageFreeNumber,'\0',10);
		for(temp = 0; ListPageFree[temp] != ';';temp++)
		{
			PageFreeNumber[temp] = ListPageFree[temp];
		}

		writeBlock(atoi(PageFreeNumber) + 2,&rec_mgr_FileHandle,WriteString);
		rec_mgr_FileHandle.totalNumPages--; 
		memset(FreePageListNew,'\0',PAGE_SIZE);
		for(temp = (strlen(PageFreeNumber) + 1) ;temp < strlen(ListPageFree);temp++)
		{
			FreePageListNew[i++] = ListPageFree[temp];
		}
		writeBlock(1,&rec_mgr_FileHandle,FreePageListNew);
		free(FreePageListNew);
		RcdID = atoi(PageFreeNumber);
	}

	record->id.page = RcdID;
	record->id.slot = -99; 
    free(WriteString);

    	return RC_OK;

}
/*
	#deleteRecord
	It deletes the record with specified RID passed as parameter.
 */
 
RC deleteRecord (RM_TableData *rel, RID id)
{
	int i = 0 ;
	while(TombstoneID[i] != -99)
		i++;
	TombstoneID[i] = id.page;

    return RC_OK;
}
/*
	#updateRecord
	It updates the existing record with new value.
 */

 RC updateRecord (RM_TableData *rel, Record *record)
{

	openPageFile (rel->name, &rec_mgr_FileHandle);
	writeBlock(record->id.page + 2,&rec_mgr_FileHandle,record->data);
	closePageFile(&rec_mgr_FileHandle);
    return RC_OK;
}
/*
	#getRecord
	It retrieves a record with certain RID passed as parameter.
 */
 
RC getRecord (RM_TableData *rel, RID id, Record *record)
{

    SM_PageHandle pageHandle;
	SM_FileHandle fileHandle;
	openPageFile (rel->name, &fileHandle);
    pageHandle = (SM_PageHandle) malloc(PAGE_SIZE);
    memset(pageHandle,'\0',PAGE_SIZE);
    readBlock(id.page + 2, &fileHandle, pageHandle);
    record->id.page = id.page;
    strcpy(record->data, pageHandle);
    closePageFile(&fileHandle);
    free(pageHandle);

    return RC_OK;
}
  
//   ## SCAN FUNCTIONS ##


/*

	#startScan
	It initializes the RM_ScanHandle data structure passed as parameter.

 */

 RC startScan (RM_TableData *rel, RM_ScanHandle *scan, Expr *cond)
{
	scan->mgmtData = cond;
	scan->rel = rel;
	return RC_OK;
}

/*
 * Author	:Karthik
 * Date		:23.Apr.2015
 * Module	:storePosSemicolon
 * Description: It stores the semicolon position
 */
void storeSemiColonPostion(char * record,int *p)
{
	int i=0;
	int j = 0;
	//for(i = 0 ; i < strlen(record); i++)
	while(i<strlen(record))
	{		
	if(record[i] == ';')
	{
			p[j++] = i;
	}
	i++;
	}
}
/*
 * Author	:Karthik
 * Date		:23.Apr.2015
 * Module	:getColumndata
 * Description:It gets the columndata.
 */
void getColumnData(int columnNum,char * record,int *PosSemicolon,char * CellVal)
{
	int StartCell;
	int i;
	int j = 0;
	if(columnNum != 0)
	{
	StartCell = PosSemicolon[columnNum - 1] + 1;	
	}
	else
	{
	StartCell = 0;	
	}
	
	i = StartCell;
	while(i < PosSemicolon[columnNum])
	{	
	CellVal[j++] = record[i];
	i++;
	}
	

}
/*
	#next
	It returns the next tuple that fullfills the scan condition.
	It also returns RC_RM_NO_MORE_TUPLES once the scan is completed and RC_OK otherwise 
	unless an error occurs. 

 */
 
RC next (RM_ScanHandle *scan, Record *record)
{
	char  CellVal[PAGE_SIZE];
	int PosSemicolon[3];
	Expr *expr = (Expr *) scan->mgmtData;
	if(rec_mgr_FileHandle.fileName == NULL)
		
		openPageFile(scan->rel->name,&rec_mgr_FileHandle);
	
	while(scanRecNum < rec_mgr_FileHandle.totalNumPages - 2)
	{
		SM_PageHandle pageHandle = (SM_PageHandle) malloc(PAGE_SIZE);
		memset(pageHandle,'\0',PAGE_SIZE);

		readBlock(scanRecNum+2,&rec_mgr_FileHandle,pageHandle);

		
		memset(CellVal,'\0',PAGE_SIZE);

		storeSemiColonPostion(pageHandle,PosSemicolon);
		if(expr->expr.op->type == OP_BOOL_NOT)
		{
			getColumnData(expr->expr.op->args[0]->expr.op->args[0]->expr.attrRef,pageHandle,PosSemicolon,CellVal);
			if(expr->expr.op->args[0]->expr.op[0].args[1]->expr.cons->v.intV <= atoi(CellVal))
			{
				strcpy(record->data,pageHandle);
				scanRecNum++;
				free(pageHandle);
				return RC_OK;
			}
		}
		
		else if(expr->expr.op->type == OP_COMP_EQUAL)
		{
			getColumnData(expr->expr.op->args[1]->expr.attrRef,pageHandle,PosSemicolon,CellVal);
			if(expr->expr.op->args[0]->expr.cons->dt == DT_STRING)
			{
				if(strcmp(CellVal,expr->expr.op->args[0]->expr.cons->v.stringV) == 0)
				{
					strcpy(record->data,pageHandle);
					scanRecNum++;
					free(pageHandle);
					return RC_OK;
				}
			}
			
			else if(expr->expr.op->args[0]->expr.cons->dt == DT_INT)
			{
				if(atoi(CellVal) == expr->expr.op->args[0]->expr.cons->v.intV)
				{
					strcpy(record->data,pageHandle);
					scanRecNum++;
					free(pageHandle);
					return RC_OK;
				}
			}
			
		}
		
		scanRecNum++;
		free(pageHandle);
	}
	scanRecNum = 0; 
	return RC_RM_NO_MORE_TUPLES;
}
/*
	#closeScan
	It indicates to the record manager that all associated 
	resources can be cleaned up.
 */
 
RC closeScan (RM_ScanHandle *scan)
{
    return RC_OK;
}


//   ## Schema Functions ##

/*
	#getRecordSize
	It returns the size of records for specified schema.

 */

int getRecordSize (Schema *schema){
    int mysize;
    int index;
    mysize = 0;
    for (int i = 0; i<schema->numAttr; i++)
	{
        if (schema->dataTypes[i] == DT_BOOL) 
		{
            mysize += sizeof(bool);
        }
        else if (schema->dataTypes[i] == DT_STRING)
		{
                mysize += schema->typeLength[i];
		}
		
        else if (schema->dataTypes[i] == DT_INT)
		{
                mysize += sizeof(int);
        }     
        else if (schema->dataTypes[i] == DT_FLOAT)
		{
                mysize += sizeof(float);
        }  
        
    }
    mysize = 8;
    return mysize;
}


/*
	#createSchema
	It creates new schema.
 */


Schema *createSchema (int numAttr, char **attrNames, DataType *dataTypes, int *typeLength, int keySize, int *keys){
    Schema *myschema = (Schema *)malloc(sizeof(Schema));
    myschema->numAttr = numAttr;
    myschema->typeLength = typeLength;
    myschema->keySize = keySize;
	myschema->keyAttrs = keys;
	myschema->attrNames = attrNames;
    myschema->dataTypes = dataTypes;
    
    return myschema;
}


/*
	#freeSchema
	It frees a given schema.

 */

RC freeSchema (Schema *schema){
    free(schema);
    return RC_OK;
}



//   ## ATTRIBUTE FUNCTIONS ##

/*
	#createRecord
	It creates a new record for specified schema.

 */

RC createRecord (Record **record, Schema *schema){
    *record = (Record *)malloc(sizeof(Record));
    (*record)->data = (char *)malloc(PAGE_SIZE);
    memset((*record)->data,'\0',PAGE_SIZE);
	
    if(record == NULL)
	{
	return RC_CREATE_FAILED;
    }
	
	else
	{
	return RC_OK;
    }
}


/*
	#freeRecord 
	It frees all the memory assigned to specified record.
 */

RC freeRecord (Record *record){
    free(record);
    return RC_OK;
}


/*
	#getAttr 
	It retrieves the attribute values of a record.

 */

RC getAttr (Record *record, Schema *schema, int attrNum, Value **value){
     int myDataTypCou = 0;
    char tem[PAGE_SIZE+1];
	int myTempCou = 1;
   *value = (Value *)malloc(sizeof(Value) * schema->numAttr);
    int k;
    memset(tem,'\0',PAGE_SIZE + 1);
    for (k = 0; k<PAGE_SIZE; k++){
        if ((record->data[k]==';')||(record->data[k]=='\0')){
            if (attrNum == myDataTypCou)
			{
                if (schema->dataTypes[myDataTypCou] == DT_BOOL){
                        tem[0] = 'b';
					}
                       
                    else if (schema->dataTypes[myDataTypCou]==DT_STRING)
					{
                        tem[0] = 's';
                    }
                    else if (schema->dataTypes[myDataTypCou]== DT_INT)
					{
                        tem[0] = 'i';
                    }   
                   else if (schema->dataTypes[myDataTypCou]== DT_FLOAT)
					{
                        tem[0] = 'f';
					}
                 
                *value = stringToValue(tem);
                break;
            }
            myDataTypCou++;
            myTempCou=1;
            memset(tem,'\0',PAGE_SIZE+1);
        }
        else {
            tem[myTempCou++] = record->data[k];
        }
    }
    return RC_OK;
}


/*
	#setAttr 
	It sets the attribute of a record.
 */


RC setAttr (Record *record, Schema *schema, int attrNum, Value *value)
{
	 char * colValue = serializeValue(value);

	 int semiColNum = 0;
	 int i;
	 for(i = 0; i < strlen(record->data) ; i++)
		 if(record->data[i] == ';')
			 semiColNum++;

	 if(semiColNum != schema->numAttr)
	 {
		strcat(record->data,colValue);
		strcat(record->data,";");
		
	 }
	 else
	 {
		  int semiColPos[schema->numAttr];

		 int j = 0;
		 for(i = 0; i < strlen(record->data) ; i++)
			 if(record->data[i] == ';')
				 semiColPos[j++] = i;

		 if( attrNum != 0)
		 {
			 char strFirst[PAGE_SIZE];
			 char strEnd[PAGE_SIZE];

			 memset(strFirst,'\0',PAGE_SIZE);
			 memset(strEnd,'\0',PAGE_SIZE);

			 for(i = 0 ; i <= semiColPos[attrNum - 1]; i++)
			 {
				 strFirst[i] = record->data[i];
			 }
			 strFirst[i] = '\0';

			 int j = 0;
			 for(i = semiColPos[attrNum] ; i < strlen(record->data) ; i++)
			 {
				 strEnd[j++] = record->data[i];
			 }
			 strEnd[j] = '\0';

			 strcat(strFirst,colValue);
			 strcat(strFirst,strEnd);

			 memset(record->data,'\0',PAGE_SIZE);

			 strcpy(record->data,strFirst);
		 }
		 else
		 {
			  char strEnd[PAGE_SIZE];
			 memset(strEnd,'\0',PAGE_SIZE);
			 i = 0;
			 int j;
			 for(j = semiColPos[attrNum] ; j < strlen(record->data) ; j++ )
			 {
				 strEnd[i++] = record->data[j];
			 }
			 strEnd[i] = '\0';

			 memset(record->data,'\0',PAGE_SIZE);

			 strcpy(record->data,colValue);
			 strcpy(record->data,strEnd);
			
		 }
		 
		
	 }

	 if(colValue == NULL)
		  return RC_SET_ATTR_FAILED;
		
	 else
		 return RC_OK;
}
