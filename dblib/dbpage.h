// ================================================================================
//
//	File:
//      dbpage.h
//
//	Component:
//      Database Engine
//
//	Description:
//      Database Page object definition.  Used to control page manipulation.
//
// --------------------------------------------------------------------------------
//  Copyright (c) 2001-2004 Andrew Carter
//  All rights reserved
// ================================================================================

#ifndef __DBPAGE_H__
#define __DBPAGE_H__

// ================================================================================
// Class:
//      CDbPage
//
//  Description:
//      Database page object.  Controls page manipulation and is used to signal
//		next page requests.
// ================================================================================
class CDbPage : public CObject
{
public:
	CDbPage(const CDbBufferManager* pBufferMgr, DbPage* pdbPage);
	~CDbPage();

	AddRecord(const DbRecord* pRecord);
	EraseRecord(const RECID rid);
	GetRecord(const RECID rid);

	GetPageID()					{ return m_pdbPage->PageID;     }
	GetNextPageID()				{ return m_pdbPage->NextPageID; }

private:
	Read();
	Write();

private:
	CMutex		m_mutex;		// Access lock
	DbPage*		m_pdbPage;		// Data page
};

#endif // __DBFILE_H__


