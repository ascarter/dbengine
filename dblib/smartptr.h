// ================================================================================
//
//	File:
//      smartptr.h
//
//	Component:
//      Photon System
//
//	Description:
//      Smart pointer definition
//
//	Author:
//		andrewc
// --------------------------------------------------------------------------------
//  Copyright (c) 2001-2004 Andrew Carter
//  All rights reserved
// ================================================================================

#ifndef __SMARTPTR_H__
#define __SMARTPTR_H__

// --------------------------------------------------------------------------------
//	Macros
// --------------------------------------------------------------------------------
#define SmartPointer(classname)					\
	class classname;							\
	typedef CPointer<classname> classname##Ptr

// cast a smart pointer of one type to a pointer of another type.
#define SmartPointerCast(type, smartptr)	((type*)(void*)(smartptr))

// ================================================================================
//  Template:
//      CPointer
//
//  Description:
//      Smart pointer for CObject derived classes.
// ================================================================================
template <class T>
	class CPointer
{
public:
    // ----------------------------------------------------------------------------
    // CONSTRUCTION
    // ----------------------------------------------------------------------------

	CPointer (T* pObject = 0)
	{
		m_pObject = pObject;
		if (m_pObject)
			m_pObject->Increment();
	}

	CPointer (const CPointer& rPointer)
	{
		m_pObject = rPointer.m_pObject;
		if (m_pObject)
			m_pObject->Increment();
	}

	~CPointer()
	{
		if (m_pObject)
			m_pObject->Decrement();
	}

    // ----------------------------------------------------------------------------
    // IMPLICIT CONVERSIONS
    // ----------------------------------------------------------------------------

	operator T* () const		{ return  m_pObject; }
	T& operator* () const		{ return *m_pObject; }
	T* operator-> () const		{ return  m_pObject; }

	// ----------------------------------------------------------------------------
    // ASSIGNMENT
    // ----------------------------------------------------------------------------

	CPointer& operator= (const CPointer& rPointer)
	{
		if (m_pObject != rPointer.m_pObject)
		{
			if (m_pObject)
				m_pObject->Decrement();
			m_pObject = rPointer.m_pObject;
			if (m_pObject)
				m_pObject->Increment();
		}
		return *this;
	}

	CPointer& operator= (T* pObject)
	{
		if (m_pObject != pObject)
		{
			if (m_pObject)
				m_pObject->Decrement();
			m_pObject = pObject;
			if (m_pObject)
				m_pObject->Increment();
		}
		return *this;
	}

	// ----------------------------------------------------------------------------
    // COMPARISONS
    // ----------------------------------------------------------------------------

	bool operator== (T* pObject) const		{ return m_pObject == pObject; }
	bool operator!= (T* pObject) const		{ return m_pObject != pObject; }

	bool operator== (const CPointer& rPointer) const
	{
		return m_pObject == rPointer.m_pObject;
	}

	bool operator!= (const CPointer& rPointer) const
	{
		return m_pObject != rPointer.m_pObject;
	}

private:
	T* m_pObject;
};


#endif // __SMARTPTR_H__


