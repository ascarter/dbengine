// ================================================================================
//
//	File:
//      event.h
//
//	Component:
//      Photon System
//
//	Description:
//      Event object
//
//	Author:
//		andrewc
// --------------------------------------------------------------------------------
//  Copyright (c) 2001-2004 Andrew Carter
//  All rights reserved
// ================================================================================

#ifndef __EVENT_H__
#define __EVENT_H__

// ================================================================================
//  Class:
//      CEvent
//
//  Description:
//      Event class.  Can be customized per platform
// ================================================================================
class CEvent
{
public:
	CEvent();
	~CEvent();

	void Set();
	void Reset();
	void Signal();
	void Wait();

private:
#if defined (__WIN32__)
	HANDLE	m_hEvent;
#elif defined (__LINUX__)

#endif
};

#endif // __EVENT_H__

