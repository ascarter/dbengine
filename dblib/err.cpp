// ================================================================================
//
//	File:
//		err.cpp
//
//	Component:
//		PhotonSystem
//
//	Description:
//		System error handling functions (implementation)
//
//	Author:
//		andrewc
// --------------------------------------------------------------------------------
//  Copyright (c) 2001-2004 Andrew Carter
//  All rights reserved
// ================================================================================

#include "sys.h"

// --------------------------------------------------------------------------------
// Result message arrays
// --------------------------------------------------------------------------------

// RESULT CODE ARRAY
const char* RS_ERRORS[] =	{
							RS_ERR_SUCCESS,
							RS_ERR_FAIL,
							RS_ERR_UNEXPECTED,
							RS_ERR_OUTOFMEMORY,
							RS_ERR_HANDLEINVALID,
							RS_ERR_INPUTMISSING,
							RS_ERR_NOTFOUND,
							RS_ERR_NOTCREATED,
							RS_ERR_NOTSAVED,
							RS_ERR_EXISTS,
							RS_ERR_OPEN,
							RS_ERR_NOTOPEN,
							RS_ERR_EOF,
							RS_ERR_OVERFLOW
							};

// LONG RESULT CODE ARRAY
const char* RS_ERRMSGS[] =	{
							RS_ERRMSG_SUCCESS,
							RS_ERRMSG_FAIL,
							RS_ERRMSG_UNEXPECTED,
							RS_ERRMSG_OUTOFMEMORY,
							RS_ERRMSG_HANDLEINVALID,
							RS_ERRMSG_INPUTMISSING,
							RS_ERRMSG_NOTFOUND,
							RS_ERRMSG_NOTCREATED,
							RS_ERRMSG_NOTSAVED,
							RS_ERRMSG_EXISTS,
							RS_ERRMSG_OPEN,
							RS_ERRMSG_NOTOPEN,
							RS_ERRMSG_EOF,
							RS_ERRMSG_OVERFLOW
							};

// --------------------------------------------------------------------------------
//  Function:
//      TraceError
//
//  Description:
//      Outputs error information to debug console
//
//  Inputs:
//      result == IN: error result
// --------------------------------------------------------------------------------
void TraceError
(
	CTrace*	pTrace,
	RESULT	result
)
{
	strstream strMsg;

	strMsg << RS_ERRORS[result] << ": " << RS_ERRMSGS[result] << endl;
	pTrace->Print(strMsg.str());
}

// --------------------------------------------------------------------------------
//	Function:
//		GetSystemError
//
//	Description:
//		Get system error message from error value
//
//	Inputs:
//		dwErr		== IN:	Error value
//		pszErrMsg	== OUT: Error message buffer
//		cbLen		== IN:	Error message buffer length
// --------------------------------------------------------------------------------
void GetSystemError
(
	DWORD	dwErr,
	char*	pszErrMsg,
	UINT	cbLen
)
{
#ifdef WIN32
    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM,
                  NULL,
                  dwErr,
                  0,
                  pszErrMsg,
                  cbLen,
                  NULL);
#elif LINUX
	strcpy(pszErrMsg, "SYSTEM ERROR\n");
#endif
}

