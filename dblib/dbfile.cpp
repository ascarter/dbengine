// ================================================================================
//
//	File:
//		dbfile.cpp
//
//	Component:
//		Database Engine
//
//	Description:
//		Database file implementation
//
// --------------------------------------------------------------------------------
//  Copyright (c) 2001-2004 Andrew Carter
//  All rights reserved
// ================================================================================

#include "db.h"

// --------------------------------------------------------------------------------
//  Method:
//      CDbFile::CDbFile
//
//  Description:
//      Default constructor
//
//  Inputs:
//      None
//
//  Returns:
//      Nothing
// --------------------------------------------------------------------------------
CDbFile::CDbFile
(
	const string& strFile,
	UINT cbBuffer
)
{
    TRACE_INIT("CDbFile::CDbFile");

	memset(&m_fileInfo, 0, sizeof(m_fileInfo));
	
	m_pTableInfo	= NULL;
	m_pIndexInfo	= NULL;
	m_pFile			= new CFile(strFile);
	m_cbBuffer		= cbBuffer;
	m_pBuffer		= new BYTE[cbBuffer];


}

// --------------------------------------------------------------------------------
//  Method:
//      CDbFile::~CDbFile
//
//  Description:
//      Default destructor
//
//  Inputs:
//      None
//
//  Returns:
//      Nothing
// --------------------------------------------------------------------------------
CDbFile::~CDbFile()
{
    TRACE_INIT("CDbFile::~CDbFile");

	m_pFile->Close();

	delete[] m_pTableInfo;
	delete[] m_pIndexInfo;
	delete[] m_pBuffer;
}

// --------------------------------------------------------------------------------
//  Method:
//      CDbFile::Create
//
//  Description:
//      Create a new empty database file.
//
//  Inputs:
//      cTables == IN: Maximum count of tables in the file
//
//  Returns:
//      RESULT code
// --------------------------------------------------------------------------------
void CDbFile::Create
(
	UINT cTables
)
{
	RESULT result = SUCCESS;

    TRACE_INIT("CDbFile::Create");

	m_mutex.Lock();

	try
	{
		m_pFile->Create();

		// Initialize file header
		m_fileInfo.MajorVersion				= verDbEngine.Major;
		m_fileInfo.MinorVersion				= verDbEngine.Minor;

		// Initialize time stamps
		time(&m_fileInfo.Created);
		time(&m_fileInfo.LastUpdated);

		// Calculate initial data offsets
		m_fileInfo.DataOffsetStart			= sizeof(m_fileInfo);
		m_fileInfo.DataOffsetEnd			= m_fileInfo.DataOffsetStart;

		// Initialize table catalog metadata
		m_fileInfo.Tables.Size				= sizeof(DbTableInfo);
		m_fileInfo.Tables.Entries			= 0;
		m_fileInfo.Tables.Slots				= cTables;
		m_fileInfo.Tables.GrowthFactor		= DB_DEFAULT_TABLES;
		m_fileInfo.Tables.LastId			= 0;
		m_fileInfo.Tables.Offset			= m_fileInfo.DataOffsetStart;
		m_fileInfo.DataOffsetEnd		   += m_fileInfo.Tables.Size * m_fileInfo.Tables.Slots;

		m_pTableInfo = (DbTableInfo*) InitCatalog(&m_fileInfo.Tables);

		// Intialize index catalog metadata
		m_fileInfo.Indexes.Size				= sizeof(DbIndexInfo);
		m_fileInfo.Indexes.Entries			= 0;
		m_fileInfo.Indexes.Slots			= 0;
		// m_fileInfo.Indexes.Slots			= DB_DEFAULT_INDEX_CATALOG;
		m_fileInfo.Indexes.GrowthFactor		= DB_DEFAULT_GROWTH_FACTOR;
		m_fileInfo.Indexes.LastId			= 0;
		m_fileInfo.Indexes.Offset			= m_fileInfo.DataOffsetEnd;
		m_fileInfo.DataOffsetEnd		   += m_fileInfo.Indexes.Size * m_fileInfo.Indexes.Slots;

		m_pIndexInfo = (DbIndexInfo*) InitCatalog(&m_fileInfo.Indexes);

		TRACE_DEBUG_PRINT(ctime(&m_fileInfo.Created));
	}
	catch ( ... )
	{
		m_mutex.Unlock();
		throw;
	}

	m_mutex.Unlock();
}


