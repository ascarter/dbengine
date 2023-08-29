// ================================================================================
//
//	File:
//      file.h
//
//	Component:
//      System
//
//	Description:
//      File object interface
//
//	Author:
//		andrewc
// --------------------------------------------------------------------------------
//  Copyright (c) 2001-2004 Andrew Carter
//  All rights reserved
// ================================================================================

#ifndef __FILE_H__
#define __FILE_H__

#include "sys.h"

// --------------------------------------------------------------------------------
//	TYPEDEF
// --------------------------------------------------------------------------------
typedef UINT FILEOFFSET;


SmartPointer(CFile);

// ================================================================================
// Class:
//      CFile
//
//  Description:
//      Operating system file object.  Binary access to low level disk file.
// ================================================================================
class CFile : public CObject
{
public:
	CFile(const string& strFile);
	virtual ~CFile();

	void Create();
	void Open();
	void Close();
	UINT Read(void* pBuffer, UINT cbLen);
	UINT Write(void* pBuffer, UINT cbLen);
	void Seek(FILEOFFSET position);
	void Rename(const string& strName);
	void Delete();
	void Copy(const string& strName);
	void Expand(FILEOFFSET position, UINT uiValue);
	void Flush()						{ fflush(m_hFile); }

	bool Exists();
	bool IsOpen()						{ return (m_hFile != NULL); }
	bool IsEOF()						{ return (feof(m_hFile) != 0); }

	const string GetFilename()			{ return m_strFile; }
	FILEOFFSET GetPosition()			{ return ftell(m_hFile); }

	UINT GetFileSize();

private:
	string	m_strFile;		// File name
	FILE*	m_hFile;		// File handle
	CMutex	m_mutex;		// Mutex to serialize file access
};

#endif // __DBFILE_H__


