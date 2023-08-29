// ================================================================================
//
//	File:
//      dbfile.h
//
//	Component:
//      Database Engine
//
//	Description:
//      Database engine interface.
//
// --------------------------------------------------------------------------------
//  Copyright (c) 2001-2004 Andrew Carter
//  All rights reserved
// ================================================================================

#ifndef __DBFILE_H__
#define __DBFILE_H__

// Data operations buffer size (default)
const UINT DB_DATA_BUFFER	= 4096;
const UINT DB_EOF			= 0xFFFFFFFF;

class CDbTable;
class CFile;

// ================================================================================
// Class:
//      CDbFile
//
//  Description:
//      Database file object.  Encapsulates the database file structure.
// ================================================================================
class CDbFile : public CObject
{
public:
	CDbFile(const string& strFile, UINT cbBuffer = DB_DATA_BUFFER);
	~CDbFile();

	// ----------------------------------------------------------------------------
	//	DATA FILE OPERATIONS
	// ----------------------------------------------------------------------------

	void Create(UINT cTables = DB_DEFAULT_TABLES);
	void Open();
	void Close();
	void Save();
	void Compact();
	bool Exists()				{ return m_pFile->Exists(); }
	void Delete()				{ m_pFile->Delete(); }
	UINT GetFileSize()			{ return m_pFile->GetFileSize(); }

	// ----------------------------------------------------------------------------
	//	TABLE OPERATIONS
	// ----------------------------------------------------------------------------

	CDbTable*	CreateTable	(
							const string&	strTable,
							UINT			cbRowSize,
							UINT			cSlots			= DB_DEFAULT_SLOTS,
							UINT			cGrowthFactor	= DB_DEFAULT_GROWTH_FACTOR
							);

	void		DeleteTable(const string& strTable);
	void		ExpandTable(const string& strTable);
	CDbTable*	GetTable(const string& strTable);


private:
	void Load();
	UINT LoadCatalog(CFile*	pFile, DbCatalog* pCatalog,	DbObjectInfo* pBuffer);
	UINT SaveCatalog(CFile* pFile, DbCatalog* pCatalog, DbObjectInfo* pBuffer);

	DbObjectInfo*	InitCatalog(DbCatalog* pCatalog);
	void			ExpandCatalog(DbCatalog* pCatalog, DbObjectInfo* pInfo);

	INT	 FindTable(const string& strTable);
	INT	 GetFreeSlot(DbCatalog* pCatalog, DbObjectInfo* pInfo);
	void AppendTableData(DbTableInfo* pTableInfo);
	void CopyTableData(FILEOFFSET idxSrc, FILEOFFSET idxDest, UINT cbLen);
	void SwapTableInfo(UINT idxSrc, UINT idxDest);

private:
	CMutex				m_mutex;			// Access lock
	CFilePtr			m_pFile;			// File object
	DbFileInfo			m_fileInfo;			// File info header
	DbTableInfo*		m_pTableInfo;		// Table catalog
	DbIndexInfo*		m_pIndexInfo;		// Index catalog
	UINT				m_cbBuffer;			// Data operations buffer size
	BYTE*				m_pBuffer;			// Data operations buffer

	// Special access to the base class to avoid exposing file functions
	friend class CDbTable;
};

#endif // __DBFILE_H__