// --------------------------------------------------------------------------------
//  Method:
//      CDbFile::Open
//
//  Description:
//      Open database file.
// --------------------------------------------------------------------------------
void CDbFile::Open()
{
	TRACE_INIT("CDbFile::Open");

	m_mutex.Lock();

	try
	{
		m_pFile->Open();
		Load();
	}
	catch ( ... )
	{
		m_mutex.Unlock();
		throw;
	}

	m_mutex.Unlock();
}

// --------------------------------------------------------------------------------
//  Method:
//      CDbFile::Close
//
//  Description:
//      Close database file.  Saves any outstanding changes to the file.  Updates
//		any open information.
// --------------------------------------------------------------------------------
void CDbFile::Close()
{
    TRACE_INIT("CDbFile::Close");

	m_mutex.Lock();

	try
	{
		Save();
		m_pFile->Close();

		// Reset internal file state
		memset(&m_fileInfo, 0, sizeof(m_fileInfo));
		delete[] m_pTableInfo;
		delete[] m_pIndexInfo;
	}
	catch ( ... )
	{
		m_mutex.Unlock();
		throw;
	}

	m_mutex.Unlock();
}

// --------------------------------------------------------------------------------
//  Method:
//      CDbFile::Save
//
//  Description:
//      Save file header, table catalog, and index catalog
// --------------------------------------------------------------------------------
void CDbFile::Save()
{
	TRACE_INIT("CDbFile::Save");

	m_mutex.Lock();

	try
	{
		UINT cbWritten = 0;
		
		// Update timestamp
		time(&m_fileInfo.LastUpdated);

		// Move to beginning of the file and write file header
		m_pFile->Seek(0);	
		cbWritten = m_pFile->Write(&m_fileInfo, sizeof(m_fileInfo));
		_ASSERTE(cbWritten == sizeof(m_fileInfo));

		// Save table and index catalogs
		cbWritten = SaveCatalog(m_pFile, &m_fileInfo.Tables,  m_pTableInfo);
		cbWritten = SaveCatalog(m_pFile, &m_fileInfo.Indexes, m_pIndexInfo);

		// Commit buffers to disk
		m_pFile->Flush();

		TRACE_DEBUG_PRINT(ctime(&m_fileInfo.LastUpdated));
	}
	catch ( ... )
	{
		m_mutex.Unlock();
		throw;
	}

	m_mutex.Unlock();
}

// --------------------------------------------------------------------------------
//  Method:
//      CDbFile::Compact
//
//  Description:
//      Compresses empty space in tables.  Re-organizes indexes and metadata tables
//
//  Inputs:
//      None
//
//  Returns:
//      RESULT code
// --------------------------------------------------------------------------------
void CDbFile::Compact()
{
    TRACE_INIT("CDbFile::Compact");

	m_mutex.Lock();

	try
	{
		UINT	cbRead		= 0;
		UINT	cbWritten	= 0;

		_ASSERTE(!m_pFile->IsOpen());
		
		// Open the source file
		if (!m_pFile->IsOpen())
			Open();

		// Open a temp file for copying
		string strFile = m_pFile->GetFilename();
		strFile.append(".tmp");
		CFilePtr pDest = new CFile(strFile);
		pDest->Create();
		
		// Recalculate file offsets
		m_fileInfo.Tables.Offset  = m_fileInfo.DataOffsetStart;
		m_fileInfo.Indexes.Offset = m_fileInfo.Tables.Offset +
											(m_fileInfo.Tables.Slots * sizeof(DbTableInfo));
		m_fileInfo.DataOffsetEnd  = m_fileInfo.Indexes.Offset +
											(m_fileInfo.Indexes.Slots * sizeof(DbIndexInfo));

		// Compress each table data area
		for (UINT iidx = 0; iidx < m_fileInfo.Tables.Entries; iidx++)
		{
			DbTableInfo* pTableInfo = m_pTableInfo + iidx;

			// Determine offsets
			FILEOFFSET idxStartOffset = pTableInfo->Offset;
			FILEOFFSET idxEndOffset	  = idxStartOffset + (pTableInfo->Size * pTableInfo->Slots);

			// Position file pointers
			m_pFile->Seek(idxStartOffset);
			pDest->Seek(m_fileInfo.DataOffsetEnd);

			// Read source data to new file and update the info offsets
			while (idxStartOffset < idxEndOffset)
			{
				// Read buffer of data
				cbRead = m_pFile->Read(m_pBuffer, min(idxEndOffset - idxStartOffset, m_cbBuffer));

				// Write buffer of data to new file
				cbWritten = pDest->Write(m_pBuffer, cbRead);
				_ASSERTE(cbRead == cbWritten);
				idxStartOffset += cbRead;
			}

			// Adjust offsets
			pTableInfo->Offset		 = m_fileInfo.DataOffsetEnd;
			m_fileInfo.DataOffsetEnd = pDest->GetPosition();
		}

		// Write file header
		pDest->Seek(0);
		cbWritten = pDest->Write(&m_fileInfo, sizeof(m_fileInfo));
		_ASSERTE(cbWritten == sizeof(m_fileInfo));

		// Write table catalog
		_ASSERTE(pDest->GetPosition() == m_fileInfo.Tables.Offset);
		SaveCatalog(pDest, &m_fileInfo.Tables, m_pTableInfo);

		// Write index catalog
		_ASSERTE(pDest->GetPosition() == m_fileInfo.Indexes.Offset);
		SaveCatalog(pDest, &m_fileInfo.Indexes,	m_pIndexInfo);

		// Close files
		Close();
		pDest->Close();

		// Delete source
		m_pFile->Delete();

		// Rename destination
		pDest->Rename(m_pFile->GetFilename());
	}
	catch ( ... )
	{
		m_mutex.Unlock();
		throw;
	}

	m_mutex.Unlock();
}

