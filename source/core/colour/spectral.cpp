//******************************************************************************
///
/// @file core/colour/spectral.cpp
///
/// Implementations related to colour spectra.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2016 Persistence of Vision Raytracer Pty. Ltd.
///
/// POV-Ray is free software: you can redistribute it and/or modify
/// it under the terms of the GNU Affero General Public License as
/// published by the Free Software Foundation, either version 3 of the
/// License, or (at your option) any later version.
///
/// POV-Ray is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU Affero General Public License for more details.
///
/// You should have received a copy of the GNU Affero General Public License
/// along with this program.  If not, see <http://www.gnu.org/licenses/>.
///
/// ----------------------------------------------------------------------------
///
/// POV-Ray is based on the popular DKB raytracer version 2.12.
/// DKBTrace was originally written by David K. Buck.
/// DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
///
/// @endparblock
///
//******************************************************************************

// Unit header file must be the first file included within POV-Ray *.cpp files (pulls in config)
#include "core/colour/spectral.h"

#include "base/colour.h"
#include "base/mathutil.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

#if ((POV_COLOUR_MODEL == 0) || (POV_COLOUR_MODEL == 3))

// Table of RGB colors by wavelength
// (integrated over wavelength)

#if 0
// Variant 1:
// Values based on the CIE XYZ tristimulus table, transformed to (linear) sRGB color space;
// color values gamut-adjusted by clipping
#define SPECTRAL_HUE_TABLE_SIZE 74
#define SPECTRAL_HUE_TABLE_BASE 375.0
#define SPECTRAL_HUE_TABLE_STEP 5.0
static RGBColour SpectralHueIntegral[SPECTRAL_HUE_TABLE_SIZE] = {
    /* 375 nm */ RGBColour(0.0000,0.0000,0.0000), // (expected to be {0.0,0} by algotithm)
    /* 380 nm */ RGBColour(0.0001,0.0000,0.0003),
    /* 385 nm */ RGBColour(0.0002,0.0000,0.0008),
    /* 390 nm */ RGBColour(0.0005,0.0000,0.0017),
    /* 395 nm */ RGBColour(0.0010,0.0000,0.0034),
    /* 400 nm */ RGBColour(0.0020,0.0000,0.0065),
    /* 405 nm */ RGBColour(0.0035,0.0000,0.0115),
    /* 410 nm */ RGBColour(0.0063,0.0000,0.0211),
    /* 415 nm */ RGBColour(0.0114,0.0000,0.0381),
    /* 420 nm */ RGBColour(0.0203,0.0000,0.0678),
    /* 425 nm */ RGBColour(0.0344,0.0000,0.1156),
    /* 430 nm */ RGBColour(0.0530,0.0000,0.1794),
    /* 435 nm */ RGBColour(0.0745,0.0000,0.2542),
    /* 440 nm */ RGBColour(0.0973,0.0000,0.3349),
    /* 445 nm */ RGBColour(0.1199,0.0000,0.4174),
    /* 450 nm */ RGBColour(0.1417,0.0000,0.4996),
    /* 455 nm */ RGBColour(0.1621,0.0000,0.5809),
    /* 460 nm */ RGBColour(0.1806,0.0000,0.6591),
    /* 465 nm */ RGBColour(0.1962,0.0000,0.7310),
    /* 470 nm */ RGBColour(0.2079,0.0000,0.7921),
    /* 475 nm */ RGBColour(0.2156,0.0000,0.8420),
    /* 480 nm */ RGBColour(0.2198,0.0000,0.8814),
    /* 485 nm */ RGBColour(0.2209,0.0060,0.9117),
    /* 490 nm */ RGBColour(0.2209,0.0203,0.9351),
    /* 495 nm */ RGBColour(0.2209,0.0431,0.9532),
    /* 500 nm */ RGBColour(0.2209,0.0748,0.9676),
    /* 505 nm */ RGBColour(0.2209,0.1166,0.9792),
    /* 510 nm */ RGBColour(0.2209,0.1689,0.9880),
    /* 515 nm */ RGBColour(0.2209,0.2315,0.9942),
    /* 520 nm */ RGBColour(0.2209,0.3025,0.9981),
    /* 525 nm */ RGBColour(0.2209,0.3788,1.0000),
    /* 530 nm */ RGBColour(0.2209,0.4577,1.0000),
    /* 535 nm */ RGBColour(0.2209,0.5372,1.0000),
    /* 540 nm */ RGBColour(0.2212,0.6153,1.0000),
    /* 545 nm */ RGBColour(0.2253,0.6901,1.0000),
    /* 550 nm */ RGBColour(0.2336,0.7600,1.0000),
    /* 555 nm */ RGBColour(0.2467,0.8234,1.0000),
    /* 560 nm */ RGBColour(0.2649,0.8789,1.0000),
    /* 565 nm */ RGBColour(0.2885,0.9251,1.0000),
    /* 570 nm */ RGBColour(0.3177,0.9609,1.0000),
    /* 575 nm */ RGBColour(0.3526,0.9854,1.0000),
    /* 580 nm */ RGBColour(0.3928,0.9984,1.0000),
    /* 585 nm */ RGBColour(0.4377,1.0000,1.0000),
    /* 590 nm */ RGBColour(0.4867,1.0000,1.0000),
    /* 595 nm */ RGBColour(0.5387,1.0000,1.0000),
    /* 600 nm */ RGBColour(0.5922,1.0000,1.0000),
    /* 605 nm */ RGBColour(0.6458,1.0000,1.0000),
    /* 610 nm */ RGBColour(0.6980,1.0000,1.0000),
    /* 615 nm */ RGBColour(0.7474,1.0000,1.0000),
    /* 620 nm */ RGBColour(0.7927,1.0000,1.0000),
    /* 625 nm */ RGBColour(0.8328,1.0000,1.0000),
    /* 630 nm */ RGBColour(0.8673,1.0000,1.0000),
    /* 635 nm */ RGBColour(0.8965,1.0000,1.0000),
    /* 640 nm */ RGBColour(0.9207,1.0000,1.0000),
    /* 645 nm */ RGBColour(0.9403,1.0000,1.0000),
    /* 650 nm */ RGBColour(0.9557,1.0000,1.0000),
    /* 655 nm */ RGBColour(0.9676,1.0000,1.0000),
    /* 660 nm */ RGBColour(0.9766,1.0000,1.0000),
    /* 665 nm */ RGBColour(0.9832,1.0000,1.0000),
    /* 670 nm */ RGBColour(0.9879,1.0000,1.0000),
    /* 675 nm */ RGBColour(0.9914,1.0000,1.0000),
    /* 680 nm */ RGBColour(0.9940,1.0000,1.0000),
    /* 685 nm */ RGBColour(0.9958,1.0000,1.0000),
    /* 690 nm */ RGBColour(0.9970,1.0000,1.0000),
    /* 695 nm */ RGBColour(0.9979,1.0000,1.0000),
    /* 700 nm */ RGBColour(0.9985,1.0000,1.0000),
    /* 705 nm */ RGBColour(0.9989,1.0000,1.0000),
    /* 710 nm */ RGBColour(0.9993,1.0000,1.0000),
    /* 715 nm */ RGBColour(0.9995,1.0000,1.0000),
    /* 720 nm */ RGBColour(0.9996,1.0000,1.0000),
    /* 725 nm */ RGBColour(0.9997,1.0000,1.0000),
    /* 730 nm */ RGBColour(0.9998,1.0000,1.0000),
    /* 735 nm */ RGBColour(0.9999,1.0000,1.0000),
    /* 740 nm */ RGBColour(1.0000,1.0000,1.0000)  // (expected to be {1,1.1} by algotithm)
};

