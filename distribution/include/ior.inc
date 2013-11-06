// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

//    Persistence of Vision Ray Tracer version 3.7 Include File
//    File: ior.inc
//    Last updated: 2010.12.12
//    Description: This file contains constants for ior and dispersion of various materials.

#ifndef(Ior_Inc_Temp)
#declare Ior_Inc_Temp=version;
#version 3.7;

#ifdef(View_POV_Include_Stack)
#debug "including ior.inc\n"
#end

// Index Of Refraction and Dispersion of various materials
// =======================================================

//  Original 2008-2009 by Christoph Lipka

//  Notes on literature:
//
//  * Refractive index is typically denoted with the symbol "n" (sometimes "N"), optionally followed by a subscript designating
//    the specific conditions under which measurements were made. Common subscripts are:
//    - Greek lower case letters omega and epsilon:
//      These are related to birefringence (i.e. the property of a material to refract light at two different angles, splitting up
//      the light according to polarization).
//      Since PoV-ray currently doesn't support bireferingence at all, we can ignore this effect and use an average of the values.
//    - Greek lower case letters alpha, beta and gamma:
//      These are related to biaxial birefringence, aka trirefringence. Again, no PoV-ray support.
//      Sometimes, Nx, Ny and Nz or simply alpha, beta and gamma are used for n[alpha], n[beta], n[gamma].
//    - Latin uppercase (sometimes lowercase) letters:
//      These are related to the wavelength of the light; the letter denotes the corresponding "Fraunhofer line", a series of
//      distinct spectral lines in natural sunlight, with the following being most common:
//        B   686.719nm   extreme red   molecular Oxygen (O2)
//        C   656.281nm   red           Hydrogen (H)
//        D   589.29 nm   yellow        Sodium (Na); center of D1 and D2 ("Sodium doublet")
//        F   486.134nm   blue          Hydrogen (H)
//        G   430.78 nm   violet        Iron (Fe, 430.790nm) or Calcium (Ca, 430.774 nm)
//        h   410.175nm   violet        Hydrogen (H); Balmer series delta; alternatively center of delta and epsilon (404.7 nm)
//    - If no further designation is used, the refractive index is usually measured for Fraunhofer line "D".
//    The PoV-ray documentation does not specify which color (let alone wavelength) is considered "neutral" with respect to
//    dispersion, so I suggest using nD, if only for simplicity.
//
//  * Dispersion is typically characterized in one of the following ways:
//    - A (typically undesignated) value in order of magnitude ~0.010:
//      Such a value specifies the difference of the refractive indices for two given wavelengths. Usually, nG-nB is specified.
//      Differences between other wavelengths will be denoted accordingly as e.g. "nF-nC".
//      This is often seen with natural minerals.
//    - The so-called "Abbe number":
//      This value specifies the quotient (nD-1)/(nF-nC).
//      This is often seen with industrial mass products, especially glass and common glass substitutes.
//    - By refractive indices for individual wavelengths.
//      This is often seen with industrial high-end products.
//    PoV-ray characterizes dispersion as the quotient of refractive indices for "violet" and "red"; assuming that this roughly
//    matches nG and nB respectively, the value to be used can quite easily be approximated from nG-nB and the overall index of
//    refraction, as long as nG-nB is significantly small (which in practice it always is).
//    Approximating the PoV-ray "dispersion" value from the Abbe number is not so straightforward though, as this is based on a
//    different wavelength interval, and the index of refraction is not linear with respect to wavelength.
//    Approximating the PoV-ray "dispersion" value from individual wavelengths is again an easy task of course, provided that the
//    wavelengths of two of the known refractive indices qualify as "violet" and "red" respectively.
//
//  Other notes:
//
//  * Not all of the materials listed below are typically transparent. The refractive indices may nonetheless be of interest
//    in subsurface scattering simulation, or to properly simulate a polished gem's reflection using the fresnel keyword,
//    so I included them here as I happened to come across them.
//  * Some materials have varying optical properties or exhibit birefringence, making it impossible to specify a single exact
//    refractive index; in such cases, a typical average value was chosen.