// --------------------------------------------------------------------------------
//  Method:
//      CDbFile::CreateTable
//
//  Description:
//      Create a new table in the database file.  Updates the catalog and allocates
//		a new data space for the table data.
//
//  Inputs:
//		pwszTable	== IN:	Table name
//		cbRowSize	== IN:	Size of table row (in bytes)
//		cSlots		== IN:	Initial table size (in slots)
//		iGrowth		== IN:	Growth rate (%)
//
//  Returns:
//      Pointer to new table object
// --------------------------------------------------------------------------------
CDbTable* CDbFile::CreateTable
(
	const string&	strTable,
	UINT			cbRowSize,
	UINT			cSlots,
	UINT			cGrowthFactor
)
{
	CDbTable* pTable = NULL;

	TRACE_INIT("CDbFile::CreateTable");

	m_mutex.Lock();

	try
	{
		// Check to see if a table with this name already exists
		if (FindTable(strTable) >= 0)
		{
			throw runtime_error("Table already exists");
		}

		// Get a free table slot
		INT idx = GetFreeSlot(&m_fileInfo.Tables, m_pTableInfo);

		if (idx < 0)
		{
			throw runtime_error("No free table slot found");
		}

		DbTableInfo* pTableInfo = m_pTableInfo + idx;

		// Create table in catalog
		m_fileInfo.Tables.Entries++;
		m_fileInfo.Tables.LastId++;

		// Determine the table metadata
		strncpy(pTableInfo->Name, strTable.c_str(), DB_MAX_OBJECT_NAME);

		pTableInfo->Id				= m_fileInfo.Tables.LastId;
		pTableInfo->Size			= cbRowSize;
		pTableInfo->GrowthFactor	= cGrowthFactor;
		pTableInfo->Slots			= cSlots;
		pTableInfo->Entries			= 0;
		pTableInfo->LastRecordId	= 0;
		
		// Append table data area
		AppendTableData(pTableInfo);

		pTable = new CDbTable(this, pTableInfo);
	}
	catch ( ... )
	{
		m_mutex.Unlock();
		throw;
	}

	m_mutex.Unlock();
	return pTable;
}

// --------------------------------------------------------------------------------
//  Method:
//      CDbFile::DeleteTable
//
//  Description:
//      Deletes a table from the database file.  Updates the catalog by deleting
//		the entry.  Space will be reclaimed when database is compacted.
//
//  Inputs:
//      pwszTable == IN: Table name
// --------------------------------------------------------------------------------
void CDbFile::DeleteTable
(
	const string& strTable
)
{
    TRACE_INIT("CDbFile::DeleteTable");

	m_mutex.Lock();

	try
	{
		INT idx = FindTable(strTable);
		if (idx < 0)
		{
			throw runtime_error("Table missing - cannot delete");
		}

		memset(m_pTableInfo + idx, 0, sizeof(DbTableInfo));
		m_fileInfo.Tables.Entries--;
	}
	catch ( ... )
	{
		m_mutex.Unlock();
		throw;
	}

	m_mutex.Lock();
}

