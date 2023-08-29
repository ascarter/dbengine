// ================================================================================
//
//	File:
//      sys.h
//
//	Component:
//      PhotonSystem
//
//	Description:
//      Common system header file
//
//	Author:
//		andrewc
// --------------------------------------------------------------------------------
//  Copyright (c) 2001-2004 Andrew Carter
//  All rights reserved
// ================================================================================

// --------------------------------------------------------------------------------
// OS HEADERS
// --------------------------------------------------------------------------------
#if defined (__WIN32__)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <crtdbg.h>
#include <process.h>
#elif defined (__LINUX__)
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <pthread.h>
#include <sched.h>
#endif

// --------------------------------------------------------------------------------
// STANDARD LIBRARY HEADERS
// --------------------------------------------------------------------------------
#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <memory>
#include <string>
#include <cwchar>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <strstream>
#include <time.h>

using namespace std;

// --------------------------------------------------------------------------------
// SYSTEM HEADERS
// --------------------------------------------------------------------------------
#include "types.h"
#include "mutex.h"
#include "semaphore.h"
#include "event.h"
#include "gate.h"
#include "rwlock.h"
#include "smartptr.h"
#include "object.h"
#include "file.h"
#include "err.h"
#include "trace.h"
#include "thread.h"

// --------------------------------------------------------------------------------
// MACROS
// --------------------------------------------------------------------------------

#if defined (__LINUX__)
#define TEXT(text)	L##text
#define _ASSERTE	assert
#endif

// --------------------------------------------------------------------------------
// CONSTANTS
// --------------------------------------------------------------------------------

#if defined (__LINUX__)
#define MAX_PATH 260
#endif

#ifndef __SYS_H__
#define __SYS_H__

#endif // __SYS_H__


