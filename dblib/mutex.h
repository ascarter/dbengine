// ================================================================================
//
//	File:
//      mutex.h
//
//	Component:
//      Photon System
//
//	Description:
//      Mutex object
//
//	Author:
//		andrewc
// --------------------------------------------------------------------------------
//  Copyright (c) 2001-2004 Andrew Carter
//  All rights reserved
// ================================================================================

#ifndef __MUTEX_H__
#define __MUTEX_H__

// ================================================================================
//  Class:
//      CMutex
//
//  Description:
//      Mutex class.
// ================================================================================
class CMutex
{
public:
	CMutex();
	virtual ~CMutex();

	void Lock();
	void Unlock();

private:
#if defined (__WIN32__)
	CRITICAL_SECTION	m_mutex;		// Win32 synchronization object
#elif defined (__LINUX__)
	pthread_mutex_t		m_mutex;		// POSIX compliant mutex object
#endif
};

#endif // __MUTEX_H__


