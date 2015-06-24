// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

//    Persistence of Vision Ray Tracer version 3.7 Include File
//    File: ior.inc
//    Last updated: 2015-06-24
//    Description: This file contains constants for ior and dispersion of various materials.

#ifndef(Ior_Inc_Temp)
#declare Ior_Inc_Temp=version;
#version 3.71;

#ifdef(View_POV_Include_Stack)
#debug "including ior.inc\n"
#end

// Index Of Refraction and Dispersion of various materials
// =======================================================

//  Original 2008-2015 by Christoph Lipka

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
//      We're also using the following:
//        e   438.355nm   violet        Iron (Fe); note that designation "e" may also be used for the 546.073nm Mercury (Hg) line
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


// Macros to approximate POV-Ray's "dispersion" value
// --------------------------------------------------

#local Average = function(x,y) { (x+y)/2 }

// from refractive index n (nD) and nominal dispersion disp (nG-nB)
#macro IorData_n_disp (n, optional disp)
  #local iorResult = n;
  #ifdef(local(disp))
    #local dispResult = 1+disp/(n-disp/2);
  #end
  ( iorResult , local(dispResult) )
#end

// from typical refractive index range n1 to n2 (nD) and typical nominal dispersion disp1 to disp2 (nG-nB)
#macro IorData_n_disp_Range (n1, optional n2, optional disp1, optional disp2)
  #ifdef(local(n2))
    #local n = Average(n1,n2);
  #else
    #local n = n1;
  #end
  #ifdef(local(disp1))
    #ifdef(local(disp2))
      #local disp = Average(disp1,disp2);
    #else
      #local disp = disp1;
    #end
  #end
  IorData_n_disp(n, local(disp))
#end

// from refractive indices at Fraunhofer lines G, D and B
#macro IorData_G_D_B (nG, nD, nB)
  IorData_n_disp(nD, nG-nB)
#end

// from refractive indices at three other wavelengths;
// for now we're happy with Fraunhofer lines e(Fe), h or 404.7nm instead of nG, and with Fraunhofer line C instead of B
#macro IorData_eFe_D_B   ( neFe , nD , nB ) IorData_G_D_B ( neFe , nD , nB ) #end
#macro IorData_G_D_C     ( nG   , nD , nC ) IorData_G_D_B ( nG   , nD , nC ) #end
#macro IorData_h_D_C     ( nh   , nD , nC ) IorData_G_D_B ( nh   , nD , nC ) #end
#macro IorData_405nm_D_C ( n405 , nD , nC ) IorData_G_D_B ( n405 , nD , nC ) #end


// Glasses
// -------

// Common Optical Glasses by Schott Designations

// Source: Schott Optical Glass Data Sheets, 04/15/2015, found at:
//         http://www.schott.com/advanced_optics/english/abbe_datasheets/schott-datasheet-all-english.pdf
#declare ( iorGlassBaK1     , dispGlassBaK1     ) = IorData_405nm_D_C ( 1.58941, 1.57241, 1.56949 );
#declare ( iorGlassBK7      , dispGlassBK7      ) = IorData_405nm_D_C ( 1.53024, 1.51673, 1.51432 );
#declare ( iorGlassF2       , dispGlassF2       ) = IorData_405nm_D_C ( 1.65064, 1.61989, 1.61503 );
#declare ( iorGlassLaSFN9   , dispGlassLaSFN9   ) = IorData_405nm_D_C ( 1.89845, 1.85002, 1.84255 );
#declare ( iorGlassSF11     , dispGlassSF11     ) = IorData_405nm_D_C ( 1.84235, 1.78446, 1.77596 );


// Multi-Purpose Glasses

// Source: http://refractiveindex.info
#declare ( iorQuartzGlass   , dispQuartzGlass   ) = IorData_G_D_B     ( 1.4671,  1.4584,  1.4556  ); // fused silica
#declare ( iorSodaLimeGlass , dispSodaLimeGlass ) = IorData_G_D_B     ( 1.5338,  1.5233,  1.5199  );

// Generic Glasses

