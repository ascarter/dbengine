// ================================================================================
//
//	File:
//      db.h
//
//	Component:
//      Database Engine
//
//	Description:
//		Common database header files
//
// --------------------------------------------------------------------------------
//  Copyright (c) 2001-2004 Andrew Carter
//  All rights reserved
// ================================================================================

// --------------------------------------------------------------------------------
// OS HEADERS
// --------------------------------------------------------------------------------
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <crtdbg.h>
#endif

// --------------------------------------------------------------------------------
// STANDARD LIBRARY HEADERS
// --------------------------------------------------------------------------------
#include <vector>
#include <queue>

// --------------------------------------------------------------------------------
// INTERNAL SYSTEM HEADERS
// --------------------------------------------------------------------------------
#include "sys.h"

// --------------------------------------------------------------------------------
// DATABASE HEADERS
// --------------------------------------------------------------------------------
#include "version.h"
#include "dbstruct.h"
#include "dbfile.h"
#include "dbtable.h"

