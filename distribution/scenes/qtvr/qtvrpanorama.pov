// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision Ray Tracer Scene Description File
// File: qtvrPanorama.pov
// Desc: Show how to design a QuickTime VR panoramic movie file.
//       After rendering this at 384x1248, you need to use
//       Apple's free application "Make QTVR Panorama" to
//       turn it into a panorama movie that can be displayed with
//       Apple's QTVR Player.
//
//       If you want better resolution, try any multiple of 4
//       for width, and multiple of 96 for height.  A good
//       high resolution QTVR scene would be 756x2016.
//
//       This scene is just a quick proof-of-concept, not a
//       full-fledged Myst environment!
// Date: 02/22/1998
// Auth: Eduard Schwan
// Camera  Enhancements: Stephen Andrusyszyn, balalaika@earthlink.net
//
// -w384 -h1284

#version 3.6;
global_settings {
  assumed_gamma 1.0
  max_trace_level 5
}

#include "colors.inc"
#include "metals.inc"

// ----------------------------------------
camera
{
  // Create a 360 degree camera view on its side
  cylinder 2     // sideways cylinder camera (along X axis)
  angle 360      // full 360 degree view

  right  1*x
  up     1/3*y  // wider angle
  sky    +x     // orient camera on its side (90 deg. CCW) for QTVR Pano tool
  location  <0.0, 0.0, 0.0>
  look_at   <0.0, 0.0, 1.0>
}


// ----------------------------------------
sky_sphere
{
  pigment
  {
    average
    pigment_map
    {
      [ gradient y  // gradient sky
        color_map { [0.0 color blue 0.2] [1.0 color rgb 1] } ]
      [ bozo turbulence 0.7 scale <0.5,0.1,0.5>  // and clouds
        color_map { [0.7 rgbf <0.8,0.8,1,1>] [0.8 color rgbf <0.5,0.5,0.5,0.8>] [1.0 rgb 1]} ]
    }
  }
}

// ----------------------------------------
#declare LightPosition = <-100, 50, -50>;
#declare LightShape = sphere
{
  0, 6
  texture { pigment {rgb <1,0.8,0.2>} finish {ambient 1 diffuse 0} }
}

light_source
{
  0*x // light's position (translated below)
  color rgb 1
  looks_like {LightShape} // yellow sun
  translate LightPosition
}


// ----------------------------------------
plane
{
  y, -1
  texture
  {
    pigment
    {
      agate scale 2
      color_map {[0.05 rgb <0.3,0.05,0>] [0.1 rgb <.5,.3,.1>] [0.2 green 0.3]}
    }
    normal {wrinkles 0.2 scale 0.1}
    finish {ambient 0.2 specular 0.1 roughness 0.2}
  }
}

// ----------------------------------------
// glass sphere (-z)
sphere
{
  0, 2
  translate <0,1,-6>
  texture
  {
    pigment {color rgb <0.5,1,1> filter 0.9}
    finish
    {
      specular 0.5
      irid { 0.4 thickness 0.2 turbulence 0.7 }
    }
  }
  interior { ior 1.4 }
}

// ----------------------------------------
// little marble (-x)
sphere
{
  0, 0.5
  texture
  {
    pigment{radial frequency 8 rotate 30*x}
    finish{specular 0.8}
  }
  translate <-3, -0.5, 0.5>
}

// ----------------------------------------
// tower of torii (+7x)
#declare tc = 0;
#while (tc < 5)
  torus
  {
    1, 0.5  rotate 90*x
    rotate (tc*20)*y
    translate <12,tc*2,3>
    texture
    { T_Brass_1C }
  }
  #declare tc = tc+1;
#end

// ----------------------------------------
// Gazebo in center, that we look out from (0)
// first, a rounded square +/-Z bore
#declare CylinderCluster = union
{
  cylinder {-z,+z,1}
  cylinder {-z,+z,1 translate -x-y}
  cylinder {-z,+z,1 translate +x-y}
  cylinder {-z,+z,1 translate -x+y}
  cylinder {-z,+z,1 translate +x+y}
  scale <0.4, 0.4, 1.1>
}

// now a box with the above cylinder set bored through it
#declare HollowBox = intersection
{
  box {-1, +1}
  object {CylinderCluster inverse}
  object {CylinderCluster rotate 90*y inverse}
  object {CylinderCluster rotate 90*x inverse}
}

#declare HBTex = texture
{
  pigment
  {
    crackle turbulence 0.3 scale 0.2  rotate 20*y
//    color_map {[0.05 red 0.1] [0.1 rgb <.8,.6,.4>] [0.3 rgb 0.7]}
    color_map {[0.05 blue 0.1] [0.1 rgb <.4,.6,.8>] [0.3 rgb 0.7]}
  }
  normal
  {
    crackle turbulence 0.3 scale 0.2  rotate 20*y bump_size 0.4
  }
  finish { ambient 0.2 specular 0.5 roughness 0.1 }
}

object { HollowBox texture{HBTex} }

// ----------------------------------------
// another gazebo off in the distance (+z)
object { HollowBox texture{HBTex} rotate 60*y translate 4*z}