#declare ( iorCrownGlass                  , optional dispCrownGlass         ) = ( iorGlassBK7      , dispGlassBK7      ); // most common crown glass
#declare ( iorFlintGlass                  , optional dispFlintGlass         ) = ( iorGlassSF11     , dispGlassSF11     ); // a common flint glass
#declare ( iorWindowGlass                 , optional dispWindowGlass        ) = ( iorSodaLimeGlass , dispSodaLimeGlass );
#declare ( iorGlass                       , optional dispGlass              ) = ( iorSodaLimeGlass , dispSodaLimeGlass );

// Glass Aliases

#declare ( deprecated iorCrownGlassBaK1   , deprecated dispCrownGlassBaK1   ) = ( iorGlassBaK1     , dispGlassBaK1     );
#declare ( deprecated iorCrownGlassBK7    , deprecated dispCrownGlassBK7    ) = ( iorGlassBK7      , dispGlassBK7      );
#declare ( deprecated iorFlintGlassF2     , deprecated dispFlintGlassF2     ) = ( iorGlassF2       , dispGlassF2       );
#declare ( deprecated iorFlintGlassLaSFN9 , deprecated dispFlintGlassLaSFN9 ) = ( iorGlassLaSFN9   , dispGlassLaSFN9   );
#declare ( deprecated iorFlintGlassSF11   , deprecated dispFlintGlassSF11   ) = ( iorGlassSF11     , dispGlassSF11     );


// Plastics and Organic Materials
// ------------------------------

// Generic Plastics and Organic Materials

// Source: http://refractiveindex.info
#declare ( iorCellulose   , dispCellulose   ) = IorData_eFe_D_B ( 1.4808 , 1.4701 , 1.4666 ); // NOT nitrocellulose or celluloid!
#declare ( iorPC          , dispPC          ) = IorData_eFe_D_B ( 1.6109 , 1.5846 , 1.5765 ); // polycarbonate
#declare ( iorPMMA        , dispPMMA        ) = IorData_G_D_B   ( 1.5018 , 1.4905 , 1.4872 ); // polymethylmethacrylate
#declare ( iorPS          , dispPS          ) = IorData_eFe_D_B ( 1.6165 , 1.5893 , 1.5837 ); // polystyrene
#declare ( iorPVP         , dispPVP         ) = IorData_G_D_B   ( 1.5449 , 1.5273 , 1.5233 ); // polyvinylpyrrolidone

// Source: http://www.matbase.com
#declare ( iorABS         ,                 ) = IorData_n_disp  (           1.6   ,        ); // acrylonitrile butadiene styrene

// Source: http://www.goodfellow.com
#declare ( iorPVC         ,                 ) = IorData_n_disp  (           1.54  ,        ); // polyvinylchloride

// Commercial Plastics

// Source: http://refractiveindex.info
#declare ( iorNAS21       , dispNAS21       ) = IorData_eFe_D_B ( 1.5929 , 1.5711 , 1.5644 ); // a styrene methyl methacrylate
#declare ( iorOptorez1330 , dispOptorez1330 ) = IorData_eFe_D_B ( 1.5216 , 1.5094 , 1.5055 );
#declare ( iorZeonexE48R  , dispZeonexE48R  ) = IorData_eFe_D_B ( 1.5431 , 1.5304 , 1.5264 );

// Plastics Aliases

#declare ( iorAcrylonitrileButadieneStyrene , optional dispAcrylonitrileButadieneStyrene ) = ( iorABS  , dispABS  );
#declare ( iorPolycarbonate                 , optional dispPolycarbonate                 ) = ( iorPC   , dispPC   );
#declare ( iorPolymethylmethacrylate        , optional dispPolymethylmethacrylate        ) = ( iorPMMA , dispPMMA );
#declare ( iorPolystyrene                   , optional dispPolystyrene                   ) = ( iorPS   , dispPS   );
#declare ( iorPolyvinylchloride             , optional dispPolyvinylchloride             ) = ( iorPVP  , dispPVP  );
#declare ( iorPolyvinylpyrrolidone          , optional dispPolyvinylpyrrolidone          ) = ( iorPVP  , dispPVP  );

// Plastics Trademarks

