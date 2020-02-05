#ifdef(Consts_Inc_Temp)
// do nothing
#else
#declare Consts_Inc_Temp = version;

#ifdef(View_POV_Include_Stack)
#   debug "including consts.inc\n"
#end

/*
              Persistence of Vision Raytracer Version 3.1
                 Various constants and alias definitions
*/

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

// INDEX OF REFRACTION CONSTANTS
// Defines a few Index of Refractions for various materials for sodium light.
// Source: College Physics by Arthur L. Kimball, PhD. 4th Edition (1923)
// ---------------------------
#declare Flint_Glass_Ior = 1.71;
#declare Crown_Glass_Ior = 1.51;
#declare Diamond_Ior     = 2.47;
#declare Water_Ior       = 1.33;
#declare Air_Ior         = 1.000292;

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

// BASIC RADIOSITY MODES
// These are used in the "rad_def.inc" file.
#declare Radiosity_Debug   = 1;
#declare Radiosity_Fast    = 2;
#declare Radiosity_Normal  = 3;
#declare Radiosity_2Bounce = 4;
#declare Radiosity_Final   = 5;


#version Consts_Inc_Temp;
#end
