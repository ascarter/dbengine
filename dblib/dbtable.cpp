// ================================================================================
//
//	File:
//		dbtable.cpp
//
//	Component:
//		Database Engine
//
//	Description:
//		Implementation of database table object.
//
// --------------------------------------------------------------------------------
//  Copyright (c) 2001-2004 Andrew Carter
//  All rights reserved
// ================================================================================

#include "db.h"

// --------------------------------------------------------------------------------
//  Method:
//      CDbTable::CDbTable
//
//  Description:
//      Default constructor
//
//  Inputs:
//		pFile		== IN: Pointer to database file
//      pTableInfo	== IN: Pointer to table info
// --------------------------------------------------------------------------------
CDbTable::CDbTable
(
	CDbFile*		pdbFile,
	DbTableInfo*	pTableInfo
)
{
	TRACE_INIT("CDbTable::CDbTable");

	m_pdbFile		= pdbFile;
	m_pTableInfo	= pTableInfo;
	m_pFile			= m_pdbFile->m_pFile;
	m_idxSlot		= 0;

	m_cbBuffer	= m_pTableInfo->Size;
	m_pBuffer	= (DbRecord*) new BYTE[m_cbBuffer];
}

// --------------------------------------------------------------------------------
//  Method:
//      CDbTable::CDbTable
//
//  Description:
//      Copy constructor
//
//  Inputs:
//		rTable	== IN: Reference to source table
// --------------------------------------------------------------------------------
CDbTable::CDbTable
(
	const CDbTable& rTable
)
{
	TRACE_INIT("CDbTable::CDbTable");

	m_pdbFile		= rTable.m_pdbFile;
	m_pTableInfo	= rTable.m_pTableInfo;
	m_pFile			= m_pdbFile->m_pFile;
	m_idxSlot		= 0;

	m_cbBuffer	= m_pTableInfo->Size;
	m_pBuffer	= (DbRecord*) new BYTE[m_cbBuffer];
}

// --------------------------------------------------------------------------------
//  Method:
//      CDbTable::operator=
//
//  Description:
//      Assignment constructor
//
//  Inputs:
//		rTable	== IN: Reference to source table
// --------------------------------------------------------------------------------
CDbTable& CDbTable::operator=
(
	const CDbTable& rTable
)
{
	m_pdbFile		= rTable.m_pdbFile;
	m_pTableInfo	= rTable.m_pTableInfo;
	m_pFile			= m_pdbFile->m_pFile;
	m_idxSlot		= 0;

	m_cbBuffer	= m_pTableInfo->Size;
	m_pBuffer	= (DbRecord*) new BYTE[m_cbBuffer];

	return *this;
}

// --------------------------------------------------------------------------------
//  Method:
//      CDbTable::~CDbTable
//
//  Description:
//      Default destructor
// --------------------------------------------------------------------------------
CDbTable::~CDbTable()
{
	TRACE_INIT("CDbTable::~CDbTable");

	if (m_pBuffer)
	{
		delete[] m_pBuffer;
	}
}

// ================================================================================
// NAVIGATION
// ================================================================================

// --------------------------------------------------------------------------------
//  Method:
//      CDbTable::Find
//
//  Description:
//      Retrieve record by id
//
//  Inputs:
//      id == IN: Record id
//
//  Returns:
//      Position of matching record
// --------------------------------------------------------------------------------
DBPOS CDbTable::Find
(
	DBRECID		id
)
{
	DbRecord record;

	TRACE_INIT("CDbTable::FindRecord");

	// Scan table records for id
	// TODO: Use indexed table scan	
	MoveFirst();
	while (Fetch(&record, 1) == 1)
	{
		if (record.RID == id)
		{
			return true;
		}
	}

	// Record not found
	return 0;
}

