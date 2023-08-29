// ================================================================================
//
//	File:
//      rwlock.h
//
//	Component:
//      Photon System
//
//	Description:
//      Reader/Writer lock object
//
//	Author:
//		andrewc
// --------------------------------------------------------------------------------
//  Copyright (c) 2001-2004 Andrew Carter
//  All rights reserved
// ================================================================================

#ifndef __RWLOCK_H__
#define __RWLOCK_H__

#include "types.h"
#include "mutex.h"
#include "semaphore.h"
#include "gate.h"

// ================================================================================
//  Class:
//      CRWLock
//
//  Description:
//      Reader/writer lock class.  Allows multiple simultaneous reads and only one
//		write.
// ================================================================================
class CRWLock : private CMutex
{
public:
	CRWLock();
	void ReadLock();
	void WriteLock();
	void Unlock();

private:
  CSemaphore	m_WriteLock;		// used as a one-at-a-time release valve
  CGate			m_ReadBarrier;		// used to block/wakeup readers
  UINT			m_cWriter;			// # of writers waiting for or holding the lock
  UINT			m_cReader;			// # of readers holding the lock
};

// ================================================================================
//	INLINE FUNCTIONS
// ================================================================================

// --------------------------------------------------------------------------------
//  Method:
//      CRWLock::CRWLock
//
//  Description:
//      Constructor
// --------------------------------------------------------------------------------
inline CRWLock::CRWLock()
{
	m_cWriter = 0;
	m_cReader = 0;

	// Initialize semaphore count to one
	m_WriteLock.Post();
}

// --------------------------------------------------------------------------------
//  Method:
//      CRWLock::ReadLock
//
//  Description:
//      Acquire read lock
// --------------------------------------------------------------------------------
inline void CRWLock::ReadLock()
{
	CMutex::Lock();

	// Wait until there are no more writers holding the lock
	while (m_cWriter > 0)
	{
		CMutex::Unlock();
		m_ReadBarrier.Wait();
		CMutex::Lock();
	}

	m_cReader++;
	CMutex::Unlock();
}

// --------------------------------------------------------------------------------
//  Method:
//      CRWLock::WriteLock
//
//  Description:
//      Acquire exclusive write lock
// --------------------------------------------------------------------------------
inline void CRWLock::WriteLock()
{
    CMutex::Lock();

	// Stop new readers from getting lock
    m_cWriter++;

    // Wait for write lock to become available
	m_WriteLock.Wait();

    // Give new readers something to wait for
	m_ReadBarrier.Close();

	CMutex::Unlock();
}

// --------------------------------------------------------------------------------
//  Method:
//      CRWLock::Unlock
//
//  Description:
//      Release lock (reader or writer)
// --------------------------------------------------------------------------------
inline void CRWLock::Unlock()
{
    CMutex::Lock();

	if (m_cWriter > 0)
	{
		m_cWriter--;
		m_WriteLock.Post();

		if (m_cWriter == 0)
		{
			m_ReadBarrier.Open();
		}
	}
	else
	{
		m_cReader--;

		// If last reader and a writer is waiting, let it go
		if ((m_cReader == 0) && (m_cWriter > 0))
		{
			m_WriteLock.Post();
		}
	}

	CMutex::Unlock();
}


#endif // __RWLOCK_H__