// Formula to approximate PoV-ray's "dispersion" value
// ---------------------------------------------------
 
#declare FnPovDispersion = function(ri,disp) { 1+disp/(ri-disp/2) } // from refractive index ri (=nD) and nominal dispersion disp (=nG-nB)


// Common glass types by Schott designations
// -----------------------------------------

// Note: For these, the POV values are computed here according to the Fraunhofer lines h (404.7 nm), D and C

#macro Ior (data) (data.y) #end
#macro Disp(data) (FnPovDispersion(data.y, data.x-data.z)) #end

#local iorData=<1.58941, 1.57241, 1.56949>; #declare iorCrownGlassBaK1    = Ior(iorData); #declare dispCrownGlassBaK1    = Disp(iorData);
#local iorData=<1.53024, 1.51673, 1.51432>; #declare iorCrownGlassBK7     = Ior(iorData); #declare dispCrownGlassBK7     = Disp(iorData);

#local iorData=<1.65064, 1.61989, 1.61503>; #declare iorFlintGlassF2      = Ior(iorData); #declare dispFlintGlassF2      = Disp(iorData);
#local iorData=<1.89844, 1.85002, 1.84256>; #declare iorFlintGlassLaSFN9  = Ior(iorData); #declare dispFlintGlassLaSFN9  = Disp(iorData);
#local iorData=<1.84208, 1.78446, 1.77599>; #declare iorFlintGlassSF11    = Ior(iorData); #declare dispFlintGlassSF11    = Disp(iorData);

#declare iorCrownGlass    = iorCrownGlassBK7;   #declare dispCrownGlass   = dispCrownGlassBK7;  // most common crown glass
#declare iorFlintGlass    = iorFlintGlassSF11;  #declare dispFlintGlass   = dispFlintGlassSF11; // a common flint glass
#declare iorWindowGlass   = iorCrownGlassBK7;   #declare dispWindowGlass  = dispCrownGlassBK7;
#declare iorGlass         = iorWindowGlass;     #declare dispGlass        = dispWindowGlass;

#undef Ior
#undef Disp


// Gemstones by alphabet (excluding variants and aliases)
// ------------------------------------------------------

// Note: For these, the POV values are computed here directly from the refractive index and dispersion values found in literature;
// Where refractive indices at selected wavelengths were found instead, these values were precomputed from the refractive indices at
// Fraunhofer lines G, D and B respectively.

#macro Ior (data) (data.x) #end
#macro Disp(data) (FnPovDispersion(data.x, data.y)) #end

