// ================================================================================
//
//	File:
//      object.h
//
//	Component:
//      Photon System
//
//	Description:
//      Base object in the Photon System.  Supports smart pointers, reference
//		counting, run-time type information, and tracing.
//
//	Author:
//		andrewc
// --------------------------------------------------------------------------------
//  Copyright (c) 2001-2004 Andrew Carter
//  All rights reserved
// ================================================================================

#ifndef __OBJECT_H__
#define __OBJECT_H__

SmartPointer(CObject);

// ================================================================================
//  Class:
//      CObject
//
//  Description:
//      Base system object.  Provides ref counting, smart pointers, rtti, and
//		tracing
// ================================================================================
class CObject
{
public:
    CObject();
    virtual ~CObject();

	void Increment();
	void Decrement();
	UINT GetReferences();

private:
	UINT		m_cRef;			// Object reference count
};

#endif // __OBJECT_H__


