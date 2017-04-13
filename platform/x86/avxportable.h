#ifndef POVRAY_AVXPORTABLENOISE_H
#define POVRAY_AVXPORTABLENOISE_H

#include "syspovconfigbase.h"
#include "backend/frame.h"
#include "cpuid.h"

#ifdef TRY_OPTIMIZED_NOISE_AVX_PORTABLE

namespace pov
{

static bool AVXPORTABLENoiseSupported() { return HaveAVX(); }

DBL AVXPortableNoise(const Vector3d& EPoint, int noise_generator);

void AVXPortableDNoise(Vector3d& result, const Vector3d& EPoint);

}

#endif // TRY_OPTIMIZED_NOISE_AVX_PORTABLE

#endif // POVRAY_AVXPORTABLENOISE_H
