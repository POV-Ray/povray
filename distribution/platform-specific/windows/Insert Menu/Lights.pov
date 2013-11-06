// Persistence of Vision Ray Tracer Scene Description File
// File: im_lights.pov
// Vers: 3.6 / 3.7
// Desc: Lights previews for Insert Menu
// Date: August 2001
// Auth: Bob Hughes
//       modified Aug. 25 by Christoph Hormann
// Updated to 3.7 by Friedrich A. Lohmueller, June-2012.

// +w120 +h60 +a0.2 +r2 +am2 +j0.5
#version 3.7;
global_settings{ assumed_gamma 1.0 } 

/*
#declare Typ=1;   // point source
#declare Typ=2;   // area light
#declare Typ=3;   // cylindrical light
#declare Typ=4;   // spotlight
#declare Typ=5;   // parallel
#declare Typ=6;   // fading
#declare Typ=7;   // looks like
#declare Typ=8;   // light group
#declare Typ=9;   // projected through
*/

#declare LightS=Typ-1;

camera
{
        location <0,6,-6>
        right  x*image_width/image_height
        look_at 1
}

#declare Floor=
plane
{
        y,-0.001
        pigment {rgb 1}
}

#if (LightS!=7)
Floor
#end


#declare CSG1=
difference
{
        box {0,2}
        sphere {1,1.5 inverse}
        sphere {1,1.25}
 translate -1 // center on world origin
}

#declare CSG2=
difference
{
        box {0,2}
        sphere {1,1.25 inverse}
 translate -1 // center on world origin
}


#if (LightS!=8)
#if (LightS!=6)
object
{
        CSG1
        pigment {rgb 1}
        translate <2.75,1,1>
}
#end

object
{
        CSG2
        pigment {rgb 1}
        translate <-0.75,1,1>
}

#end


#declare CSG0=
object
{
        CSG1
        pigment {rgb 1}
        translate <2.75,1,1>
}

#declare CSG3=
object
{
        CSG1
        translate <5,10,-5>
}

#declare CSGs=
union
{
 object
 {
        CSG1
        translate <2.75,1,1>
 }
 object
 {
        CSG2
        translate <-0.75,1,1>
 }
}


#switch (LightS)

#case (0)

// point light
light_source
{
        0,1.5
  translate <10, 20, -10>   // <x y z> position of light
}

#break

#case (1)

// An area light (creates soft shadows)
// WARNING: This special light can significantly slow down rendering times!
light_source
{
  0*x                 // light's position (translated below)
  color rgb 1.5       // light's color
  area_light
  <6, 0, 0> <0, 0, 6> // lights spread out across this distance (x * z)
  6, 6                // total number of lights in grid (4x*4z = 16 lights)
  adaptive 2          // 0,1,2,3...
  jitter              // adds random softening of light
  circular            // make the shape of the light circular
  orient              // orient light
  translate <10, 20, -10>   // <x y z> position of light
}

#break

#case (2)

// create a point "spotlight" (cylindrical directed) light source
light_source
{
  0*x                     // light's position (translated below)
  color rgb 1.5       // light's color
  spotlight               // this kind of light source
  cylinder                // this variation
  translate <10, 20, -10> // <x y z> position of light
  point_at <0, 1, 1>      // direction of spotlight
  radius 20                // hotspot (inner, in degrees)
  tightness 10            // tightness of falloff (1...100) lower is softer, higher is tighter
  falloff 30               // intensity falloff radius (outer, in degrees)
}

#break

#case (3)

// create a point "spotlight" (conical directed) light source
light_source
{
  0*x                     // light's position (translated below)
  color rgb 1.5       // light's color
  spotlight               // this kind of light source
  translate <10, 20, -10> // <x y z> position of light
  point_at <0, 1, 1>      // direction of spotlight
  radius 5                // hotspot (inner, in degrees)
  tightness 10            // tightness of falloff (1...100) lower is softer, higher is tighter
  falloff 10               // intensity falloff radius (outer, in degrees)
}

#break

#case (4)

light_source
{
        0,1.5
  translate <10, 20, -10> // <x y z> position of light
  // put this inside a light_source to make it parallel
  parallel
  point_at <0, 1, 1>
}

#break

#case (5)

light_source
{
        0,1.5
  translate <10, 20, -10> // <x y z> position of light
  // put this inside a light_source to add light fading
  fade_distance 20
  fade_power 3
}

#break

#case (6)

light_source
{
        <2.75, 1, 1>,1.5
  // put this inside a light_source to give it a visible appearance
  looks_like { object {CSG1 pigment {rgbf <1,1,1,0.75>}}}
}

#break

#case (7)

// a light group makes certain light sources
// influence specific objects
light_group
{
  light_source            // light source(s) of this group
  {
        0,1.5
  translate <10, 20, -10> // <x y z> position of light
  }
  object
  {
        Floor             // objects illuminated by those light sources
  }
  object
  {
        CSG0              // objects illuminated by those light sources
  }
  //global_lights         // add this to make all global lights
                          // also illuminating this light group
}

#break

#case (8)

light_source
{
        0,1.5
  translate <10, 20, -10> // <x y z> position of light
  // put this inside a light_source to give the light beam
  // the shape of the object
  projected_through { object { CSGs } }
}

#break

#end
