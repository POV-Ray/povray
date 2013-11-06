
// Camera previews for Insert Menu: Cameras
// by Bob Hughes August 28, 2001
// use the following command switches, naming each accordingly
//       modified Aug. 29 by Christoph Hormann
// Updated to 3.7 by Friedrich A. Lohmueller, June-2012.

// +fs4 +w120 +h60 +a0.2 +r2 +am2 +j0.5
#version 3.7;
global_settings{ assumed_gamma 1.0 } 


/*
#declare Typ=1;   // perspective
#declare Typ=2;   // orthographic
#declare Typ=3;   // fisheye
#declare Typ=4;   // ultra_wide_angle
#declare Typ=5;   // omnimax
#declare Typ=6;   // panoramic
#declare Typ=7;   // cylinder
#declare Typ=8;   // spherical

#declare Typ=9;   // focus
#declare Typ=10;  // normal
#declare Typ=11;  // sky
*/

#if (Typ>8)
  #declare Lens=0;
#else
  #declare Lens=Typ-1;
#end

//#declare Lens=0; // type listed above

#declare Focus=no;

#declare Normal=no;

#declare Sky=no;

#if (Typ=9)
  #declare Focus=yes;
#end

#if (Typ=10)
  #declare Normal=yes;
#end

#if (Typ=11)
  #declare Sky=yes;
#end

/* ----------------------------------------------------------------------- */

// the camera
camera
{
 #switch (Lens)
 #case (0)
  perspective
 #break
 #case (1)
  orthographic
 #break
 #case (2)
  fisheye
 #break
 #case (3)
  ultra_wide_angle
 #break
 #case (4)
  omnimax
 #break
 #case (5)
  panoramic
 #break
 #case (6)
  cylinder 1
 #break
 #case (7)
  spherical
 #break
 #end

// (---viewpoint---)
  location  <0.0, 6.0, -6.0>   // position of camera <X Y Z>

// (---sky---)
 #if (Sky=yes)
  sky       <0.5,1,0>          // for tilting the camera
 #end

// (---aspect---)
 #if (Lens!=1)
  right     x*(image_width/image_height)
                               // which way is +right <X Y Z> and aspect ratio
  up        y                  // which way is +up <X Y Z>
 #else
   right  x*image_width/image_height 
  // right x*6 // for orthographic only
  //up y*3
 #end

// (---center---)
  look_at   <1.0, 1.0,  1.0>   // point center of view at this point <X Y Z>

// (---zoom---)
//  direction 2.0*z              // which way are we looking <X Y Z> & zoom
 #if (Lens!=7)
   #if (Lens!=3)
    angle 50//150                // overrides "direction" with specific angle [67 typical]
  #else
    angle 123 // for ultra_wide_angle lens example
   #end
 #end
// (---normal---)
 #if (Normal=yes)
  normal { ripples 0.1 scale 0.3 }     // perturb the camera lens with a pattern
 #end

// (---focal blur---)
 #if (Focus=yes)
  aperture 3               // 0...N (bigger is narrower depth of field)
  blur_samples 30             // # of rays per pixel
  focal_point <1,1,2>       // x,y,z point that is in focus
  confidence 0.9
  variance 0             // how precise focal blur is calculated
 #end

// (---spherical camera---)
 #if (Lens=7)
  angle 360
        180
 #end

}

/* ----------------------------------------------------------------------- */

// point light
light_source
{
        0,1.5
  translate <10, 20, -10>   // <x y z> position of light
}

/* ----------------------------------------------------------------------- */

#declare Floor=
plane
{
        y,-0.001
        pigment {rgb 1}
}

Floor

/* ----------------------------------------------------------------------- */

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


object
{
        CSG1
        pigment {rgb 1}
        translate <2.75,1,1>
}

object
{
        CSG2
        pigment {rgb 1}
        translate <-0.75,1,1>
}

/* ----------------------------------------------------------------------- */
