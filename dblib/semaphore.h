// ================================================================================
//
//	File:
//      semaphore.h
//
//	Component:
//      Photon System
//
//	Description:
//      Semaphore object
//
//	Author:
//		andrewc
// --------------------------------------------------------------------------------
//  Copyright (c) 2001-2004 Andrew Carter
//  All rights reserved
// ================================================================================

#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

// ================================================================================
//  Class:
//      CSemaphore
//
//  Description:
//      Semaphore class
// ================================================================================
class CSemaphore 
{
public:
	CSemaphore();
	CSemaphore(int iCount);
	~CSemaphore();

	void Wait();
	void Post();
	void Post(int iCount);

private:
#if defined (__WIN32__)
	HANDLE	m_semaphore;			// Win32 semaphore handle
#elif defined (__LINUX__)
	int		m_semaphore;			// Linux semaphore id
	sembuf	m_sembuf;				// Semaphore op buffer
#endif
};

#endif // __SEMAPHORE_H__