#declare ( iorAcrylicGlass                  , optional dispAcrylicGlass                  ) = ( iorPMMA , dispPMMA );
#declare ( iorPlexiglas                     , optional dispPlexiglas                     ) = ( iorPMMA , dispPMMA );
#declare ( iorAcrylite                      , optional dispAcrylite                      ) = ( iorPMMA , dispPMMA );
#declare ( iorLucite                        , optional dispLucite                        ) = ( iorPMMA , dispPMMA );
#declare ( iorPerspex                       , optional dispPerspex                       ) = ( iorPMMA , dispPMMA );


// Gemstones and Minerals
// ----------------------

// Source: http://gemologyproject.com
#declare ( iorAchroite           , dispAchroite           ) = IorData_n_disp_Range ( 1.62 ,1.64  , .017,     );
#declare ( iorAmber              ,                        ) = IorData_n_disp       ( 1.54        ,           );
#declare ( iorAmmolite           ,                        ) = IorData_n_disp_Range ( 1.525,1.670 ,     ,     );
#declare ( iorAnatase            , dispAnatase            ) = IorData_n_disp_Range ( 2.488,2.561 , .213,     );
#declare ( iorAndalusite         , dispAndalusite         ) = IorData_n_disp_Range ( 1.63 ,1.64  , .016,     );
#declare ( iorAndesine           ,                        ) = IorData_n_disp_Range ( 1.551,1.60  ,     ,     );
#declare ( iorAndraditeGarnet    , dispAndraditeGarnet    ) = IorData_n_disp_Range ( 1.85 ,1.89  , .057,     );
#declare ( iorApatite            , dispApatite            ) = IorData_n_disp_Range ( 1.63 ,1.64  , .013,     );
#declare ( iorAxinite            , dispAxinite            ) = IorData_n_disp_Range ( 1.674,1.706 , .018,.020 );
#declare ( iorAzurite            ,                        ) = IorData_n_disp_Range ( 1.720,1.848 ,     ,     );
#declare ( iorBarite             ,                        ) = IorData_n_disp_Range ( 1.634,1.648 ,     ,     );
#declare ( iorBenitoite          , dispBenitoite          ) = IorData_n_disp_Range ( 1.75 ,1.80  , .046,     );
#declare ( iorBeryl              , dispBeryl              ) = IorData_n_disp_Range ( 1.577,1.583 , .014,     );
#declare ( iorDiopside           , dispDiopside           ) = IorData_n_disp_Range ( 1.67 ,1.70  , .013,     );
#declare ( iorChrysoberyl        , dispChrysoberyl        ) = IorData_n_disp_Range ( 1.74 ,1.75  , .015,     );
#declare ( iorClinohumite        ,                        ) = IorData_n_disp_Range ( 1.628,1.674 ,     ,     );
#declare ( iorCopal              ,                        ) = IorData_n_disp       ( 1.54        ,           );
#declare ( iorCoral              ,                        ) = IorData_n_disp_Range ( 1.48 ,1.56  ,     ,     );
#declare ( iorCorundum           , dispCorundum           ) = IorData_n_disp_Range ( 1.762,1.770 , .018,     );
#declare ( iorCubicZirconia      , dispCubicZirconia      ) = IorData_n_disp_Range ( 2.171,2.177 , .059,.065 ); // [1]; NOT Zircon!
#declare ( iorDanburite          , dispDanburite          ) = IorData_n_disp_Range ( 1.630,1.636 , .017,     );
#declare ( iorDatolite           , dispDatolite           ) = IorData_n_disp_Range ( 1.622,1.670 , .016,     );
#declare ( iorDiamond            , dispDiamond            ) = IorData_n_disp       ( 2.417       , .044      );
#declare ( iorDiaspore           ,                        ) = IorData_n_disp_Range ( 1.682,1.752 ,     ,     );
#declare ( iorDioptase           , dispDioptase           ) = IorData_n_disp_Range ( 1.644,1.709 , .030,     );
#declare ( iorEkanite            ,                        ) = IorData_n_disp_Range ( 1.560,1.596 ,     ,     );
#declare ( iorEnstatite          , dispEnstatite          ) = IorData_n_disp_Range ( 1.65 ,1.68  , .010,     );
#declare ( iorFluorite           , dispFluorite           ) = IorData_n_disp       ( 1.434       , .007      );
#declare ( iorFosterite          ,                        ) = IorData_n_disp_Range ( 1.634,1.670 ,     ,     );
#declare ( iorGrossulariteGarnet , dispGrossulariteGarnet ) = IorData_n_disp_Range ( 1.738,1.745 , .027,     );
#declare ( iorIolite             , dispIolite             ) = IorData_n_disp_Range ( 1.53 ,1.55  , .017,     );
#declare ( iorJasper             ,                        ) = IorData_n_disp_Range ( 1.53 ,1.54  ,     ,     );
#declare ( iorKornerupine        , dispKornerupine        ) = IorData_n_disp_Range ( 1.67 ,1.68  , .019,     );
#declare ( iorKyanite            , dispKyanite            ) = IorData_n_disp_Range ( 1.710,1.734 , .020,     );
#declare ( iorLabradorite        ,                        ) = IorData_n_disp_Range ( 1.559,1.570 ,     ,     );
#declare ( iorLapisLazuli        ,                        ) = IorData_n_disp_Range ( 1.500,1.670 ,     ,     ); // lazurite compound
#declare ( iorMalachite          ,                        ) = IorData_n_disp_Range ( 1.655,1.909 ,     ,     );
#declare ( iorMawSitSit          ,                        ) = IorData_n_disp       ( 1.52        ,           ); // common value; nD may range up to 1.74
#declare ( iorMoissanite         , dispMoissanite         ) = IorData_n_disp_Range ( 2.648,2.691 , .104,     );
#declare ( iorMoonstone          ,                        ) = IorData_n_disp       ( 1.52        ,           );
#declare ( iorNephriteJade       ,                        ) = IorData_n_disp       ( 1.62        ,           ); // "soft jade"
#declare ( iorOpal               ,                        ) = IorData_n_disp       ( 1.45        ,           );
#declare ( iorOrthoclase         , dispOrthoclase         ) = IorData_n_disp_Range ( 1.52 ,1.53  , .012,     );
#declare ( iorPearl              ,                        ) = IorData_n_disp_Range ( 1.52 ,1.69  ,     ,     );
#declare ( iorPectolite          ,                        ) = IorData_n_disp_Range ( 1.59 ,1.63  ,     ,     );
#declare ( iorPeridot            , dispPeridot            ) = IorData_n_disp_Range ( 1.65 ,1.69  , .020,     );
#declare ( iorPrehnite           ,                        ) = IorData_n_disp_Range ( 1.616,1.649 ,     ,     );
#declare ( iorPyropeGarnet       , dispPyropeGarnet       ) = IorData_n_disp_Range ( 1.730,1.760 , .022,     );
#declare ( iorQuartz             , dispQuartz             ) = IorData_n_disp_Range ( 1.544,1.553 , .013,     );
#declare ( iorSiderite           ,                        ) = IorData_n_disp_Range ( 1.633,1.875 ,     ,     );
#declare ( iorSphene             , dispSphene             ) = IorData_n_disp_Range ( 1.880,2.099 , .051,     );
#declare ( iorSpinel             , dispSpinel             ) = IorData_n_disp_Range ( 1.712,1.736 , .026,     ); // [1]
#declare ( iorSpodumene          , dispSpodumene          ) = IorData_n_disp_Range ( 1.66 ,1.68  , .017,     );
#declare ( iorTopaz              , dispTopaz              ) = IorData_n_disp_Range ( 1.606,1.644 , .014,     );
#declare ( iorTourmaline         , dispTourmaline         ) = IorData_n_disp_Range ( 1.62, 1.64  , .018,     );
#declare ( iorTurquoise          ,                        ) = IorData_n_disp_Range ( 1.610,1.650 ,     ,     );
#declare ( iorVesuvianite        , dispVesuvianite        ) = IorData_n_disp_Range ( 1.700,1.725 , .019,     );
#declare ( iorZircon             , dispZircon             ) = IorData_n_disp_Range ( 1.78, 1.99  , .039,     ); // NOT [Cubic] Zirconia!
#declare ( iorZoisite            , dispZoisite            ) = IorData_n_disp_Range ( 1.685,1.707 , .012,     ); // [2]