// --------------------------------------------------------------------------------
//  Method:
//      CDbFile::ExpandTable
//
//  Description:
//      Expands the data space used by the table and updates the catalog.
//
//  Inputs:
//      pwszTable == IN: Table name
// --------------------------------------------------------------------------------
void CDbFile::ExpandTable
(
	const string& strTable
)
{
    TRACE_INIT("CDbFile::ExpandTable");

	m_mutex.Lock();

	try
	{
		// Move table data to end of file
		CDbTable* pTable = GetTable(strTable);

		if (pTable == NULL)
		{
			throw runtime_error("Table missing - cannot expand");
		}
		
		DbTableInfo* pTableInfo = pTable->GetTableInfo();

		// Get starting/ending points of current data set
		FILEOFFSET idxStart = pTableInfo->Offset;
		FILEOFFSET idxEnd   = idxStart + (pTableInfo->Size * pTableInfo->Slots);

		// OPTIMIZATION: Determine if table is already at the end of the file
		if (idxEnd = m_fileInfo.DataOffsetEnd)
		{
			// No need to copy data since at the end.
			// Just expand the data area
			pTableInfo->Slots		+= pTableInfo->GrowthFactor;
			m_fileInfo.DataOffsetEnd = pTableInfo->Offset + (pTableInfo->Slots * pTableInfo->Size);

			m_pFile->Expand(m_fileInfo.DataOffsetEnd, DB_EOF);
		}
		else
		{
			// Recalculate new data area
			pTableInfo->Slots += pTableInfo->GrowthFactor;
			pTableInfo->Offset = m_fileInfo.DataOffsetEnd;

			// Append table data area
			AppendTableData(pTableInfo);

			// Copy table data from old area to the new area
			CopyTableData(idxStart, pTableInfo->Offset, idxEnd - idxStart);
		}
	}
	catch ( ... )
	{
		m_mutex.Unlock();
		throw;
	}

	m_mutex.Unlock();
}

// --------------------------------------------------------------------------------
//  Method:
//      CDbFile::InitCatalog
//
//  Description:
//      Initialize in memory catalog table
//
//	Inputs:
//		pCatalog == IN: Catalog to intialize
//
//	Returns:
//		Pointer to catalog info buffer
// --------------------------------------------------------------------------------
DbObjectInfo* CDbFile::InitCatalog
(
	DbCatalog* pCatalog
)
{
	UINT  cbBuffer = pCatalog->Size * pCatalog->Slots;
	BYTE* pBuffer  = new BYTE[cbBuffer];
	memset(pBuffer, 0 , cbBuffer);
	return (DbObjectInfo*) pBuffer;
}

// --------------------------------------------------------------------------------
//  Method:
//      CDbFile::ExpandCatalog
//
//  Description:
//      Relocate catalog and expand by growth factor
//
//	Inputs:
//		pCatalog	== IN:		Catalog
//		pInfo		== IN/OUT:	Catalog buffer pointer
// --------------------------------------------------------------------------------
void CDbFile::ExpandCatalog
(
	DbCatalog*		pCatalog,
	DbObjectInfo*	pInfo
)
{
	// Create new catalog data area
	DbCatalog tmpCatalog = *pCatalog;
	tmpCatalog.Slots += tmpCatalog.GrowthFactor;
	DbObjectInfo* pBuffer = InitCatalog(&tmpCatalog);

	// Copy existing catalog
	memcpy(pBuffer, pInfo, pCatalog->Size * pCatalog->Slots);

	// Release old catalog buffer
	delete[] pInfo;

	// Set info buffer
	pInfo = pBuffer;

	// Update catalog
	pCatalog->Slots			 += pCatalog->GrowthFactor;
	pCatalog->Offset		  = m_fileInfo.DataOffsetEnd;
	m_fileInfo.DataOffsetEnd += pCatalog->Size * pCatalog->Slots;
}

