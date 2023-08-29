// ================================================================================
//
//	File:
//      err.h
//
//	Component:
//      PhotonSystem
//
//	Description:
//      Error handling functions
//
//	Author:
//		andrewc
// --------------------------------------------------------------------------------
//  Copyright (c) 2001-2004 Andrew Carter
//  All rights reserved
// ================================================================================

#ifndef __ERR_H__
#define __ERR_H__

class CTrace;

enum RESULT
{
	// GENERAL PURPOSE ERRORS
    SUCCESS = 0,
	FAIL,
    ERR_UNEXPECTED,
	ERR_OUTOFMEMORY,
	ERR_HANDLEINVALID,
	ERR_INPUTMISSING,

	// FILE HANDLING ERRORS
    ERR_NOTFOUND,
    ERR_NOTCREATED,
	ERR_NOTSAVED,
    ERR_EXISTS,
    ERR_OPEN,
    ERR_NOTOPEN,
	ERR_EOF,
	ERR_OVERFLOW
};

// --------------------------------------------------------------------------------
// Error messages
// --------------------------------------------------------------------------------

// RESULT CODE STRINGS
const char RS_ERR_SUCCESS[]			= "SUCCESS";
const char RS_ERR_FAIL[]			= "FAIL";
const char RS_ERR_UNEXPECTED[]		= "ERR_UNEXPECTED";
const char RS_ERR_OUTOFMEMORY[]		= "ERR_OUTOFMEMORY";
const char RS_ERR_HANDLEINVALID[]	= "ERR_HANDLEINVALID";
const char RS_ERR_INPUTMISSING[]	= "ERR_INPUTMISSING";
const char RS_ERR_NOTFOUND[]		= "ERR_NOTFOUND";
const char RS_ERR_NOTCREATED[]		= "ERR_NOTCREATED";
const char RS_ERR_NOTSAVED[]		= "ERR_NOTSAVED";
const char RS_ERR_EXISTS[]			= "ERR_EXISTS";
const char RS_ERR_OPEN[]			= "ERR_OPEN";
const char RS_ERR_NOTOPEN[]			= "ERR_NOTOPEN";
const char RS_ERR_EOF[]				= "ERR_EOF";
const char RS_ERR_OVERFLOW[]		= "ERR_OVERFLOW";

// LONG VERSION OF ERROR MESSAGES
const char RS_ERRMSG_SUCCESS[]		= "Operation succeeded";
const char RS_ERRMSG_FAIL[]			= "Opeartion failed";
const char RS_ERRMSG_UNEXPECTED[]	= "Unexpected error occurred";
const char RS_ERRMSG_OUTOFMEMORY[]	= "Insufficient memory available";
const char RS_ERRMSG_HANDLEINVALID[]= "Handle invalid";
const char RS_ERRMSG_INPUTMISSING[]	= "Input missing";
const char RS_ERRMSG_NOTFOUND[]		= "Item not found";
const char RS_ERRMSG_NOTCREATED[]	= "Item not created";
const char RS_ERRMSG_NOTSAVED[]		= "Item not saved";
const char RS_ERRMSG_EXISTS[]		= "Item exists";
const char RS_ERRMSG_OPEN[]			= "Item open";
const char RS_ERRMSG_NOTOPEN[]		= "Item not open";
const char RS_ERRMSG_EOF[]			= "End of file reached";
const char RS_ERRMSG_OVERFLOW[]		= "Overflow occurred.";


// --------------------------------------------------------------------------------
// Error checking macros
// --------------------------------------------------------------------------------
#define RESULT_FAILED(result)		(result >  SUCCESS)
#define RESULT_SUCCEEDED(result)	(result == SUCCESS)

#define CHECK_RESULT(res)		\
	result = res;				\
	if(RESULT_FAILED(result))	\
	{							\
		TRACE_ERR(result);		\
		TRACE_SYSTEM_ERROR();	\
		goto CLEANUP;			\
	}

// --------------------------------------------------------------------------------
// Error string arrays
// --------------------------------------------------------------------------------
extern const WCHAR* RESULT_ERRORS[];
extern const WCHAR* RESULT_ERRMSGS[];

// --------------------------------------------------------------------------------
// Error handler functions
// --------------------------------------------------------------------------------

void TraceError(CTrace* pTrace, RESULT result);
void GetSystemError(DWORD dwErr, char* pszErrMsg, UINT cbLen);

#endif // __ERR_H__