// Source: Wikipedia
#declare ( iorAlmandineGarnet    , dispAlmandineGarnet    ) = IorData_n_disp       ( 1.79        , .024      );
#declare ( iorJadeite            ,                        ) = IorData_n_disp_Range ( 1.654,1.693 ,     ,     ); // "hard jade"
#declare ( iorRhodoliteGarnet    , dispRhodoliteGarnet    ) = IorData_n_disp       ( 1.76        , .026      );
#declare ( iorTsavoriteGarnet    , dispTsavoriteGarnet    ) = IorData_n_disp       ( 1.740       , .028      );

// Source:  Arthur Thomas, Gemstones: Properties, Identification and Use
#declare ( iorBoracite           ,                        ) = IorData_n_disp_Range ( 1.658,1.673 ,     ,     );
#declare ( iorCinnabar           ,                        ) = IorData_n_disp_Range ( 2.91 ,3.27  ,     ,     );
#declare ( iorCuprite            ,                        ) = IorData_n_disp       ( 2.85        ,           );
#declare ( iorDumortierite       ,                        ) = IorData_n_disp_Range ( 1.678,1.689 ,     ,     ); // NOT the gem of same name, which is quartz with dumortierite inclusions
#declare ( iorEudialyte          ,                        ) = IorData_n_disp_Range ( 1.598,1.613 ,     ,     );
#declare ( iorFibrolite          , dispFibrolite          ) = IorData_n_disp_Range ( 1.658,1.678 , .015,     );
#declare ( iorGaspeite           ,                        ) = IorData_n_disp_Range ( 1.160,1.830 ,     ,     );
#declare ( iorGrandiderite       ,                        ) = IorData_n_disp_Range ( 1.602,1.639 ,     ,     );
#declare ( iorHambergite         , dispHambergite         ) = IorData_n_disp_Range ( 1.55 ,1.63  , .015,     );
#declare ( iorHematite           ,                        ) = IorData_n_disp_Range ( 2.94 ,3.22  ,     ,     );
#declare ( iorKutnohorite        ,                        ) = IorData_n_disp_Range ( 1.535,1.727 ,     ,     );
#declare ( iorLawsonite          ,                        ) = IorData_n_disp_Range ( 1.665,1.686 ,     ,     );
#declare ( iorPainite            ,                        ) = IorData_n_disp_Range ( 1.787,1.816 ,     ,     );
#declare ( iorPezzottaite        ,                        ) = IorData_n_disp_Range ( 1.601,1.620 ,     ,     );
#declare ( iorPhenakite          , dispPhenakite          ) = IorData_n_disp_Range ( 1.654,1.670 , .015,     );
#declare ( iorPrehnite           ,                        ) = IorData_n_disp_Range ( 1.616,1.649 ,     ,     );
#declare ( iorProustite          ,                        ) = IorData_n_disp_Range ( 2.790,3.088 ,     ,     );
#declare ( iorPyragyrite         ,                        ) = IorData_n_disp_Range ( 2.881,3.084 ,     ,     );
#declare ( iorRhodozite          ,                        ) = IorData_n_disp       ( 1.69        ,           );
#declare ( iorSimpsonite         ,                        ) = IorData_n_disp_Range ( 2.025,2.045 ,     ,     );
#declare ( iorSinhalite          , dispSinhalite          ) = IorData_n_disp_Range ( 1.667,1.705 , .017,     );
#declare ( iorSmithsonite        ,                        ) = IorData_n_disp_Range ( 1.625,1.850 ,     ,     );
#declare ( iorSphalerite         , dispSphalerite         ) = IorData_n_disp_Range ( 2.37 ,2.42  , .156,     );
#declare ( iorStichtite          ,                        ) = IorData_n_disp_Range ( 1.516,1.545 ,     ,     );
#declare ( iorWillemite          ,                        ) = IorData_n_disp_Range ( 1.691,1.725 ,     ,     );