// --------------------------------------------------------------------------------
//  Method:
//      CDbTable::Fetch
//
//  Description:
//      Retrieve next n records from table.  Client is responsible for creating
//		record buffer of cRecords * RowSize
//
//  Inputs:
//		prgRecords	== OUT: Record output buffer
//      cRecords	== IN:	Count of records to retrieve
//
//  Returns:
//      Count of records retrieved
// --------------------------------------------------------------------------------
UINT CDbTable::Fetch
(
	DbRecord*	prgRecords,
	UINT		cRecords
)
{
	UINT cbRead = 0;

	TRACE_INIT("CDbTable::Fetch");

	if (!prgRecords)
	{
		throw invalid_argument("Record buffer invalid");
	}

	// Initialize output buffer
	memset(prgRecords, 0, m_pTableInfo->Size * cRecords);

	// Determine starting/ending points
	FILEOFFSET idxStart = m_pTableInfo->Offset + (m_idxSlot * m_pTableInfo->Size);
	FILEOFFSET idxEnd	= m_pTableInfo->Offset + (m_pTableInfo->Entries * m_pTableInfo->Size);

	// Determine if end of table
	if (idxEnd > idxStart)
	{
		// Adjust record count to request based on remaining rows
		cRecords = min(cRecords, (idxEnd - idxStart) / m_pTableInfo->Size);

		// Read next n records
		m_pFile->Seek(idxStart);
		cbRead = m_pFile->Read(prgRecords, cRecords * m_pTableInfo->Size);

		// Adjust cursor
		m_idxSlot += cbRead / m_pTableInfo->Size;
	}

	return cbRead / m_pTableInfo->Size;
}

// --------------------------------------------------------------------------------
//  Method:
//      CDbTable::Move
//
//  Description:
//      Position cursor by offset amount (skip next n records)
//
//  Inputs:
//		cSkip	== IN:	Count of records to skip
//
//  Returns:
//      New cursor position
// --------------------------------------------------------------------------------
DBPOS CDbTable::Move
(
	UINT cSkip
)
{
	m_idxSlot = min(m_idxSlot + cSkip, m_pTableInfo->Entries);
	return m_idxSlot;
}

// ================================================================================
// MODIFICATION
// ================================================================================

// --------------------------------------------------------------------------------
//  Method:
//      CDbTable::Insert
//
//  Description:
//      Insert new records to table
//
//	Inputs:
//		prgRecords == IN: New record collection
//		cRecords == IN: Count of new records
//
//  Returns:
//      Count of inserted records
// --------------------------------------------------------------------------------
UINT CDbTable::Insert
(
	DbRecord*	prgRecords,
	UINT		cRecords
)
{
	TRACE_INIT("CDbTable::Insert");

	MoveFirst();

	// Test to see if there is room in the table data area
	if ((m_pTableInfo->Entries + cRecords) > m_pTableInfo->Slots)
	{
		// OVERFLOW -- move table to a new data area
		m_pdbFile->ExpandTable(m_pTableInfo->Name);
	}

	// Calculate row position and new file offsets
	UINT		cbToWrite		 = cRecords * m_pTableInfo->Size;
	FILEOFFSET	posStartOffset	 = m_pTableInfo->Offset + (m_pTableInfo->Entries * m_pTableInfo->Size);

	m_pTableInfo->Entries += cRecords;

	// Set row id's
	for (UINT idx = 0; idx < cRecords; idx++)
	{
		DbRecord* pRecord = (DbRecord*) ((BYTE*) prgRecords + (idx * m_pTableInfo->Size));
		m_pTableInfo->LastRecordId += 1;
		pRecord->RID = m_pTableInfo->LastRecordId;
	}

	// Append rows to the file
	m_pFile->Seek(posStartOffset);
	UINT cbWritten = m_pFile->Write(prgRecords, cbToWrite);

	return cbWritten / m_pTableInfo->Size;
}