// --------------------------------------------------------------------------------
//  Method:
//      CDbFile::Load
//
//  Description:
//      Load file header, table catalog, and index catalog
// --------------------------------------------------------------------------------
void CDbFile::Load()
{
	TRACE_INIT("CDbFile::Load");

	m_mutex.Lock();

	try
	{
		// Move to the file header and read
		m_pFile->Seek(0);
		UINT cbRead = m_pFile->Read(&m_fileInfo, sizeof(m_fileInfo));
		_ASSERTE(cbRead == sizeof(m_fileInfo));

		// Initialize catalog buffers
		m_pTableInfo = (DbTableInfo*) InitCatalog(&m_fileInfo.Tables);
		m_pIndexInfo = (DbIndexInfo*) InitCatalog(&m_fileInfo.Indexes);

		// Read catalogs
		LoadCatalog(m_pFile, &m_fileInfo.Tables,  m_pTableInfo);
		LoadCatalog(m_pFile, &m_fileInfo.Indexes, m_pIndexInfo);
	}
	catch ( ... )
	{
		m_mutex.Unlock();
		throw;
	}

	m_mutex.Unlock();
}

// --------------------------------------------------------------------------------
//  Method:
//      CDbFile::LoadCatalog
//
//  Description:
//      Load catalog
//
//	Inputs:
//		pFile		== IN:	Source file
//		pCatalog	== IN:	Metadata catalog
//		pBuffer		== IN:	Catalog buffer
//		
//  Returns:
//		Count of catalog metadata records loaded
// --------------------------------------------------------------------------------
UINT CDbFile::LoadCatalog
(
	CFile*			pFile,
	DbCatalog*		pCatalog,
	DbObjectInfo*	pBuffer
)
{
	// Find catalog
	pFile->Seek(pCatalog->Offset);

	// Read catalog
	UINT cbRead = pFile->Read(pBuffer, pCatalog->Size * pCatalog->Slots);
	_ASSERTE(cbRead == (pCatalog->Size * pCatalog->Slots));	

	return (cbRead / pCatalog->Size);
}

// --------------------------------------------------------------------------------
//  Method:
//      CDbFile::SaveCatalog
//
//  Description:
//      Save catalog
//
//	Inputs:
//		pFile		== IN:	Destination file
//		pCatalog	== IN:	Metadata catalog
//		pBuffer		== IN:	Catalog buffer
//
//  Returns:
//		Count of catalog metadata records saved
// --------------------------------------------------------------------------------
UINT CDbFile::SaveCatalog
(
	CFile*			pFile,
	DbCatalog*		pCatalog,
	DbObjectInfo*	pBuffer
)
{
	// Move to file position
	pFile->Seek(pCatalog->Offset);

	// Write catalog to file
	UINT cbWritten = pFile->Write(pBuffer, pCatalog->Size * pCatalog->Slots);
	_ASSERTE(cbWritten == (pCatalog->Size * pCatalog->Slots));

	return (cbWritten / pCatalog->Size);
}

// --------------------------------------------------------------------------------
//  Method:
//      CDbFile::GetTable
//
//  Description:
//      Access database table by requesting the table by name
//
//  Inputs:
//      strTable == IN: Table name
//
//  Returns:
//		Pointer to the table object
// --------------------------------------------------------------------------------
CDbTable* CDbFile::GetTable
(
	const string& strTable
)
{
	CDbTable* pTable = NULL;

	m_mutex.Lock();

	try
	{
		// Search the table catalog for the tableinfo
		INT idx = FindTable(strTable);

		if (idx >= 0)
		{
			pTable = new CDbTable(this, m_pTableInfo + idx);
		}
	}
	catch ( ... )
	{
		m_mutex.Unlock();
		throw;
	}
	
	m_mutex.Unlock();
	return pTable;
}

// --------------------------------------------------------------------------------
//  Method:
//      CDbFile::FindTable
//
//  Description:
//      Find slot for specified table
//
//  Inputs:
//      strTable == IN: Table name
//
//  Returns:
//		Table slot
// --------------------------------------------------------------------------------
INT CDbFile::FindTable
(
	const string& strTable
)
{
	// Search the table catalog for the tableinfo
	for (UINT idx = 0; idx < m_fileInfo.Tables.Entries; idx++)
	{
		DbTableInfo* pTableInfo = m_pTableInfo + idx;

		if (strncmp(strTable.c_str(), pTableInfo->Name, DB_MAX_OBJECT_NAME) == 0)
		{
			return idx;
		}
	}

	return -1;
}