#local iorData=<1.790, .024>; #declare iorAlmandineGarnet    = Ior(iorData); #declare dispAlmandineGarnet    = Disp(iorData);
#local iorData=<1.64,  .016>; #declare iorAndalusite         = Ior(iorData); #declare dispAndalusite         = Disp(iorData);
#local iorData=<1.888, .057>; #declare iorAndraditeGarnet    = Ior(iorData); #declare dispAndraditeGarnet    = Disp(iorData);
#local iorData=<1.63,  .013>; #declare iorApatite            = Ior(iorData); #declare dispApatite            = Disp(iorData);
#local iorData=<1.58,  .014>; #declare iorBeryl              = Ior(iorData); #declare dispBeryl              = Disp(iorData);
#local iorData=<1.69,  .019>; #declare iorChromeDiopside     = Ior(iorData); #declare dispChromeDiopside     = Disp(iorData);
#local iorData=<1.746, .015>; #declare iorChrysoberyl        = Ior(iorData); #declare dispChrysoberyl        = Disp(iorData);
#local iorData=<2.16,  .060>; #declare iorCubicZirconia      = Ior(iorData); #declare dispCubicZirconia      = Disp(iorData); // [1]    NOT Zircon!
#local iorData=<1.766, .018>; #declare iorCorundum           = Ior(iorData); #declare dispCorundum           = Disp(iorData);
#local iorData=<1.885, .057>; #declare iorDemantoidGarnet    = Ior(iorData); #declare dispDemantoidGarnet    = Disp(iorData);
#local iorData=<2.417, .044>; #declare iorDiamond            = Ior(iorData); #declare dispDiamond            = Disp(iorData);
#local iorData=<1.434, .007>; #declare iorFluorite           = Ior(iorData); #declare dispFluorite           = Disp(iorData);
#local iorData=<1.740, .028>; #declare iorGrossulariteGarnet = Ior(iorData); #declare dispGrossulariteGarnet = Disp(iorData);
#local iorData=<1.55,  .017>; #declare iorIolite             = Ior(iorData); #declare dispIolite             = Disp(iorData);
#local iorData=<1.73,  .020>; #declare iorKyanite            = Ior(iorData); #declare dispKyanite            = Disp(iorData);
#local iorData=<1.654, .020>; #declare iorPeridot            = Ior(iorData); #declare dispPeridot            = Disp(iorData);
#local iorData=<1.74,  .022>; #declare iorPyropeGarnet       = Ior(iorData); #declare dispPyropeGarnet       = Disp(iorData);
#local iorData=<1.544, .013>; #declare iorQuartz             = Ior(iorData); #declare dispQuartz             = Disp(iorData);
#local iorData=<1.76,  .026>; #declare iorRhodoliteGarnet    = Ior(iorData); #declare dispRhodoliteGarnet    = Disp(iorData);
#local iorData=<1.81,  .027>; #declare iorSpessartiteGarnet  = Ior(iorData); #declare dispSpessartiteGarnet  = Disp(iorData);
#local iorData=<1.70,  .051>; #declare iorSphene             = Ior(iorData); #declare dispSphene             = Disp(iorData); // [2]
#local iorData=<1.712, .020>; #declare iorSpinel             = Ior(iorData); #declare dispSpinel             = Disp(iorData);
#local iorData=<1.620, .014>; #declare iorTopaz              = Ior(iorData); #declare dispTopaz              = Disp(iorData);
#local iorData=<1.624, .017>; #declare iorTourmaline         = Ior(iorData); #declare dispTourmaline         = Disp(iorData);
#local iorData=<1.740, .028>; #declare iorTsavoriteGarnet    = Ior(iorData); #declare dispTsavoriteGarnet    = Disp(iorData);

#local iorData=<1.67,  .017>; #declare iorSpodumene          = Ior(iorData); #declare dispSpodumene          = Disp(iorData);
#local iorData=<1.95,  .039>; #declare iorZircon             = Ior(iorData); #declare dispZircon             = Disp(iorData); // [3]    NOT [Cubic] Zirconia!
#local iorData=<1.70,  .030>; #declare iorZoisite            = Ior(iorData); #declare dispZoisite            = Disp(iorData);

#undef Ior
#undef Disp

// Values from other sources:
// [1] ior = 1.80 .. 2.23 (lower bound dubious, maybe mixup with Zircon)
// [2] ior = 1.88 .. 2.05
// [3] ior = 1.88


// Variants by alphabet
// --------------------

#declare iorAgate         = iorQuartz;        #declare dispAgate        = dispQuartz;
#declare iorAlexandrite   = iorChrysoberyl;   #declare dispAlexandrite  = dispChrysoberyl;
#declare iorAmethyst      = iorQuartz;        #declare dispAmethyst     = dispQuartz;
#declare iorAmetrine      = iorQuartz;        #declare dispAmetrine     = dispQuartz;
#declare iorAquamarine    = iorBeryl;         #declare dispAquamarine   = dispBeryl;
#declare iorAventurine    = iorQuartz;        #declare dispAventurine   = dispQuartz;
#declare iorChalcedony    = iorQuartz;        #declare dispChalcedony   = dispQuartz;
#declare iorCitrine       = iorQuartz;        #declare dispCitrine      = dispQuartz;
#declare iorEmerald       = iorBeryl;         #declare dispEmerald      = dispBeryl;
#declare iorHiddenite     = iorSpodumene;     #declare dispHiddenite    = dispSpodumene;
#declare iorKunzite       = iorSpodumene;     #declare dispKunzite      = dispSpodumene;
#declare iorMorganite     = iorBeryl;         #declare dispMorganite    = dispBeryl;
#declare iorOnyx          = iorQuartz;        #declare dispOnyx         = dispQuartz;
#declare iorPadparadscha  = iorCorundum;      #declare dispPadparadscha = dispCorundum;
#declare iorRoseQuartz    = iorQuartz;        #declare dispRoseQuartz   = dispQuartz;
#declare iorRuby          = iorCorundum;      #declare dispRuby         = dispCorundum;
#declare iorSapphire      = iorCorundum;      #declare dispSapphire     = dispCorundum;
#declare iorSmokyQuartz   = iorQuartz;        #declare dispSmokyQuartz  = dispQuartz;
#declare iorTanzanite     = iorZoisite;       #declare dispTanzanite    = dispZoisite;
#declare iorTigersEye     = iorQuartz;        #declare dispTigersEye    = dispQuartz;


