// ================================================================================
//
//	File:
//      thread.h
//
//	Component:
//      System
//
//	Description:
//      Thread object interface
//
//	Author:
//		andrewc
// --------------------------------------------------------------------------------
//  Copyright (c) 2001-2004 Andrew Carter
//  All rights reserved
// ================================================================================

#ifndef __THREAD_H__
#define __THREAD_H__

// Thread types
#if defined (__WIN32__)
typedef HANDLE					THREAD_ID;			// WIN32 thread handle
typedef DWORD					THREAD_STACK;		// Type used to specify a stack size
typedef DWORD					THREAD_EXIT;		// Type used to hold a thread's exit code
typedef DWORD					THREAD_KEY;			// Key type for thread-specific values
typedef LPSECURITY_ATTRIBUTES	THREAD_ATTRIBUTE;	// Security attributes
#elif defined (__LINUX__)
typedef pthread_t				THREAD_ID;			// Type used to identify a thread
typedef unsigned				THREAD_STACK;		// Type used to specify a stack size
typedef int						THREAD_EXIT;		// Type used to hold a thread's exit code
typedef pthread_key_t			THREAD_KEY;			// Key type for thread-specific values
typedef pthread_attr_t			THREAD_ATTRIBUTE;	// POSIX thread attribute type
#endif

// Thread function pointers
#if defined (__WIN32__)
typedef unsigned (__stdcall *PTHREAD_FUNCTION)(void *arg);	// WIN32 thread function
#elif defined (__LINUX__)
typedef void *(*PTHREAD_FUNCTION)(void* arg);				// POSIX thread function
#endif

// --------------------------------------------------------------------------------
//	Enumerations
// --------------------------------------------------------------------------------

//	Current thread state
enum THREAD_STATE
{ 
  THREAD_STATE_INVALID = 0,	// The current state of this thread is unknown
  THREAD_STATE_CANCELED,	// Thread has been canceled
  THREAD_STATE_EXITED,		// Thread has exited
  THREAD_STATE_NEW,			// Newly created thread
  THREAD_STATE_RUNNING,		// Thread is currently running
  THREAD_STATE_SUSPENDED,	// Thread has been suspended
  THREAD_STATE_WAITING		// Execution is blocked waiting for a signal 
};

//	Thread priority settings
enum THREAD_PRIORITY
{
  THREAD_PRIORITY_INVALID = 0,	// The priority of this thread is invalid
  THREAD_PRIORITY_LOW,			// Move to the bottom of the priority chain
  THREAD_PRIORITY_DEFAULT,		// Add to the priority chain (default)
  THREAD_PRIORITY_HIGH			// Move to the top of the priority chain
};

//	Thread scheduling policies (POSIX)
//	NOTE: The WIN32 scheduling policy is determined by the priority class of
//	its process. None of these policies have any effect under WIN32. 
enum THREAD_PRIORITY_CLASS
{
  THREAD_PRIORITY_CLASS_INVALID = 0,	// The priority class is invalid
  THREAD_PRIORITY_CLASS_OTHER,			// Another scheduling policy (default policy)
  THREAD_PRIORITY_CLASS_FIFO,			// First in-first out (FIFO) scheduling policy
  THREAD_PRIORITY_CLASS_RR				// Round robin scheduling policy
};

//	Thread error codes
enum THREAD_ERROR
{
  THREAD_NO_ERROR = 0,		// No errors reported
  THREAD_INVALID_CODE,		// Invalid thread error code
  
  THREAD_CANCEL_ERROR,		// Error canceling thread
  THREAD_CLOSE_ERROR,		// Error closing thread
  THREAD_CREATE_ERROR,		// Error creating thread
  THREAD_RUNNING_ERROR,		// Thread is already running
  THREAD_EXECUTE_ERROR,		// Thread cannot be executed 
  THREAD_POLICY_ERROR,		// Cannot retrieve thread scheduling policy
  THREAD_PRIORITY_ERROR,	// Error setting the thread priority
  THREAD_RESUME_ERROR,		// Error resuming thread
  THREAD_SCHED_ERROR,		// Scheduling error
  THREAD_SCOPE_ERROR,		// Contention scope error
  THREAD_STACK_ERROR,		// Error setting the stack size
  THREAD_STATE_ERROR,		// Could not set the thread state
  THREAD_SUSPEND_ERROR		// Error suspending thread 
};

//	Thread types
enum THREAD_TYPE
{ 
  THREAD_TYPE_DETACHED,		// System reclaims resources when thread terminates
  THREAD_TYPE_JOINABLE
};

//	Process types
enum PROCESS_TYPE
{
  PROCESS_PRIVATE = 0,	// Resources are available only to a single process
  PROCESS_SHARED  = 1	// Resources are available to multiple processes
};

// ================================================================================
// Class:
//      CThread
//
//  Description:
//      Thread object.  Wraps up platform specific thread object.
// ================================================================================
class CThread : public CObject
{
public:
	CThread();
	~CThread();

	void Init(
		PTHREAD_FUNCTION	pFunction,
		void*				pArg,
		bool				fSuspended	= false,
		THREAD_STACK		stack		= 0
		);

	void Suspend();
	void Resume();
	void Close();
	void Destroy();
	void Cancel();
	void GetState();

private:
	THREAD_ID		m_ThreadID;
	UINT			m_ThreadAddr;
	THREAD_STATE	m_ThreadState;
};

#endif // __THREAD_H__


