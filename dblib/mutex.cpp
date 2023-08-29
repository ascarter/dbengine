// ================================================================================
//
//	File:
//      mutex.cpp
//
//	Component:
//      Photon System
//
//	Description:
//      Mutex object implementation
//
//	Author:
//		andrewc
// --------------------------------------------------------------------------------
//  Copyright (c) 2001-2004 Andrew Carter
//  All rights reserved
// ================================================================================

#include "sys.h"

// --------------------------------------------------------------------------------
//  Method:
//      CMutex::CMutex
//
//  Description:
//      Constructor -- initializes critical section/mutex object
// --------------------------------------------------------------------------------
CMutex::CMutex()
{
#ifdef WIN32
	InitializeCriticalSection(&m_mutex);
#elif LINUX
	pthread_mutexattr_t	attr;
	pthread_mutexattr_init(&attr);
	pthread_mutex_init(&m_mutex, &attr);
#endif
}
	
// --------------------------------------------------------------------------------
//  Method:
//      CMutex::~CMutex
//
//  Description:
//      Destructor -- clears critical section/mutex object
// --------------------------------------------------------------------------------
CMutex::~CMutex()
{
#ifdef WIN32
	DeleteCriticalSection(&m_mutex);
#elif LINUX
	pthread_mutex_destroy(&m_mutex);
#endif
}

// --------------------------------------------------------------------------------
//  Method:
//      CMutex::Lock
//
//  Description:
//      Locks mutex (blocks until lock acquired)
// --------------------------------------------------------------------------------
void CMutex::Lock()
{
#ifdef WIN32
	EnterCriticalSection(&m_mutex);
#elif LINUX
	pthread_mutex_lock(&m_mutex);
#endif
}
	
// --------------------------------------------------------------------------------
//  Method:
//      CMutex::Unlock
//
//  Description:
//      Releases mutex lock
// --------------------------------------------------------------------------------
void CMutex::Unlock()
{
#ifdef WIN32
	LeaveCriticalSection(&m_mutex);
#elif LINUX
	pthread_mutex_unlock(&m_mutex);
#endif
}

