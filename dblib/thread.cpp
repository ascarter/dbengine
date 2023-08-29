// ================================================================================
//
//	File:
//      thread.cpp
//
//	Component:
//      System
//
//	Description:
//      Thread object implementation
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
//      CThread::CThread
//
//  Description:
//      Default constructor
// --------------------------------------------------------------------------------
CThread::CThread()
{
}

// --------------------------------------------------------------------------------
//  Method:
//      CThread::~CThread
//
//  Description:
//      Default destructor
// --------------------------------------------------------------------------------
CThread::~CThread()
{
}

// ================================================================================
//	THREAD FUNCTIONS
// ================================================================================

// --------------------------------------------------------------------------------
//  Method:
//      CThread::Create
//
//  Description:
//      Initialize thread
// --------------------------------------------------------------------------------
void CThread::Init
(
	PTHREAD_FUNCTION	pFunction,
	void*				pArg,
	bool				fSuspended,
	THREAD_STACK		stack
)
{
#ifdef WIN32
	m_ThreadID = (HANDLE) _beginthreadex(NULL,
										 stack,
										 pFunction,
										 pArg,
										 fSuspended ? CREATE_SUSPENDED : 0,
										 &m_ThreadAddr);
	if (!m_ThreadID)
	{
		throw runtime_error("Thread could not be created");
	}
#elif LINUX
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	if (pthread_create(&m_ThreadAddr, &attr, pFunction, pArg))
	{
		throw runtime_error("Thread could not be created");
	}
#endif
}

// --------------------------------------------------------------------------------
//  Method:
//      CThread::Suspend
//
//  Description:
//      Suspend thread execution
// --------------------------------------------------------------------------------
void CThread::Suspend()
{
#if defined (__WIN32__)
	SuspendThread(m_ThreadID);
#elif defined (__LINUX__)

#endif
}

// --------------------------------------------------------------------------------
//  Method:
//      CThread::Resume
//
//  Description:
//      Resume thread execution
// --------------------------------------------------------------------------------
void CThread::Resume()
{
#if defined (__WIN32__)
	ResumeThread(m_ThreadID);
#elif defined (__LINUX__)

#endif
}

// --------------------------------------------------------------------------------
//  Method:
//      CThread::Close
//
//  Description:
//      Close and exit thread
// --------------------------------------------------------------------------------
void CThread::Close()
{
#if defined (__WIN32__)
	ExitThread(-1);
#elif defined (__LINUX__)

#endif
}

// --------------------------------------------------------------------------------
//  Method:
//      CThread::Destroy
//
//  Description:
//      Kill thread
// --------------------------------------------------------------------------------
void CThread::Destroy()
{
#if defined (__WIN32__)
	TerminateThread(m_ThreadID, -1);
#elif defined (__LINUX__)

#endif
}

// --------------------------------------------------------------------------------
//  Method:
//      CThread::Cancel
//
//  Description:
//      Cancel thread
// --------------------------------------------------------------------------------
void CThread::Cancel()
{
#if defined (__WIN32__)

#elif defined (__LINUX__)

#endif
}


