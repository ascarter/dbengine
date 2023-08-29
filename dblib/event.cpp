// ================================================================================
//
//	File:
//      event.cpp
//
//	Component:
//      Photon System
//
//	Description:
//      Event object implementation
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
//      CEvent::CEvent
//
//  Description:
//      Constructor
// --------------------------------------------------------------------------------
CEvent::CEvent()
{
#if defined (__WIN32__)
	m_hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
#elif defined (__LINUX__)

#endif
}

// --------------------------------------------------------------------------------
//  Method:
//      CEvent::~CEvent
//
//  Description:
//      Default destructor.  Closes open event handles.
// --------------------------------------------------------------------------------
CEvent::~CEvent()
{
#if defined (__WIN32__)
	CloseHandle(m_hEvent);
#elif defined (__LINUX__)

#endif
}

// --------------------------------------------------------------------------------
//  Method:
//      CEvent::Set
//
//  Description:
//      Sets event to signaled state
//
// --------------------------------------------------------------------------------
void CEvent::Set()
{
#if defined (__WIN32__)
	SetEvent(m_hEvent);
#elif defined (__LINUX__)

#endif
}
	
// --------------------------------------------------------------------------------
//  Method:
//      CEvent::Reset
//
//  Description:
//      Reset event to unsignaled state
// --------------------------------------------------------------------------------
void CEvent::Reset()
{
#if defined (__WIN32__)
	ResetEvent(m_hEvent);
#elif defined (__LINUX__)

#endif
}

// --------------------------------------------------------------------------------
//  Method:
//      CEvent::Signal
//
//  Description:
//      Set event to signaled state and then reset to unsignaled.
// --------------------------------------------------------------------------------
void CEvent::Signal()
{
#if defined (__WIN32__)
	PulseEvent(m_hEvent);
#elif defined (__LINUX__)

#endif
}

// --------------------------------------------------------------------------------
//  Method:
//      CEvent::Wait
//
//  Description:
//      Wait for event to be signaled
// --------------------------------------------------------------------------------
void CEvent::Wait()
{
#if defined (__WIN32__)
	WaitForSingleObject(m_hEvent, INFINITE);
#elif defined (__LINUX__)

#endif
}

