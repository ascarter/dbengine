// ================================================================================
//
//	File:
//      object.cpp
//
//	Component:
//      Photon System
//
//	Description:
//      Base object implementation
//
//	Author:
//		andrewc
// --------------------------------------------------------------------------------
//  Copyright (c) 2001-2004 Andrew Carter
//  All rights reserved
// ================================================================================

#include "sys.h"
#include "object.h"

// --------------------------------------------------------------------------------
//  Method:
//      CObject::CObject
//
//  Description:
//      Constructor that accepts function name
//
//  Inputs:
//      pszFunction == Function name
// --------------------------------------------------------------------------------
CObject::CObject()
{
    m_cRef = 0;
}

// --------------------------------------------------------------------------------
//	Method:
//		CObject::~CObject
//
//	Description:
//		Default destructor
// --------------------------------------------------------------------------------
CObject::~CObject()
{
}

// --------------------------------------------------------------------------------
//  Method:
//      CObject::Increment
//
//  Description:
//      Increment ref count
// --------------------------------------------------------------------------------
void CObject::Increment()
{
	m_cRef++;
}

// --------------------------------------------------------------------------------
//  Method:
//      CObject::Decrement
//
//  Description:
//      Decrement ref count
// --------------------------------------------------------------------------------
void CObject::Decrement()
{
	m_cRef--;

	if (m_cRef == 0)
	{
 		delete this;
	}
}

// --------------------------------------------------------------------------------
//  Method:
//      CObject::GetReferences
//
//  Description:
//      Get object's reference count
//
//	Retuns:
//		Object ref count
// --------------------------------------------------------------------------------
UINT CObject::GetReferences()
{
	return m_cRef;
}


