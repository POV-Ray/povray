// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

//    Persistence of Vision Ray Tracer version 3.5 Include File
//    File: consts.inc
//    Last updated: 2001.8.12
//    Description: Various constants and alias definitions.

#ifndef(Consts_Inc_Temp)
#declare Consts_Inc_Temp = version;
#version 3.5;

#ifdef(View_POV_Include_Stack)
    #debug "including consts.inc\n"
#end

#declare e = exp(1);//2.71828182845904523536028747135266249775724709369996;

#declare o = < 0, 0, 0>;
#declare xy = < 1, 1, 0>;
#declare yz = < 0, 1, 1>;
#declare xz = < 1, 0, 1>;

// MAP_TYPE CONSTANTS
#declare Plane_Map    = 0;
#declare Sphere_Map   = 1;
#declare Cylinder_Map = 2;
#declare Torus_Map    = 5;

// INTERPOLATION TYPE CONSTANTS
#declare Bi           = 2;
#declare Norm         = 4;

// FOG TYPE CONSTANTS
#declare Uniform_Fog = 1;
#declare Ground_Fog  = 2;

// FOCAL BLUR HEXGRID CONSTANTS
#declare Hex_Blur1    = 7;
#declare Hex_Blur2    = 19;
#declare Hex_Blur3    = 37;

// Dispersion amount constants
//Glass: 1.01 - 1.05
#declare Quartz_Glass_Dispersion = 1.012;
#declare Water_Dispersion = 1.007;
#declare Diamond_Dispersion = 1.035;
#declare Sapphire_Dispersion = 1.015;

// INDEX OF REFRACTION CONSTANTS
// Defines a few Index of Refractions for various materials for sodium light.
// Source: College Physics by Arthur L. Kimball, PhD. 4th Edition (1923)
// ---------------------------
#declare Flint_Glass_Ior = 1.71;
#declare Crown_Glass_Ior = 1.51;
#declare Diamond_Ior     = 2.47;
#declare Water_Ior       = 1.33;
#declare Air_Ior         = 1.000292;

#declare Ice_Ior = 1.31;
#declare Fluorite_Ior = 1.434;
#declare Plexiglas_Ior = 1.5;
#declare Gypsum_Ior =1.525;
#declare Salt_Ior = 1.544;

#declare Quartz_Ior = 1.550;
#declare Citrine_Ior = 1.550;
#declare Amethyst_Ior = 1.550;

#declare Beryl_Ior = 1.575;
#declare Aquamarine_Ior = 1.575;
#declare Emerald_Ior = 1.575;

#declare Topaz_Ior = 1.620;
#declare Apatite_Ior = 1.635;
#declare Tourmaline_Ior = 1.650;

#declare Corundum_Ior = 1.765;
#declare Ruby_Ior = 1.765;
#declare Sapphire_Ior = 1.765;

#declare Quartz_Glass_Ior = 1.458;
#declare Flint_Glass_Heavy_Ior = 1.8;
#declare Flint_Glass_Medium_Ior = 1.63;
#declare Flint_Glass_Light_Ior = 1.6;



// POV-Ray 3.1 NOTE: These are included for backwards compatibility only.
// Use the camera "angle" parameter for version 3.0 and above.
//
// Direction Vectors for various Field of View angles.
// The formula used to calculate these is:  FoV = 0.5 / tan(angle/2)
// Based on the height, (the UP vector), not width or diagonal.
// Useage:  direction <0, 0, FoV_45>
// (You will also need to adjust the location vector if you change FoV and
// want to keep the same visual distance from your scene.)
#declare FoV_15  = 7.595981;
#declare FoV_30  = 3.732166;
#declare FoV_45  = 2.414293;
#declare FoV_60  = 1.732113;
#declare FoV_75  = 1.303277;
#declare FoV_90  = 1.000046;
#declare FoV_105 = 0.767370;
#declare FoV_120 = 0.577391;
#declare FoV_135 = 0.414254;
#declare FoV_150 = 0.267991;
#declare FoV_165 = 0.131696;

// ATMOSPHERE TYPES
#declare ISOTROPIC_SCATTERING         = 1;
#declare MIE_HAZY_SCATTERING          = 2;
#declare MIE_MURKY_SCATTERING         = 3;
#declare RAYLEIGH_SCATTERING          = 4;
#declare HENYEY_GREENSTEIN_SCATTERING = 5;

#version Consts_Inc_Temp;
#end