// Values from other sources:
// [1] disp = 0.020
// [2] disp = 0.030

// Source: ?
#declare ( iorSpessartiteGarnet  , dispSpessartiteGarnet  ) = IorData_n_disp       ( 1.81        , .027      );
#declare ( iorIvory              ,                        ) = IorData_n_disp       ( 1.540       ,           );

// Gemstone and Mineral Variants

#declare ( iorAgate              , optional dispAgate              ) = ( iorQuartz             , dispQuartz             );
#declare ( iorAlexandrite        , optional dispAlexandrite        ) = ( iorChrysoberyl        , dispChrysoberyl        );
#declare ( iorAmethyst           , optional dispAmethyst           ) = ( iorQuartz             , dispQuartz             );
#declare ( iorAmetrine           , optional dispAmetrine           ) = ( iorQuartz             , dispQuartz             );
#declare ( iorAquamarine         , optional dispAquamarine         ) = ( iorBeryl              , dispBeryl              );
#declare ( iorAventurine         , optional dispAventurine         ) = ( iorQuartz             , dispQuartz             );
#declare ( iorBixbite            , optional dispBixbite            ) = ( iorBeryl              , dispBeryl              );
#declare ( iorCalifornite        , optional dispCalifornite        ) = ( iorVesuvianite        , dispVesuvianite        );
#declare ( iorChalcedony         , optional dispChalcedony         ) = ( iorQuartz             , dispQuartz             );
#declare ( iorChromeDiopside     , optional dispChromeDiopside     ) = ( iorDiopside           , dispDiopside           );
#declare ( iorCitrine            , optional dispCitrine            ) = ( iorQuartz             , dispQuartz             );
#declare ( iorCyprine            , optional dispCyprine            ) = ( iorVesuvianite        , dispVesuvianite        );
#declare ( iorDemantoidGarnet    , optional dispDemantoidGarnet    ) = ( iorAndraditeGarnet    , dispAndraditeGarnet    );
#declare ( iorEmerald            , optional dispEmerald            ) = ( iorBeryl              , dispBeryl              );
#declare ( iorGoshenite          , optional dispGoshenite          ) = ( iorBeryl              , dispBeryl              );
#declare ( iorGreenovite         , optional dispGreenovite         ) = ( iorSphene             , dispSphene             );
#declare ( iorHeliodor           , optional dispHeliodor           ) = ( iorBeryl              , dispBeryl              );
#declare ( iorHiddenite          , optional dispHiddenite          ) = ( iorSpodumene          , dispSpodumene          );
#declare ( iorKunzite            , optional dispKunzite            ) = ( iorSpodumene          , dispSpodumene          );
#declare ( iorLarimar            , optional dispLarimar            ) = ( iorPectolite          , dispPectolite          );
#declare ( iorMaxixe             , optional dispMaxixe             ) = ( iorBeryl              , dispBeryl              );
#declare ( iorMorganite          , optional dispMorganite          ) = ( iorBeryl              , dispBeryl              );
#declare ( iorOnyx               , optional dispOnyx               ) = ( iorQuartz             , dispQuartz             );
#declare ( iorPadparadscha       , optional dispPadparadscha       ) = ( iorCorundum           , dispCorundum           );
#declare ( iorRoseQuartz         , optional dispRoseQuartz         ) = ( iorQuartz             , dispQuartz             );
#declare ( iorRuby               , optional dispRuby               ) = ( iorCorundum           , dispCorundum           );
#declare ( iorSapphire           , optional dispSapphire           ) = ( iorCorundum           , dispCorundum           );
#declare ( iorSmokyQuartz        , optional dispSmokyQuartz        ) = ( iorQuartz             , dispQuartz             );
#declare ( iorTanzanite          , optional dispTanzanite          ) = ( iorZoisite            , dispZoisite            );
#declare ( iorTigersEye          , optional dispTigersEye          ) = ( iorQuartz             , dispQuartz             );
#declare ( iorViolane            , optional dispViolane            ) = ( iorDiopside           , dispDiopside           );

