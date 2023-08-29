// ================================================================================
//
//	File:
//		dbtest.cpp
//
//	Component:
//		DBEngine
//
//	Description:
//		Database engine test utility
//
// --------------------------------------------------------------------------------
//  Copyright (c) 2001-2004 Andrew Carter
//  All rights reserved
// ================================================================================

#include "db.h"
#include "userrec.h"

// --------------------------------------------------------------------------------
// FUNCTION PROTOTYPES
// --------------------------------------------------------------------------------
void		DisplayRecords(CDbTable* pTable);
CDbFile*	CreateTestFile(const string& strFile);
void		CreateTestTables(CDbFile* pFile);
void		StressAdd(CDbTable* pTable);
void		StressUpdate(CDbTable* pTable);
void		StressDelete(CDbTable* pTable);
void		Compact(CDbFile* pFile);

const UINT REC_BUFFER	= 10;
const UINT REC_BLOCK	= 100;

// --------------------------------------------------------------------------------
//  Method:
//      main
//
//  Description:
//      Main application entry point
//
//  Inputs:
//      None
//
//  Returns:
//      Nothing
// --------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	// Verify input
	if (argc < 2)
	{
        cout << "test [file]" << endl;
		return -1;
	}

	clock_t tBefore;
	clock_t tAfter;
	time_t	tTime;

	time(&tTime);
	cout << "Start: " << ctime(&tTime) << endl;
	tBefore = clock();

	try
	{
		CDbFilePtr pFile = CreateTestFile(argv[1]);

		CreateTestTables(pFile);
		
		CDbTable* pTable = pFile->GetTable("User7");
		pFile->DeleteTable("User6");

		for (int iCount = 0; iCount < 10; iCount++)
		{
			StressAdd(pTable);
			StressUpdate(pTable);
			StressDelete(pTable);
		}


		// Read records
		pTable = pFile->GetTable("User7");
		DisplayRecords(pTable);

		// Close the file
		pFile->Close();

		// Compress the file
		Compact(pFile);
	}
	catch ( exception& e )
	{
		cerr << e.what() << endl;
	}
	
	tAfter = clock();

	time(&tTime);
	cout << "End: " << ctime(&tTime) << endl;

	float duration = (float) (tAfter - tBefore) / CLOCKS_PER_SEC;
	cout << "Total program execution: " << duration << "s" << endl;

	return 0;
}


void DisplayRecords(CDbTable* pTable)
{
	UserRecord*	pRecords = new UserRecord[REC_BUFFER];
	UINT		cRecords = 0;
	UINT		cTotal	 = 0;
	clock_t		tBefore;
	clock_t		tAfter;

	tBefore = clock();

	// Read records
	pTable->MoveFirst();
	while ((cRecords = pTable->Fetch(pRecords, REC_BUFFER)) > 0)
	{
		cout << "Read records " << pRecords[0].RID << " to ";
		cout << pRecords[REC_BUFFER - 1].RID << endl;

		for (UINT iRec = 0; iRec < REC_BUFFER; iRec++)
		{
			UserRecord* pRecord = &pRecords[iRec];

			cout << pRecord->RID << "\t";
			cout << pRecord->UserId << "\t";
			cout << pRecord->FirstName << "\t";
			cout << pRecord->LastName << "\t";
			cout << pRecord->Login << "\t";
			cout << pRecord->Uniform << "\t";
			cout << pRecord->Age << "\t";
			cout << pRecord->BirthMonth << "/";
			cout << pRecord->BirthDay << "/";
			cout << pRecord->BirthYear << "\t";
			cout << pRecord->Hometown << "\t";
			cout << pRecord->Acquired << endl;
		}

		cTotal += cRecords;
	}

	tAfter = clock();

	delete[] pRecords;

	float duration = (float) (tAfter - tBefore) / CLOCKS_PER_SEC;
	cout << "Fetched " << cTotal << " records (";
	cout << duration << "s\t" << (float) (REC_BLOCK) / duration << " records per second)" << endl;
}

CDbFile* CreateTestFile(const string& strFile)
{
	CDbFile* pFile = new CDbFile(strFile);

	if (pFile->Exists())
	{
		pFile->Delete();
	}

	pFile->Create();

	return pFile;
}

