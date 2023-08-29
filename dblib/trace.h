// ================================================================================
//
//	File:
//      trace.h
//
//	Component:
//      Photon System
//
//	Description:
//      Tracing object
//
//	Author:
//		andrewc
// --------------------------------------------------------------------------------
//  Copyright (c) 2001-2004 Andrew Carter
//  All rights reserved
// ================================================================================

#ifndef __TRACE_H__
#define __TRACE_H__

#include "err.h"

// --------------------------------------------------------------------------------
// Buffer lengths
// --------------------------------------------------------------------------------
const int MAX_ERROR			= 1024;
const int TRACE_BUFFER_SIZE	= 4096;

// --------------------------------------------------------------------------------
// MACROS
// --------------------------------------------------------------------------------
#ifdef _DEBUG
#define TRACE_INIT(func)        CTrace trace(func)
#define TRACE(msg)              trace.Print(msg)
#define TRACE_DEBUG_PRINT(msg)	trace.Print(msg)
#define TRACE_ENTER()		    Enter()
#define TRACE_LEAVE()			Leave()
#define TRACE_SYSTEM_ERROR()	trace.PrintSystemError(GetLastError())
#define TRACE_ERR(result)		TraceError(&trace, result)
#else
#define TRACE_INIT(func)        /* nothing */
#define TRACE(msg)              /* nothing */
#define TRACE_DEBUG_PRINT(msg)  /* nothing */
#define TRACE_ENTER()			/* nothing */
#define TRACE_LEAVE()			/* nothing */
#define TRACE_SYSTEM_ERROR()	/* nothing */
#define TRACE_ERR(result)		/* nothing */
#endif

#if defined (__WIN32__)
#define PrintDebugString(msg)	OutputDebugStringA(msg)
#else
#define PrintDebugString(msg)	clog << msg
#endif

// ================================================================================
//  Class:
//      CTrace
//
//  Description:
//      System tracing object
// ================================================================================
class CTrace
{
public:
    CTrace(string strFunction);
    // CTrace(WCHAR*	pwszFunction);
    ~CTrace();

    void Enter();
	void Leave();
    void Print(const string& strMessage);
    void Log  (const string& strMessage);
	void PrintSystemError(DWORD dwErr);

private:
	string		m_strFunction;
};

#endif // __TRACE_H__


