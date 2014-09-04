namespace vfePlatform
{
#if !defined _WIN64 && POV_RAY_IS_OFFICIAL == 1
  // the following function is defined in code which cannot be re-licensed under the AGPL.
  // hence it is not provided with the AGPL POV-Ray source distribution. if you are getting
  // a compile error in this file you are compiling with POV_RAY_IS_OFFICIAL set to 1 (which
  // should never be the case for any build other than internal ones made by the POV team).
  #include "../../../legacy-license/misc/cpuinfo.cpp"
#else
  // GetCPUCount should provide the total available logical CPU's, total available CPU cores, and
  // total physical CPU's in the system. if this information is not available or is not accurate, it
  // should return false. knowing the number of logical cores vs physical cores is important for
  // correct reporting of statistics. the only time logical cores should be different from physical
  // ones (at least at the time of writing) is when the cores implement hyperthreading.
  //
  // If implementing this, please be wary of using GetLogicalProcessorInformation() or similar Win32
  // API calls, unless you are willing to either use GetProcAddress() or some other portable method
  // to access the API, or you are willing to drop support for pre-SP3 versions of Windows XP (32-bit).
  bool GetCPUCount(unsigned int *TotAvailLogical, unsigned int *TotAvailCore, unsigned int *PhysicalNum)
  {
    return false;
  }
#endif
}