#elif 0
// Variant 2:
// Values based on the CIE XYZ tristimulus table, transformed to (linear) sRGB color space;
// color values gamut-adjusted by adding white
#define SPECTRAL_HUE_TABLE_SIZE 73
#define SPECTRAL_HUE_TABLE_BASE 375.0
#define SPECTRAL_HUE_TABLE_STEP 5.0
static RGBColour SpectralHueIntegral[SPECTRAL_HUE_TABLE_SIZE] = {
    /* 375 nm */ RGBColour(0.0000,0.0000,0.0000), // (expected to be {0.0,0} by algotithm)
    /* 380 nm */ RGBColour(0.0001,0.0000,0.0003),
    /* 385 nm */ RGBColour(0.0004,0.0000,0.0008),
    /* 390 nm */ RGBColour(0.0008,0.0000,0.0017),
    /* 395 nm */ RGBColour(0.0016,0.0000,0.0034),
    /* 400 nm */ RGBColour(0.0031,0.0000,0.0065),
    /* 405 nm */ RGBColour(0.0056,0.0000,0.0115),
    /* 410 nm */ RGBColour(0.0102,0.0000,0.0210),
    /* 415 nm */ RGBColour(0.0184,0.0000,0.0380),
    /* 420 nm */ RGBColour(0.0325,0.0000,0.0675),
    /* 425 nm */ RGBColour(0.0552,0.0000,0.1148),
    /* 430 nm */ RGBColour(0.0851,0.0000,0.1777),
    /* 435 nm */ RGBColour(0.1196,0.0000,0.2510),
    /* 440 nm */ RGBColour(0.1561,0.0000,0.3294),
    /* 445 nm */ RGBColour(0.1923,0.0000,0.4087),
    /* 450 nm */ RGBColour(0.2270,0.0000,0.4866),
    /* 455 nm */ RGBColour(0.2595,0.0000,0.5620),
    /* 460 nm */ RGBColour(0.2887,0.0000,0.6328),
    /* 465 nm */ RGBColour(0.3130,0.0000,0.6958),
    /* 470 nm */ RGBColour(0.3305,0.0000,0.7464),
    /* 475 nm */ RGBColour(0.3408,0.0000,0.7841),
    /* 480 nm */ RGBColour(0.3445,0.0000,0.8097),
    /* 485 nm */ RGBColour(0.3450,0.0049,0.8279),
    /* 490 nm */ RGBColour(0.3450,0.0179,0.8428),
    /* 495 nm */ RGBColour(0.3450,0.0396,0.8558),
    /* 500 nm */ RGBColour(0.3450,0.0702,0.8677),
    /* 505 nm */ RGBColour(0.3450,0.1105,0.8789),
    /* 510 nm */ RGBColour(0.3450,0.1608,0.8894),
    /* 515 nm */ RGBColour(0.3450,0.2201,0.8987),
    /* 520 nm */ RGBColour(0.3450,0.2863,0.9064),
    /* 525 nm */ RGBColour(0.3450,0.3557,0.9122),
    /* 530 nm */ RGBColour(0.3450,0.4255,0.9156),
    /* 535 nm */ RGBColour(0.3450,0.4936,0.9160),
    /* 540 nm */ RGBColour(0.3472,0.5614,0.9160),
    /* 545 nm */ RGBColour(0.3523,0.6282,0.9160),
    /* 550 nm */ RGBColour(0.3605,0.6926,0.9160),
    /* 555 nm */ RGBColour(0.3722,0.7532,0.9160),
    /* 560 nm */ RGBColour(0.3874,0.8091,0.9160),
    /* 565 nm */ RGBColour(0.4065,0.8590,0.9160),
    /* 570 nm */ RGBColour(0.4295,0.9020,0.9160),
    /* 575 nm */ RGBColour(0.4563,0.9375,0.9160),
    /* 580 nm */ RGBColour(0.4868,0.9649,0.9160),
    /* 585 nm */ RGBColour(0.5205,0.9842,0.9160),
    /* 590 nm */ RGBColour(0.5568,0.9957,0.9160),
    /* 595 nm */ RGBColour(0.5951,1.0000,0.9160),
    /* 600 nm */ RGBColour(0.6353,1.0000,0.9172),
    /* 605 nm */ RGBColour(0.6784,1.0000,0.9219),
    /* 610 nm */ RGBColour(0.7223,1.0000,0.9290),
    /* 615 nm */ RGBColour(0.7653,1.0000,0.9376),
    /* 620 nm */ RGBColour(0.8058,1.0000,0.9467),
    /* 625 nm */ RGBColour(0.8423,1.0000,0.9556),
    /* 630 nm */ RGBColour(0.8741,1.0000,0.9639),
    /* 635 nm */ RGBColour(0.9014,1.0000,0.9712),
    /* 640 nm */ RGBColour(0.9242,1.0000,0.9776),
    /* 645 nm */ RGBColour(0.9427,1.0000,0.9829),
    /* 650 nm */ RGBColour(0.9574,1.0000,0.9872),
    /* 655 nm */ RGBColour(0.9688,1.0000,0.9906),
    /* 660 nm */ RGBColour(0.9774,1.0000,0.9932),
    /* 665 nm */ RGBColour(0.9838,1.0000,0.9951),
    /* 670 nm */ RGBColour(0.9884,1.0000,0.9964),
    /* 675 nm */ RGBColour(0.9917,1.0000,0.9975),
    /* 680 nm */ RGBColour(0.9942,1.0000,0.9982),
    /* 685 nm */ RGBColour(0.9959,1.0000,0.9987),
    /* 690 nm */ RGBColour(0.9971,1.0000,0.9991),
    /* 695 nm */ RGBColour(0.9979,1.0000,0.9994),
    /* 700 nm */ RGBColour(0.9985,1.0000,0.9995),
    /* 705 nm */ RGBColour(0.9990,1.0000,0.9997),
    /* 710 nm */ RGBColour(0.9993,1.0000,0.9998),
    /* 715 nm */ RGBColour(0.9995,1.0000,0.9999),
    /* 720 nm */ RGBColour(0.9996,1.0000,1.0000),
    /* 725 nm */ RGBColour(0.9998,1.0000,1.0000),
    /* 730 nm */ RGBColour(0.9999,1.0000,1.0000),
    /* 735 nm */ RGBColour(1.0000,1.0000,1.0000)
};
#else
// Variant 3:
// Values based on the formula used in previous POV-Ray versions (see below),
// presuming that the formula was originally designed to yield gamma-precorrected colors
// for a display gamma of 1.8.
//
// Original code used in previous versions:
//
//void Trace::ComputeDispersion(RGBColour& hue, unsigned int elem, unsigned int nelems)
//{
//  // Gives color to a dispersion element.
//  //
//  // Requirements:
//  // * Sum of all hues must add to white (or white*constand)
//  //   (white tiles seen through glass should still be white)
//  //
//  // * Each hue must be maximally saturated, bright
//  //   (The code shown here cheats a little)
//  //   [RLP: maximally saturated, anyway.  maximally bright is a
//  //    mistake.  And the new code no longer cheats.]
//  //
//  // * colors must range from red at elem=1 to violet at elem=nelems
//  //
//  // The equations herein were derived by back-of-the-envelope
//  // curve fitting to some RGB color-matching function tables
//  // I found on the web somewhere.  I could have just interpolated
//  // those tables, but I think this gives results that are as good
//  // and scale well.  The various magic numbers below were
//  // determined empirically to match four important criteria:
//  //
//  // 1) The peak for a given element must be at the same place as
//  //    on the color-matching table.
//  // 2) The width of a given element's curve must be about the same.
//  // 3) The width of the clipped portion of a given element must
//  //    be about the same.
//  // 4) The integral of each element's curve must be approximately
//  //    the same as the integral of each of the other elements.
//
//  // When I derived the functions, I went with the assumption that
//  // 0 is near-UV, and 1 is near-IR.  When I looked at the code, I
//  // realized that it wanted exactly the reverse.  Thus the "1.0-"
//  SNGL hc = 0.964 - 0.934 * ((SNGL)(elem - 1) / (SNGL)(nelems - 1));      // NB: rest of code was changed; nowadays this would be   SNGL hc = 0.964 - 0.934 *( 1.0 - (wavelength-SPECTRAL_VIOLET)/SPECTRAL_BANDWIDTH );
//
//  // The blue component.  The peak is at hc = 0.28, and there is no
//  // clipped part.  0.98 is a scaling factor to make the integrals
//  // come out even.  4 determines the width of the nonzero part
//  // of the curve; the larger the narrower.  The 1 helps determine
//  // the width of the clipped portion (but blue has no clipped
//  // portion.)  Four constraints, four magic numbers.  Who'da thunk
//  // it?
//  SNGL b = 0.98 * (1.0 - Sqr(4.0 * (hc-0.28)));
//  if(b < 0.0)
//      b = 0.0;
//  hue.blue() = b;                                                         // NB: rest of code was changed; nowadays this would be   hue.blue() = 3.0*b;
//
//  // This is substantially the same code as the blue code above,
//  // with different magic numbers.
//  SNGL g = 0.97 * (1.1 - Sqr(4.5 * (hc - 0.57)));
//  if(g < 0.0)
//      g = 0.0;
//  hue.green() = g;                                                        // NB: rest of code was changed; nowadays this would be   hue.green() = 3.0*g;
//
//  // This is also substantially the same code as the blue, with
//  // one exception: the red component has a second, smaller peak
//  // at the violet end of the spectrum.  That is represented by
//  // the second set of magic numbers below.  Also, red is the
//  // component to which the others are standardized (because it
//  // had the smallest integral to begin with) so there is no
//  // 0.9x fudge-factor.
//  SNGL r = 1.15 - Sqr(5.0 * (hc - 0.75));
//  if(r < 0.0)
//      r = 0.12 - Sqr(4.0 * (hc - 0.12));
//  if(r < 0.0)
//      r = 0.0;
//  hue.red() = r;                                                          // NB: rest of code was changed; nowadays this would be   hue.red() = 3.0*r;
//}
#define SPECTRAL_HUE_TABLE_SIZE 74
#define SPECTRAL_HUE_TABLE_BASE 375.0
#define SPECTRAL_HUE_TABLE_STEP 5.0
static RGBColour SpectralHueIntegral[SPECTRAL_HUE_TABLE_SIZE] = {
    /* 375 nm */ RGBColour(0.0000,0.0000,0.0000),
    /* 380 nm */ RGBColour(0.0009,0.0000,0.0038),
    /* 385 nm */ RGBColour(0.0029,0.0000,0.0113),
    /* 390 nm */ RGBColour(0.0058,0.0000,0.0222),
    /* 395 nm */ RGBColour(0.0094,0.0000,0.0363),
    /* 400 nm */ RGBColour(0.0135,0.0000,0.0534),
    /* 405 nm */ RGBColour(0.0178,0.0000,0.0734),
    /* 410 nm */ RGBColour(0.0223,0.0000,0.0960),
    /* 415 nm */ RGBColour(0.0267,0.0000,0.1210),
    /* 420 nm */ RGBColour(0.0308,0.0000,0.1482),
    /* 425 nm */ RGBColour(0.0344,0.0000,0.1775),
    /* 430 nm */ RGBColour(0.0374,0.0000,0.2085),
    /* 435 nm */ RGBColour(0.0394,0.0000,0.2412),
    /* 440 nm */ RGBColour(0.0404,0.0000,0.2753),
    /* 445 nm */ RGBColour(0.0404,0.0000,0.3106),
    /* 450 nm */ RGBColour(0.0404,0.0000,0.3470),
    /* 455 nm */ RGBColour(0.0404,0.0000,0.3841),
    /* 460 nm */ RGBColour(0.0404,0.0000,0.4219),
    /* 465 nm */ RGBColour(0.0404,0.0000,0.4600),
    /* 470 nm */ RGBColour(0.0404,0.0000,0.4984),
    /* 475 nm */ RGBColour(0.0404,0.0000,0.5368),
    /* 480 nm */ RGBColour(0.0404,0.0000,0.5750),
    /* 485 nm */ RGBColour(0.0404,0.0000,0.6128),
    /* 490 nm */ RGBColour(0.0404,0.0000,0.6500),
    /* 495 nm */ RGBColour(0.0404,0.0000,0.6864),
    /* 500 nm */ RGBColour(0.0404,0.0045,0.7218),
    /* 505 nm */ RGBColour(0.0404,0.0131,0.7560),
    /* 510 nm */ RGBColour(0.0404,0.0256,0.7888),
    /* 515 nm */ RGBColour(0.0404,0.0417,0.8200),
    /* 520 nm */ RGBColour(0.0404,0.0612,0.8494),
    /* 525 nm */ RGBColour(0.0404,0.0839,0.8768),
    /* 530 nm */ RGBColour(0.0404,0.1095,0.9020),
    /* 535 nm */ RGBColour(0.0404,0.1378,0.9248),
    /* 540 nm */ RGBColour(0.0404,0.1684,0.9449),
    /* 545 nm */ RGBColour(0.0404,0.2013,0.9623),
    /* 550 nm */ RGBColour(0.0404,0.2360,0.9767),
    /* 555 nm */ RGBColour(0.0404,0.2724,0.9878),
    /* 560 nm */ RGBColour(0.0404,0.3102,0.9956),
    /* 565 nm */ RGBColour(0.0404,0.3492,0.9997),
    /* 570 nm */ RGBColour(0.0404,0.3891,1.0000),
    /* 575 nm */ RGBColour(0.0429,0.4297,1.0000),
    /* 580 nm */ RGBColour(0.0502,0.4707,1.0000),
    /* 585 nm */ RGBColour(0.0620,0.5119,1.0000),
    /* 590 nm */ RGBColour(0.0780,0.5530,1.0000),
    /* 595 nm */ RGBColour(0.0979,0.5937,1.0000),
    /* 600 nm */ RGBColour(0.1215,0.6340,1.0000),
    /* 605 nm */ RGBColour(0.1483,0.6733,1.0000),
    /* 610 nm */ RGBColour(0.1781,0.7117,1.0000),
    /* 615 nm */ RGBColour(0.2105,0.7487,1.0000),
    /* 620 nm */ RGBColour(0.2454,0.7841,1.0000),
    /* 625 nm */ RGBColour(0.2823,0.8178,1.0000),
    /* 630 nm */ RGBColour(0.3210,0.8494,1.0000),
    /* 635 nm */ RGBColour(0.3611,0.8786,1.0000),
    /* 640 nm */ RGBColour(0.4024,0.9054,1.0000),
    /* 645 nm */ RGBColour(0.4446,0.9293,1.0000),
    /* 650 nm */ RGBColour(0.4873,0.9501,1.0000),
    /* 655 nm */ RGBColour(0.5302,0.9677,1.0000),
    /* 660 nm */ RGBColour(0.5731,0.9817,1.0000),
    /* 665 nm */ RGBColour(0.6155,0.9919,1.0000),
    /* 670 nm */ RGBColour(0.6573,0.9981,1.0000),
    /* 675 nm */ RGBColour(0.6982,1.0000,1.0000),
    /* 680 nm */ RGBColour(0.7377,1.0000,1.0000),
    /* 685 nm */ RGBColour(0.7756,1.0000,1.0000),
    /* 690 nm */ RGBColour(0.8116,1.0000,1.0000),
    /* 695 nm */ RGBColour(0.8454,1.0000,1.0000),
    /* 700 nm */ RGBColour(0.8767,1.0000,1.0000),
    /* 705 nm */ RGBColour(0.9052,1.0000,1.0000),
    /* 710 nm */ RGBColour(0.9306,1.0000,1.0000),
    /* 715 nm */ RGBColour(0.9525,1.0000,1.0000),
    /* 720 nm */ RGBColour(0.9707,1.0000,1.0000),
    /* 725 nm */ RGBColour(0.9849,1.0000,1.0000),
    /* 730 nm */ RGBColour(0.9947,1.0000,1.0000),
    /* 735 nm */ RGBColour(0.9998,1.0000,1.0000),
    /* 740 nm */ RGBColour(1.0000,1.0000,1.0000)
};
#endif

AttenuatingColour SpectralBand::GetHueIntegral(double wavelength)
{
    double tableOffset = clip((wavelength-SPECTRAL_HUE_TABLE_BASE)/SPECTRAL_HUE_TABLE_STEP, 0.0, SPECTRAL_HUE_TABLE_SIZE-1.0);
    int tableIndex = min((int)tableOffset, SPECTRAL_HUE_TABLE_SIZE-2);
    tableOffset -= tableIndex;
    return AttenuatingColour((1.0-tableOffset) * SpectralHueIntegral[tableIndex] + tableOffset * SpectralHueIntegral[tableIndex+1]);
}

#endif

}