void CreateTestTables(CDbFile* pFile)
{
	CDbTable*	pTable	= NULL;
	UINT		iidx	= 0;

	// Create test tables
	for (iidx = 0; iidx < 10; iidx++)
	{
		strstream strTable;
		strTable << "User" << iidx << ends;
		pTable = pFile->CreateTable(strTable.str(), sizeof(UserRecord), REC_BUFFER, REC_BUFFER);
		pTable = NULL;
	}
}

void StressAdd(CDbTable* pTable)
{
	UserRecord*	pRecords = new UserRecord[REC_BUFFER];
	UINT		cRecords = 0;
	UINT		iidx	 = 0;
	UINT		iRec	 = 0;
	clock_t		tBefore;
	clock_t		tAfter;

	memset(pRecords, 0, sizeof(UserRecord) * REC_BUFFER);
	
	tBefore = clock();

	// Add REC_BLOCK records
	for (iidx = 0; iidx < REC_BLOCK; iidx++)
	{
		for (iRec = 0; iRec < REC_BUFFER; iRec++)
		{
			UserRecord* pRecord = &pRecords[iRec];

			strcpy(pRecord->FirstName, "Record");
			sprintf(pRecord->LastName, "%d", iRec + 1);
			sprintf(pRecord->Login, "%s%d", "record", iRec + 1);
			strcpy(pRecord->Hometown, "Seattle, WA");
			strcpy(pRecord->Acquired, "Drafted 1st round (8th pick overall)");

			pRecord->UserId		= (iidx * REC_BUFFER) + iRec + 1;
			pRecord->Age		= 20 + iRec;
			pRecord->BirthMonth = iRec + 1;
			pRecord->BirthDay	= (iRec + 1) * 2;
			pRecord->BirthYear	= 2001 - pRecord->Age;
			pRecord->Uniform	= iRec * 3;
		}

		pTable->Insert(pRecords, REC_BUFFER);
	}

	tAfter = clock();

	delete[] pRecords;

	float duration = (float) (tAfter - tBefore) / CLOCKS_PER_SEC;
	cout << "Added " << REC_BLOCK << " records (";
	cout << duration << "s\t" << (float) (REC_BLOCK) / duration << " records per second)" << endl;
}

void StressUpdate(CDbTable* pTable)
{
	clock_t		tBefore;
	clock_t		tAfter;
	UINT		iidx = 0;
	UserRecord* pRecords = new UserRecord[REC_BLOCK];

	memset(pRecords, 0, sizeof(UserRecord) * REC_BLOCK);

	tBefore = clock();

	for (iidx = 0; iidx < REC_BLOCK; iidx++)
	{
		pRecords[iidx].RID = iidx + 400;
		strcpy(pRecords[iidx].FirstName, "** Updated");
		strcpy(pRecords[iidx].LastName, "Record **");
		strcpy(pRecords[iidx].Login, "updated");
	}
	
	pTable->Update(pRecords, 100);

	tAfter = clock();

	delete[] pRecords;

	float duration = (float) (tAfter - tBefore) / CLOCKS_PER_SEC;
	cout << "Updated " << REC_BLOCK << " records (";
	cout << duration << "s\t" << (float) (REC_BLOCK) / duration << " records per second)" << endl;
}

void StressDelete(CDbTable* pTable)
{
	clock_t		tBefore;
	clock_t		tAfter;
	UINT		iidx = 0;
	UserRecord* pRecords = new UserRecord[REC_BLOCK];

	memset(pRecords, 0, sizeof(UserRecord) * REC_BLOCK);

	tBefore = clock();

	for (iidx = 0; iidx < REC_BLOCK; iidx++)
	{
		pRecords[iidx].RID = iidx + 400;
	}
	
	pTable->Delete(pRecords, 100);

	tAfter = clock();

	delete[] pRecords;

	float duration = (float) (tAfter - tBefore) / CLOCKS_PER_SEC;
	cout << "Deleted " << REC_BLOCK << " records (";
	cout << duration << "s\t" << (float) (REC_BLOCK) / duration << " records per second)" << endl;
}

void StressGet()
{
}

void StressGetBlock()
{
}


void Compact(CDbFile* pFile)
{
	clock_t	tBefore;
	clock_t	tAfter;

	cout << "File size before compact: " << pFile->GetFileSize() << endl;

	tBefore = clock();
	pFile->Compact();
	tAfter = clock();

	cout << "File size after compact:  " << pFile->GetFileSize() << endl;

	float duration = (float) (tAfter - tBefore) / CLOCKS_PER_SEC;
	cout << "Compacted file (";
	cout << duration << "s)" << endl;
}
