#if defined (_WIN32) || defined (_WIN64) || defined (WIN32) || defined (WIN64)
#ifndef POVRAY_BACKEND_PROCESSORGROUPS_H
#define POVRAY_BACKEND_PROCESSORGROUPS_H

// This module enables support for processor groups in Windows 8 and above.
// The implementation only requires a thread index be passed into SetThreadAffinity
// which will then be used to select an appropriate logical processor on a system with
// one or more processor groups.
//
// The module is meant to support earlier versions of Windows by dynamically loading appropriate
// dlls and entry points at runtime if they exist.  If support for processor groups is not present
// (e.g. Windows 7) then the older SetThreadAffinityMask will be used.
namespace pov
{
	namespace processorGroups
	{
		// Initializes internal state (e.g. loading Kernel32.dll and getting required entry points).
		// This does not have to be called explicitly.  The utility functions below will also call Initialize
		// if it has not already been called.
		extern void Initialize();

		// Returns the total number of available processors.
		extern int GetNumberOfProcessors();

		// Sets the thread affinity for the calling thread given the specified index.
		// Here index can pertain to any linearly increasing value (e.g. thread index, task index).
		extern void SetThreadAffinity(int index);

		// Frees and deinitializes internal state.
		extern void Deinitialize();
	}
}

#endif
#endif