// ================================================================================
//
//	File:
//      gate.h
//
//	Component:
//      Photon System
//
//	Description:
//      Gate object
//
//	Author:
//		andrewc
// --------------------------------------------------------------------------------
//  Copyright (c) 2001-2004 Andrew Carter
//  All rights reserved
// ================================================================================

#ifndef __GATE_H__
#define __GATE_H__

#include "event.h"

// ================================================================================
//  Class:
//      CGate
//
//  Description:
//      Gate object.  Stops all threads from proceeding or allows all to continue.
// ================================================================================
class CGate
{
public:
	CGate(void)			{ }
	~CGate(void)		{ }

	void Open(void)		{ m_evtGate.Set(); }
	void Close(void)	{ m_evtGate.Reset(); }

	// Temporarily open the gate and allow waiting threads to proceed
	void Release(void)	{ m_evtGate.Signal(); }
	void Wait(void)		{ m_evtGate.Wait(); }

private:
	CEvent	m_evtGate;		// Gate event
};

#endif // __GATE_H__


