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
	1.	Makefile
	2.	buffer_mgr.h
	3.  buffer_mgr.c
	4.	buffer_mgr_stat.c
	5.	buffer_mgr_stat.h
	6.	dberror.c
	7.	dberror.h
	8.	expr.c
	9.	expr.h
	10. README.txt
	11.	record_mgr.h
	12. record_mgr.c
	13.	rm_serializer.c
	14.	storage_mgr.h
	15. storage_mgr.c
	16.	tables.h
	17.	test_assign3_1.c
	18.	test_expr.c
	19.	test_helper.h
	20. dt.h
	21. DataStructs.h
	
****************************************************************
	3.Compilation Instruction
****************************************************************

To compile and execute our code we used codeblocks IDE on windows 10.
We have also prepared make file which was used in Mingw on Windows 10. Inorder to run it use command "mingw32-make"
It will create object files, which can be executed using "test_assign3" and "test_expr"

****************************************************************
	4. Functions Used
****************************************************************
## TABLE AND RECORD MANAGER FUNCTIONS ##

#initRecordManager 
Intializes Record manager.

#shutdownRecordManager  
Shutdowns the record manager and frees the resources assigned to record manager

#createTable 
It creates a table. If it already exists throws error message. It 
stores information about the schema, free-space in the table information pages.
		
#openTable
It opens the file with give table name.

#closeTable
It frees all the resources assigned with table.

#deleteTable
It deletes the table.

#getNumTuples
It  returns the number of tuples in the table.

## RECORD FUNCTIONS ##

#insertRecord
It inserts a new record.New record will be assigned a RID and the record attributes are updated. 

#deleteRecord
It deletes the record with specified RID passed as parameter.

#updateRecord
It updates the existing record with new value.

#getRecord
It retrieves a record with certain RID passed as parameter.

## SCAN FUNCTIONS ##

#startScan
It initializes the RM_ScanHandle data structure passed as parameter.

#next
It returns the next tuple that fullfills the scan condition.
It also returns RC_RM_NO_MORE_TUPLES once the scan is completed and RC_OK otherwise 
unless an error occurs. 

#closeScan
It indicates to the record manager that all associated 
resources can be cleaned up.

## Schema Functions ##

#getRecordSize
It returns the size of records for specified schema.

#createSchema
It creates new schema.

#freeSchema
It frees a given schema.

## ATTRIBUTE FUNCTIONS ##

#createRecord
It creates a new record for specified schema.

#freeRecord 
It frees all the memory assigned to specified record.

#getAttr 
It retrieves the attribute values of a record.

#setAttr 
It sets the attribute of a record.


****************************************************************
	5. Test Cases
****************************************************************

test_asign3 file implements several test cases using the record_mgr.h 

test_expr file implements several test cases using the expr.h interface 

We have also included additional error check

           1. #define RC_SET_ATTR_FAILED 601
           2. #define RC_CREATE_FAILED 602
           3. #define RC_INSERT_ERROR 603
           4. #define RC_OPEN_TABLE_FAILED 604
