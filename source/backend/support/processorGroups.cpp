#include <mutex>
#include <Windows.h>
#include "processorGroups.h"

namespace pov
{
	namespace processorGroups
	{
		// used to dynamically load processor group utility functions from Kernel32.dll at runtime.
		typedef WORD(*PFN_GETACTIVEPROCESSORGROUPCOUNT)(void);
		typedef WORD(*PFN_GETACTIVEPROCESSORCOUNT)(WORD);
		typedef BOOL(*PFN_SETTHREADGROUPAFFINITY)(HANDLE, const GROUP_AFFINITY*, PGROUP_AFFINITY);
		static bool g_Initialized = false;
		static HMODULE g_Kernel32 = NULL;
		static PFN_GETACTIVEPROCESSORGROUPCOUNT g_GetActiveProcessorGroupCount = NULL;
		static PFN_GETACTIVEPROCESSORCOUNT g_GetActiveProcessorCount = NULL;
		static PFN_SETTHREADGROUPAFFINITY g_SetThreadGroupAffinity = NULL;

		void Initialize()
		{
			// Initialize internal state if it hasn't already been done.
			if (!g_Initialized)
			{
				// Prevent multiple threads from entering.
				static std::mutex mutex;
				std::lock_guard<std::mutex> lock(mutex);
				if (!g_Initialized)
				{
					// Default all pointers to null.
					g_Kernel32 = NULL;
					g_GetActiveProcessorGroupCount = NULL;
					g_GetActiveProcessorCount = NULL;
					g_SetThreadGroupAffinity = 0;

					// Load Kernel32.dll and retrieve required entry points.
					g_Kernel32 = LoadLibrary("Kernel32.dll");
					if (g_Kernel32 != NULL)
					{
						g_GetActiveProcessorGroupCount = (PFN_GETACTIVEPROCESSORGROUPCOUNT)GetProcAddress(g_Kernel32, "GetActiveProcessorGroupCount");
						g_GetActiveProcessorCount = (PFN_GETACTIVEPROCESSORCOUNT)GetProcAddress(g_Kernel32, "GetActiveProcessorCount");
						g_SetThreadGroupAffinity = (PFN_SETTHREADGROUPAFFINITY)GetProcAddress(g_Kernel32, "SetThreadGroupAffinity");
					}

					g_Initialized = true;
				}
			}
		}

		int GetNumberOfProcessors()
		{
			// Safety check.
			Initialize();

			// Check to see if processor group support is present, else default to old Windows 7 function.
			bool hasProcessorGroupsSupport = (g_Kernel32 && g_GetActiveProcessorGroupCount && g_GetActiveProcessorCount && g_SetThreadGroupAffinity);
			if (hasProcessorGroupsSupport) return g_GetActiveProcessorCount(ALL_PROCESSOR_GROUPS);
			else
			{
				SYSTEM_INFO sysInfo = {};
				GetSystemInfo(&sysInfo);
				return sysInfo.dwNumberOfProcessors;
			}
		}

		void SetThreadAffinity(int index)
		{
			// Skip setting thread affinity if incoming value was never set or isn't positive (Task class defaults taskIndex to -1).
			if (index == -1)
				return;

			// Safety check.
			Initialize();

			// Check to see if processor group support is present.
			bool hasProcessorGroupsSupport = (g_Kernel32 && g_GetActiveProcessorGroupCount && g_GetActiveProcessorCount && g_SetThreadGroupAffinity);
			if (hasProcessorGroupsSupport)
			{
				// Since thread index may continue to increase unbounded, wrap it back around within the range of available processors.
				int processorIndex = index % g_GetActiveProcessorCount(ALL_PROCESSOR_GROUPS);
				
				// Determine which processor group to bind the thread to.
				auto totalProcessors = 0U;
				for (auto i = 0U; i < g_GetActiveProcessorGroupCount(); ++i)
				{
					totalProcessors += g_GetActiveProcessorCount(i);
					if (totalProcessors >= processorIndex)
					{
						GROUP_AFFINITY groupAffinity = { ~0ULL, static_cast<WORD>(i), { 0, 0, 0 } };
						g_SetThreadGroupAffinity(GetCurrentThread(), &groupAffinity, nullptr);
						break;
					}
				}
			}
		}

		void Deinitialize()
		{
			if (g_Kernel32)
				FreeLibrary(g_Kernel32);
		}
	}
}