// --------------------------------------------------------------------------------
//  Method:
//      CDbFile::AppendTableData
//
//  Description:
//      Add table data area to end of the file
//
//  Inputs:
//      pTableInfo == IN: Table descriptor
// --------------------------------------------------------------------------------
void CDbFile::AppendTableData
(
	DbTableInfo* pTableInfo
)
{
	// Calculate data position
	pTableInfo->Offset		 = m_fileInfo.DataOffsetEnd;
	m_fileInfo.DataOffsetEnd = m_fileInfo.DataOffsetEnd + (pTableInfo->Size * pTableInfo->Slots);

	// Mark end of data area in the disk file
	m_pFile->Expand(m_fileInfo.DataOffsetEnd, DB_EOF);
}

// --------------------------------------------------------------------------------
//  Method:
//      CDbFile::GetFreeSlot
//
//  Description:
//      Search catalog for a free slot
//
//	Inputs:
//		pCatalog	== IN:	Metadata catalog to search
//		pInfo		== IN:	Catalog info buffer
//
//	Returns:
//		Index of free slot; -1 if not found
// --------------------------------------------------------------------------------
INT CDbFile::GetFreeSlot
(
	DbCatalog*		pCatalog,
	DbObjectInfo*	pInfo
)
{
	// Walk the catalog for empty slots
	for (UINT idxSlot = 0; idxSlot < pCatalog->Slots; idxSlot++)
	{
		DbObjectInfo* pRec = (DbObjectInfo*) (((BYTE*)pInfo) + (idxSlot * pCatalog->Size));
		if (pRec->Id == 0)
		{
			return idxSlot;
		}
	}

	// Catalog is full - expand
	ExpandCatalog(pCatalog, pInfo);
	return (idxSlot + 1);
}

// --------------------------------------------------------------------------------
//  Method:
//      CDbFile::SwapTableInfo
//
//  Description:
//      Switch positions of table info records in the catalog
//
//  Inputs:
//      idxSrc	== IN:	Source slot
//		idxDest	== IN:	Destination slot
// --------------------------------------------------------------------------------
void CDbFile::SwapTableInfo
(
	UINT idxSrc,
	UINT idxDest
)
{
	if (idxSrc >= m_fileInfo.Tables.Slots || idxSrc < 0)
	{
		throw out_of_range("Source table out of range");
	}

	if (idxDest >= m_fileInfo.Tables.Slots || idxDest < 0)
	{
		throw out_of_range("Destination table out of range");
	}

	DbTableInfo* pSrcTable	= m_pTableInfo + idxSrc;
	DbTableInfo* pDestTable	= m_pTableInfo + idxDest;
	DbTableInfo  tblInfo;

	tblInfo     = *pDestTable;
	*pDestTable = *pSrcTable;
	*pSrcTable  = tblInfo;
}

// --------------------------------------------------------------------------------
//  Method:
//      CDbFile::CopyTableData
//
//  Description:
//      Copies table data from source offset to destination offset
//
//  Inputs:
//      idxSrc	== IN:	Source offset
//		idxDest	== IN:	Destination offset
//		cbLen	== IN:	Bytes to copy
// --------------------------------------------------------------------------------
void CDbFile::CopyTableData
(
	FILEOFFSET	idxSrc,
	FILEOFFSET	idxDest,
	UINT		cbLen
)
{
	FILEOFFSET	idxEnd		= idxSrc + cbLen;
	UINT		cbRead		= 0;
	UINT		cbWritten	= 0;

	TRACE_INIT("CDbFile::CopyTableData");

	while (idxSrc < idxEnd)
	{
		// Read source data
		m_pFile->Seek(idxSrc);
		cbRead = m_pFile->Read(m_pBuffer, min(m_cbBuffer, (idxEnd - idxSrc)));
		_ASSERTE(cbRead == min(m_cbBuffer, (idxEnd - idxSrc)));

		// Write to destination
		m_pFile->Seek(idxDest);
		cbWritten = m_pFile->Write(m_pBuffer, cbRead);
		_ASSERTE(cbWritten == cbRead);

		// Update offsets
		idxSrc	+= cbRead;
		idxDest	+= cbWritten;
	}
}

