// ================================================================================
//
//	File:
//		file.cpp
//
//	Component:
//		System
//
//	Description:
//		Disk file object implementation
//
// --------------------------------------------------------------------------------
//  Copyright (c) 2001-2004 Andrew Carter
//  All rights reserved
// ================================================================================

#include "sys.h"
#include "trace.h"
#include "file.h"

// --------------------------------------------------------------------------------
//  Method:
//      CFile::CFile
//
//  Description:
//      Default constructor
// --------------------------------------------------------------------------------
CFile::CFile(const string& strName)
{
	m_hFile		= NULL;
	m_strFile	= strName;
}

// --------------------------------------------------------------------------------
//  Method:
//      CFile::~CFile
//
//  Description:
//      Default destructor
// --------------------------------------------------------------------------------
CFile::~CFile()
{
	Close();
}

// --------------------------------------------------------------------------------
//  Method:
//      CFile::Create
//
//  Description:
//      Create file for binary read/write
//
//	Exceptions:
//		runtime_error == File already opened or file exists
// --------------------------------------------------------------------------------
void CFile::Create()
{
	m_mutex.Lock();

	try
	{
		if (m_hFile)
		{
			throw runtime_error("File already open");
		}

		if (Exists())
		{
			throw runtime_error("File already exists");
		}

		m_hFile = fopen(m_strFile.c_str(), "w+bc");

		if (!m_hFile)
		{
			throw runtime_error("File creation failed");
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
//      CFile::Open
//
//  Description:
//      Open file for binary read/write
//
//	Exceptions:
//		runtime_error == File already opened
// --------------------------------------------------------------------------------
void CFile::Open()
{
    TRACE_INIT("CFile::Open");

	m_mutex.Lock();

	try
	{
		if (m_hFile)
		{
			throw runtime_error("File already open");
		}

		m_hFile = fopen(m_strFile.c_str(), "r+bc");

		if (!m_hFile)
		{
			throw runtime_error("File open failed");
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
//      CFile::Close
//
//  Description:
//      Close file
// --------------------------------------------------------------------------------
void CFile::Close()
{
	m_mutex.Lock();

	try
	{
		if (m_hFile)
		{
			fflush(m_hFile);
			
			if (fclose(m_hFile))
			{
				throw runtime_error("File could not be closed");
			}

			m_hFile = NULL;
		}
	}
	catch( ... )
	{
		m_mutex.Unlock();
		throw;
	}

	m_mutex.Unlock();
}

// --------------------------------------------------------------------------------
//  Method:
//      CFile::Read
//
//  Description:
//      Read n bytes from file
//
//  Inputs:
//      pBuffer	== OUT: Output buffer
//		cbLen	== IN:	Count of bytes to read
//
//	Returns:
//		Count of bytes read
//
//  Exceptions:
//		invalid_argument == read buffer is invalid
//		runtime_error	 == read fails
// --------------------------------------------------------------------------------
UINT CFile::Read
(
	void*	pBuffer,
	UINT	cbLen
)
{
	UINT cbRead = 0;

	m_mutex.Lock();

	try
	{
		if (!pBuffer)
		{
			throw invalid_argument("Read buffer invalid");
		}

		// Read up to next cbLen bytes
		cbRead =  (UINT) fread(pBuffer, 1, cbLen, m_hFile);
		_ASSERTE(cbLen == cbRead);

		if (ferror(m_hFile))
		{
			throw runtime_error("Read failed");
		}
	}
	catch( ... )
	{
		m_mutex.Unlock();
		throw;
	}

	m_mutex.Unlock();
	return cbRead;
}

// --------------------------------------------------------------------------------
//  Method:
//      CFile::Write
//
//  Description:
//      Write n bytes to file
//
//  Inputs:
//      pBuffer		== IN:	Input buffer
//		cbLen		== IN:	Count of bytes to write
//
//	Returns:
//		Count of bytes written
//
//  Exceptions:
//		invalid_argument == read buffer is invalid
//		runtime_error	 == read fails
// --------------------------------------------------------------------------------
UINT CFile::Write
(
	void*	pBuffer,
	UINT	cbLen
)
{
	UINT cbWritten = 0;

	m_mutex.Lock();

	try
	{
		if (!pBuffer)
		{
			throw invalid_argument("Input buffer invalid");
		}

		// Write cbLen bytes
		cbWritten = (UINT) fwrite(pBuffer, 1, cbLen, m_hFile);
		_ASSERTE(cbLen == cbWritten);

		if (ferror(m_hFile))
		{
			throw runtime_error("Write failed");
		}
	}
	catch( ... )
	{
		m_mutex.Unlock();
		throw;
	}

	m_mutex.Unlock();
	return cbWritten;
}

// --------------------------------------------------------------------------------
//  Method:
//      CFile::Seek
//
//  Description:
//      Sets file position
//
//  Inputs:
//      position == IN: Absolute file offset position
//
//	Exceptions:
//		runtime_error == Seek position op failed
// --------------------------------------------------------------------------------
void CFile::Seek
(
	FILEOFFSET position
)
{
	m_mutex.Lock();

	try
	{
		if (fseek(m_hFile, position, SEEK_SET))
		{
			throw runtime_error("Seek for read position failed");
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
//      CFile::Rename
//
//  Description:
//      Rename file to new name
//
//  Inputs:
//		strName	== IN:	New filename
//
//  Exceptions:
//      runtime_error == rename op failed
// --------------------------------------------------------------------------------
void CFile::Rename
(
	const string& strName
)
{
	m_mutex.Lock();

	try
	{
		if (rename(m_strFile.c_str(), strName.c_str()) != 0)
		{
			throw runtime_error("Rename operation failed");
		}

		m_strFile = strName;
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
//      CFile::Delete
//
//  Description:
//      Delete file
//
// --------------------------------------------------------------------------------
void CFile::Delete()
{
	m_mutex.Lock();

	try
	{
		if (remove(m_strFile.c_str()) != 0)
		{
			throw runtime_error("Delete operation failed");
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
//      CFile::Copy
//
//  Description:
//      Copy to destination file
//
//  Inputs:
//		pszName	== IN:	Destination file name
//
//  Exceptions:
//      runtime_error == Write to destination failed
// --------------------------------------------------------------------------------
void CFile::Copy
(
	const string& strName
)
{
	m_mutex.Lock();

	try
	{
		CFile	fileDest(strName);
		auto_ptr<BYTE>	pBuffer(new BYTE[4096]);

		// Open destination stream
		fileDest.Open();

		if (!IsOpen())
		{
			Open();
		}

		// Seek read to beginning
		Seek(0);
		fileDest.Seek(0);

		// Copy file in blocks
		while (!IsEOF())
		{
			// Read from source file
			UINT cbRead = Read(pBuffer.get(), 4096);

			// Output to destination file
			UINT cbWritten = fileDest.Write(pBuffer.get(), cbRead);
			_ASSERTE(cbRead == cbWritten);
		}

		// Commit and close destination file
		fileDest.Close();
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
//      CFile::Expand
//
//  Description:
//      Mark value at specified offset in order to expand the file on disk.
//		The value mark causes file to be physically expanded.
//
//  Inputs:
//      position	== IN:	Absolution position to mark file
//		uiValue		== IN:	Dummy value to mark at expansion point
//
//  Returns:
//      RESULT code
// --------------------------------------------------------------------------------
void CFile::Expand
(
	FILEOFFSET	position,
	UINT		uiValue
)
{
	m_mutex.Lock();

	try
	{
		// Verify that mark is beyond end of file
		if (GetFileSize() < position)
		{
			// Expand file and mark
			Seek(position);
			UINT cbWritten = Write(&uiValue, sizeof(uiValue));
			_ASSERTE(cbWritten == sizeof(uiValue));
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
//      CFile::GetFileSize
//
//  Description:
//      Returns size of file in bytes
//
//  Returns:
//      File size in bytes
// --------------------------------------------------------------------------------
UINT CFile::GetFileSize()
{
	FILE*	hFile	= NULL;
	UINT	cbSize	= 0;

	m_mutex.Lock();

	try
	{
		hFile = fopen(m_strFile.c_str(), "rb");		
		
		if (!hFile)
		{
			throw runtime_error("File could not be opened");
		}

		if (fseek(hFile, 0, SEEK_END))
		{
			throw runtime_error("Seek failed");
		}

		cbSize = ftell(hFile);

		if (hFile)
		{
			fclose(hFile);
		}
	}
	catch ( ... )
	{
		if (hFile)
		{
			fclose(hFile);
		}

		m_mutex.Unlock();
		throw;
	}

	m_mutex.Unlock();
	return cbSize;
}

// --------------------------------------------------------------------------------
//  Method:
//      CFile::Exists
//
//  Description:
//      Tests to see if the file currently exists
//
//  Returns:
//      true/false for if file exists
// --------------------------------------------------------------------------------
bool CFile::Exists()
{
	FILE* hFile;

	hFile = fopen(m_strFile.c_str(), "rb");
	if (hFile)
	{
		fclose(hFile);
		return true;
	}

	return false;
}