// Aliases by alphabet
// -------------------

#declare iorCyanite             = iorKyanite;             #declare dispCyanite            = dispKyanite;
#declare iorCZ                  = iorCubicZirconia;       #declare dispCZ                 = dispCubicZirconia;
#declare iorDisthene            = iorKyanite;             #declare dispDisthene           = dispKyanite;
#declare iorGarnetAlmandine     = iorAlmandineGarnet;     #declare dispGarnetAlmandine    = dispAlmandineGarnet; 
#declare iorGarnetAndradite     = iorAndraditeGarnet;     #declare dispGarnetAndradite    = dispAndraditeGarnet;   
#declare iorGarnetDemantoid     = iorDemantoidGarnet;     #declare dispGarnetDemantoid    = dispDemantoidGarnet;   
#declare iorGarnetGrossularite  = iorGrossulariteGarnet;  #declare dispGarnetGrossularite = dispGrossulariteGarnet;   
#declare iorGarnetPyrope        = iorPyropeGarnet;        #declare dispGarnetPyrope       = dispPyropeGarnet;   
#declare iorGarnetRhodolite     = iorRhodoliteGarnet;     #declare dispGarnetRhodolite    = dispRhodoliteGarnet;   
#declare iorGarnetSpessartite   = iorSpessartiteGarnet;   #declare dispGarnetSpessartite  = dispSpessartiteGarnet;   
#declare iorGarnetTsavorite     = iorTsavoriteGarnet;     #declare dispGarnetTsavorite    = dispTsavoriteGarnet;   
#declare iorMunkrudite          = iorKyanite;             #declare dispMunkrudite         = dispKyanite;
#declare iorZirconia            = iorCubicZirconia;       #declare dispZirconia           = dispCubicZirconia;      // NOT Zircon!


// Odds and ends
// -------------

// Note: For these, I found information on refractive indices but none about dispersion

#declare iorAdulariaMoonstone   = 1.52;
#declare iorAmazonite           = 1.53;
#declare iorAmber               = 1.54;
#declare iorAndesine            = 1.53;
#declare iorCoral               = 1.486;
#declare iorIvory               = 1.540;
#declare iorJadeite             = 1.67;   // "hard jade"
#declare iorJasper              = 1.54;
#declare iorLabradorite         = 1.56;
#declare iorLapisLazuli         = 1.5;    // lazurite compound; other sources: nD = 1.610
#declare iorMalachite           = 1.655;
#declare iorMoissanite          = 2.670;  // (Silicon Carbide)
#declare iorNephriteJade        = 1.62;   // "soft jade"
#declare iorOpal                = 1.45;
#declare iorOrthoclase          = 1.52;
#declare iorPearl               = 1.53;
#declare iorPrehnite            = 1.64;
#declare iorTurquoise           = 1.610;

#declare iorJadeNephrite        = iorNephriteJade;
#declare iorFireOpal            = iorOpal;
#declare iorMoonstoneAdularia   = iorAdulariaMoonstone;
#declare iorMossOpal            = iorOpal;

// end of ior.inc
#version Ior_Inc_Temp;
#end
