// ================================================================================
//
//	File:
//      types.h
//
//	Component:
//      Photon System
//
//	Description:
//      Portable data type definitions
//
//	Author:
//		andrewc
// --------------------------------------------------------------------------------
//  Copyright (c) 2001-2004 Andrew Carter
//  All rights reserved
// ================================================================================

#ifndef __TYPES_H__
#define __TYPES_H__

#if defined (__LINUX__)
typedef int				BOOL;
typedef int32_t			LONG;
typedef uint32_t		DWORD;
typedef uint32_t		ULONG;
typedef unsigned char	BYTE;
typedef uint16_t		WORD;
typedef float			FLOAT;
typedef int32_t			INT;
typedef uint32_t		UINT;
typedef wchar_t			WCHAR;
typedef void*			HANDLE;
#endif

struct version
{
	UINT	Major;
	UINT	Minor;
	UINT	Build;
};

#endif // __TYPES_H__


