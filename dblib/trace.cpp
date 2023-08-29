// ================================================================================
//
//	File:
//      trace.cpp
//
//	Component:
//      Photon System
//
//	Description:
//      System tracing object implementation
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
//      CTrace::CTrace
//
//  Description:
//      Constructor that accepts function name
//
//  Inputs:
//      strFunction == IN: Function name
// --------------------------------------------------------------------------------
CTrace::CTrace
(
	string strFunction
)
{
	m_strFunction = strFunction;

    TRACE_ENTER();
}

// --------------------------------------------------------------------------------
//	Method:
//		CTrace::~CTrace
//
//	Description:
//		Default destructor
// --------------------------------------------------------------------------------
CTrace::~CTrace()
{
    TRACE_LEAVE();
}

// --------------------------------------------------------------------------------
//	Method:
//		CTrace::Print
//
//	Description:
//		Output string to debug console
//
//	Inputs:
//		strMsg == IN: Debug message to output
// --------------------------------------------------------------------------------
void CTrace::Print
(
	const string& strMsg
)
{
	ostrstream strstOutput(0, TRACE_BUFFER_SIZE);
	strstOutput << "\t" << m_strFunction << "\t" << strMsg << endl << ends;
	PrintDebugString(strstOutput.str());
}

// --------------------------------------------------------------------------------
//	Method:
//		CTrace::Log
//
//	Description:
//		Initialization routine.
//
//	Inputs:
//		strMsg == Message to log
// --------------------------------------------------------------------------------
void CTrace::Log
(
	const string& strMsg
)
{
    // TO DO: Implement logging to file
	clog << strMsg << endl;
}

// --------------------------------------------------------------------------------
//  Method:
//      CTrace::Enter
//
//  Description:
//      Traces entry into current function
// --------------------------------------------------------------------------------
void CTrace::Enter()
{
	ostrstream strstOutput(0, TRACE_BUFFER_SIZE);
	strstOutput << "Entering:\t" << m_strFunction << endl << ends;
	PrintDebugString(strstOutput.str());
}

// --------------------------------------------------------------------------------
//  Method:
//      CTrace::Leave
//
//  Description:
//      Traces entry into current function
// --------------------------------------------------------------------------------
void CTrace::Leave()
{
	ostrstream strstOutput(0, TRACE_BUFFER_SIZE);
	strstOutput << "Leaving:\t" << m_strFunction << endl << ends;
	PrintDebugString(strstOutput.str());
}

// --------------------------------------------------------------------------------
//  Method:
//      CTrace::PrintSystemError
//
//  Description:
//      Traces system error
// --------------------------------------------------------------------------------
void CTrace::PrintSystemError
(
	DWORD dwErr
)
{
	char szErrMsg[MAX_ERROR];
	GetSystemError(dwErr, szErrMsg, MAX_ERROR);

	ostrstream strstOutput(0, TRACE_BUFFER_SIZE);
	strstOutput << "\t" << m_strFunction << "\tSYSTEM ERROR: ";
	strstOutput << szErrMsg << endl << ends;
	PrintDebugString(strstOutput.str());
}