// --------------------------------------------------------------------------------
//  Method:
//      CDbTable::Update
//
//  Description:
//      Update existing records
//
//	Inputs:
//		prgRecords == IN: Updated record collection
//		cRecords   == IN: Count of updated records
//
//  Returns:
//      Count of updated records
// --------------------------------------------------------------------------------
UINT CDbTable::Update
(
	DbRecord*	prgRecords,
	UINT		cRecords
)
{
	UINT cUpdated = 0;

	TRACE_INIT("CDbTable::Update");

	MoveFirst();
	
	// Loop through records and update the record in the database
	for (UINT idx = 0; idx < cRecords; idx++)
	{
		DbRecord* pRecord = (DbRecord*) ((BYTE*) prgRecords + (idx * m_pTableInfo->Size));
		FILEOFFSET posRec = FindRecordOffset(pRecord->RID);

		if (posRec != 0)
		{
			m_pFile->Seek(posRec);
			UINT cbWritten = m_pFile->Write(pRecord, m_pTableInfo->Size);
			_ASSERTE(cbWritten == m_pTableInfo->Size);
			cUpdated++;
		}
	}

	return cUpdated;
}

// --------------------------------------------------------------------------------
//  Method:
//      CDbTable::Delete
//
//  Description:
//      Remove records from the table.  Copies last record in the table to the
//		slots vacated by the deleted records.
//
//	Inputs:
//		prgRecords == IN: Records to delete (can be ID's only)
//		cRecords   == IN: Count of records to delete
//
//  Returns:
//      Count of deleted records
// --------------------------------------------------------------------------------
UINT CDbTable::Delete
(
	DbRecord*	prgRecords,
	UINT		cRecords
)
{
	UINT cDeleted = 0;
		
	TRACE_INIT("CDbTable::DeleteRecord");

	MoveFirst();

	for (UINT idx = 0; idx < cRecords; idx++)
	{
		DbRecord* pRecord = (DbRecord*) ((BYTE*) prgRecords + (idx * m_pTableInfo->Size));

		// Get record offset
		FILEOFFSET posDel = FindRecordOffset(pRecord->RID);

		if (posDel)
		{
			// Find record to move
			FILEOFFSET posMove = ((m_pTableInfo->Entries - 1) * m_pTableInfo->Size) + m_pTableInfo->Offset;
			m_pFile->Seek(posMove);

			// Copy record
			UINT cbRead = m_pFile->Read(m_pBuffer, m_cbBuffer);
			_ASSERTE(cbRead == m_cbBuffer);

			// Write record into empty slot
			m_pFile->Seek(posDel);
			UINT cbWritten = m_pFile->Write(m_pBuffer, m_cbBuffer);
			_ASSERTE(cbWritten == cbRead);

			// Clear slot for old record
			m_pFile->Seek(posMove);
			memset(m_pBuffer, 0, m_cbBuffer);
			cbWritten = m_pFile->Write(m_pBuffer, m_cbBuffer);
			_ASSERTE(cbWritten == m_cbBuffer);

			// Update table metadata
			m_pTableInfo->Entries--;
			cDeleted++;
		}
	}

	return cDeleted;
}

// --------------------------------------------------------------------------------
//  Method:
//      CDbTable::FindRecordOffset
//
//  Description:
//      Retrieves offset of record by doing a table scan for the specified record
//		id.  Returns the offset in the data file.  Use GetRecord to actually
//		retrieve the record data.
//
//	Inputs:
//		id == IN: Record id to find
//
//  Returns:
//      Disk offset
// --------------------------------------------------------------------------------
UINT CDbTable::FindRecordOffset
(
	DBRECID id
)
{
	RESULT		result	= SUCCESS;
	UINT		cbRead	= 0;
	UINT		idx		= m_pTableInfo->Offset;
	DbRecord	record;

	TRACE_INIT("CDbTable::FindRecord");

	FILEOFFSET idxEnd = m_pTableInfo->Offset + (m_pTableInfo->Size * m_pTableInfo->Entries);
	while(idx < idxEnd)
	{
		// Position to next record
		m_pFile->Seek(idx);

		// Read record
		cbRead = m_pFile->Read(&record, sizeof(record));

		if (record.RID == id)
		{
			return idx;
		}

		idx += m_pTableInfo->Size;
	}

	return 0;
}


