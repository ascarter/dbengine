// ================================================================================
//
//	File:
//      semaphore.cpp
//
//	Component:
//      Photon System
//
//	Description:
//      Semaphore object implementation
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
//      CSemaphore::CSemaphore
//
//  Description:
//      Constructor
// --------------------------------------------------------------------------------
inline CSemaphore::CSemaphore()
{
#if defined (__WIN32__)
	m_semaphore = CreateSemaphore(NULL, 0, 0x7ffffff, NULL);
#elif defined (__LINUX__)
	m_semaphore = semget(IPC_PRIVATE, 1, 0666);
#endif
}
	
// --------------------------------------------------------------------------------
//  Method:
//      CSemaphore::CSemaphore
//
//  Description:
//      Constructor
//
//	Inputs:
//		iCount == IN: Initial semaphore count
// --------------------------------------------------------------------------------
inline CSemaphore::CSemaphore(int iCount)
{
#if defined (__WIN32__)
	m_semaphore = CreateSemaphore(NULL, iCount, 0x7ffffff, NULL);
#elif defined (__LINUX__)
	m_semaphore = semget(IPC_PRIVATE, iCount, 0666);
#endif
}

// --------------------------------------------------------------------------------
//  Method:
//      CSemaphore::~CSemaphore
//
//  Description:
//      Destructor
// --------------------------------------------------------------------------------
inline CSemaphore::~CSemaphore()
{
#if defined (__WIN32__)
	CloseHandle(m_semaphore);
#elif defined (__LINUX__)
	semctl(m_semaphore, 0, IPC_RMID);
#endif
}

// --------------------------------------------------------------------------------
//  Method:
//      CSemaphore::Wait
//
//  Description:
//      Wait for semaphore to reach 0
// --------------------------------------------------------------------------------
inline void CSemaphore::Wait()
{
#if defined (__WIN32__)	
	WaitForSingleObject(m_semaphore, INFINITE);
#elif defined (__LINUX__)
	m_sembuf.sem_num = 0;
	m_sembuf.sem_op	 = 0;
	m_sembuf.sem_flg = 0;	// BLOCK
	
	semop(m_semaphore, &m_sembuf, 1);
#endif
}
	
// --------------------------------------------------------------------------------
//  Method:
//      CSemaphore::Post
//
//  Description:
//      Increases semaphore count by 1
// --------------------------------------------------------------------------------
inline void CSemaphore::Post()
{
#if defined (__WIN32__)
	ReleaseSemaphore(m_semaphore, 1, NULL);
#elif defined (__LINUX__)
	m_sembuf.sem_num = 0;
	m_sembuf.sem_op	 = 1;
	m_sembuf.sem_flg = 0;	// BLOCK
	
	semop(m_semaphore, &m_sembuf, 1);
#endif
}
	
// --------------------------------------------------------------------------------
//  Method:
//      CSemaphore::Post
//
//  Description:
//      Increas semaphore count by n
//
//	Inputs:
//		iCount == IN: Increase semaphore count by iCount
// --------------------------------------------------------------------------------
inline void CSemaphore::Post(int iCount)
{
#if defined (__WIN32__)
	ReleaseSemaphore(m_semaphore, iCount, NULL);
#elif defined (__LINUX__)
	m_sembuf.sem_num = 0;
	m_sembuf.sem_op	 = iCount;
	m_sembuf.sem_flg = 0;	// BLOCK
	
	semop(m_semaphore, &m_sembuf, 1);
#endif
}

