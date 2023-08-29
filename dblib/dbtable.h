// ================================================================================
//
//	File:
//      dbtable.h
//
//	Component:
//      Database Engine
//
//	Description:
//      Table object definition
//
// --------------------------------------------------------------------------------
//  Copyright (c) 2001-2004 Andrew Carter
//  All rights reserved
// ================================================================================

#ifndef __DBTABLE_H__
#define __DBTABLE_H__

// --------------------------------------------------------------------------------
//	TYPEDEFS
// --------------------------------------------------------------------------------
typedef UINT DBPOS;

class CDbRecord;
class CDbFile;

// ================================================================================
// Class:
//      CDbTable
//
//  Description:
//      Database table object.  Defines interface to table and access methods to
//		retrieve records.
// ================================================================================
class CDbTable : public CObject
{
public:
	CDbTable(CDbFile* pdbFile, DbTableInfo* pTableInfo);
	~CDbTable();

	CDbTable(const CDbTable& rTable);
	CDbTable& operator=(const CDbTable& table);

	// ----------------------------------------------------------------------------
	// NAVIGATION
	// ----------------------------------------------------------------------------

	DBPOS Find(DBRECID id);
	UINT  Fetch(DbRecord* prgRecords, UINT cRecords);

	DBPOS Move(UINT cSkip);
	void  MoveFirst()						{ m_idxSlot = 0; }
	void  MoveLast()						{ m_idxSlot = m_pTableInfo->Entries; }

	void  SetPosition(DBPOS idxSlot)		{ m_idxSlot = idxSlot; }
	DBPOS GetPosition()						{ return m_idxSlot; }

	// ----------------------------------------------------------------------------
	// MODIFICATION
	// ----------------------------------------------------------------------------

	UINT Insert(DbRecord* prgRecords, UINT cRecords = 1);
	UINT Update(DbRecord* prgRecords, UINT cRecords = 1);
	UINT Delete(DbRecord* prgRecords, UINT cRecords = 1);

    // ----------------------------------------------------------------------------
    // PROPERTIES
    // ----------------------------------------------------------------------------
    
	bool IsEOF()									{ return m_pFile->IsEOF(); }

	UINT GetRecordCount();
	UINT GetSlotCount();
	UINT GetFreeSlotCount();

	const string GetTableName()						{ return m_pTableInfo->Name; }

private:
	DbTableInfo*	GetTableInfo()						{ return m_pTableInfo; }
	void			SetTableInfo(DbTableInfo* pTblInfo)	{ m_pTableInfo = pTblInfo; }
	FILEOFFSET		FindRecordOffset(DBRECID id);

private:
	CDbFilePtr		m_pdbFile;			// Pointer to data file
	CFilePtr		m_pFile;			// Pointer to disk file
	DbTableInfo*	m_pTableInfo;		// Table metadata
	DBPOS			m_idxSlot;			// Current cursor slot

	DbRecord*		m_pBuffer;			// Record handling buffer
	UINT			m_cbBuffer;			// Record handler buffer size

	friend class CDbFile;
};

#endif // __DBTABLE_H__