// Gemstone and Mineral Aliases

#declare ( iorAdulariaMoonstone  , optional dispAdulariaMoonstone  ) = ( iorMoonstone          , dispMoonstone          );
#declare ( iorAmazonite          , optional dispAmazonite          ) = ( iorOrthoclase         , dispOrthoclase         );
#declare ( iorAmbosselite        , optional dispAmbosselite        ) = ( iorVesuvianite        , dispVesuvianite        );
#declare ( iorCarborundum        , optional dispCarborundum        ) = ( iorMoissanite         , dispMoissanite         );
#declare ( iorCordierite         , optional dispCordierite         ) = ( iorIolite             , dispIolite             );
#declare ( iorChalybite          , optional dispChalybite          ) = ( iorSiderite           , dispSiderite           );
#declare ( iorChromeSphene       , optional dispChromeSphene       ) = ( iorSphene             , dispSphene             );
#declare ( iorCyanite            , optional dispCyanite            ) = ( iorKyanite            , dispKyanite            );
#declare ( iorCZ                 , optional dispCZ                 ) = ( iorCubicZirconia      , dispCubicZirconia      );
#declare ( iorDisthene           , optional dispDisthene           ) = ( iorKyanite            , dispKyanite            );
#declare ( iorFireOpal           , optional dispFireOpal           ) = ( iorOpal               , dispOpal               );
#declare ( iorFluorspar          , optional dispFluorspar          ) = ( iorFluorite           , dispFluorite           );
#declare ( iorGarnetAlmandine    , optional dispGarnetAlmandine    ) = ( iorAlmandineGarnet    , dispAlmandineGarnet    );
#declare ( iorGarnetAndradite    , optional dispGarnetAndradite    ) = ( iorAndraditeGarnet    , dispAndraditeGarnet    );
#declare ( iorGarnetDemantoid    , optional dispGarnetDemantoid    ) = ( iorDemantoidGarnet    , dispDemantoidGarnet    );
#declare ( iorGarnetGrossularite , optional dispGarnetGrossularite ) = ( iorGrossulariteGarnet , dispGrossulariteGarnet );
#declare ( iorGarnetPyrope       , optional dispGarnetPyrope       ) = ( iorPyropeGarnet       , dispPyropeGarnet       );
#declare ( iorGarnetRhodolite    , optional dispGarnetRhodolite    ) = ( iorRhodoliteGarnet    , dispRhodoliteGarnet    );
#declare ( iorGarnetSpessartite  , optional dispGarnetSpessartite  ) = ( iorSpessartiteGarnet  , dispSpessartiteGarnet  );
#declare ( iorGarnetTsavorite    , optional dispGarnetTsavorite    ) = ( iorTsavoriteGarnet    , dispTsavoriteGarnet    );
#declare ( iorIdocrase           , optional dispIdocrase           ) = ( iorVesuvianite        , dispVesuvianite        );
#declare ( iorJadeNephrite       , optional dispJadeNephrite       ) = ( iorNephriteJade       , dispNephriteJade       );
#declare ( iorKFeldspar          , optional dispKFeldspar          ) = ( iorOrthoclase         , dispOrthoclase         );
#declare ( iorMoonstoneAdularia  , optional dispMoonstoneAdularia  ) = ( iorMoonstone          , dispMoonstone          );
#declare ( iorMossOpal           , optional dispMossOpal           ) = ( iorOpal               , dispOpal               );
#declare ( iorMunkrudite         , optional dispMunkrudite         ) = ( iorKyanite            , dispKyanite            );
#declare ( iorTitanite           , optional dispTitanite           ) = ( iorSphene             , dispSphene             );
#declare ( iorZirconia           , optional dispZirconia           ) = ( iorCubicZirconia      , dispCubicZirconia      ); // NOT Zircon!
#declare ( iorZultanite          , optional dispZultanite          ) = ( iorDiaspore           , dispDiaspore           );


// Cleanup
// -------

#undef IorData_n_disp
#undef IorData_n_disp_Range
#undef IorData_G_D_B
#undef IorData_eFe_D_B
#undef IorData_G_D_C
#undef IorData_h_D_C
#undef IorData_405nm_D_C

// end of ior.inc
#version Ior_Inc_Temp;
#end
