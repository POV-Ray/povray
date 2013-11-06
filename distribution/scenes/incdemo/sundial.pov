// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision Raytracer Scene Description File
// File: sundial.pov
// Desc: Horizontal and vertical sundail
//       This sundial model is NOT suitable for the southern hemisphere,
//       or for use near the poles and equator.
// Date: 15-09-1998
// Auth: Ingo Janssen
//
// Make an animation of 52 frames. The shadow will describe the figure 8.
// This is the analemma:
// http://www.uwm.edu/People/kahl/Images/Weather/Other/analemma.html
//
// The motto for this sundial:
// "By a shadow I explain the heavens"
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;
global_settings { assumed_gamma 1.0 }

#include "sunpos.inc"

camera {
   location  <3.0, 5.0, -5.0>
   right     x*image_width/image_height
   look_at   <0.40, 1.8, 0.0>
   angle 40
}

#declare Year= 1998;
#declare Month= 1;
#declare Day= 1+int(clock*365);
#declare Hour= 12;
#declare Minute= 35;
#declare Lstm= 15;
#declare LONG= 6.9;
#declare LAT= 52.266;

   //Put in the sun
light_source {
   SunPos(Year, Month, Day, Hour, Minute, Lstm, LAT, LONG)
   rgb 1
}

background {rgb 0}

    // Build the horizontal hourlines of the dail.
#declare DailH= cylinder{ <0, 0, 0>, <0, 0, 2>, 0.01 }
#declare HCount= -30;

#while ( HCount<90 )
   #declare RotH = degrees(atan2(tan(radians(HCount))/sin(radians(LAT)),1));
   #declare HourlinesH = union {
      cylinder{
         <-2, 0, 0>, <0, 0, 0>, 0.01
         rotate <0, RotH, 0>
      }
      cylinder{
         <-2, 0, 0>, <0, 0, 0>, 0.01
         rotate <0, 180-RotH, 0>
      }
   }
   #declare DailH= union {
      object {HourlinesH}
      object {DailH}
   }
   #declare HCount=HCount+15;
#end //while

#declare HGnomon= cylinder {
   <0, 0, 0>, <0, 0.53, 0>, 0.02
   rotate <90-LAT, 0, 0>
   pigment {rgb<1,0,0>}
}

    // Build the vertical hourlines.
#declare DailV= cylinder{ <0, 0, 0>, <0,-2, 0>, 0.01 }
#declare VCount= -30;

#while ( VCount<90 )
   #declare RotV = degrees(atan2(tan(radians(VCount))/cos(radians(LAT)),1));
   #declare HourlinesV = union {
      cylinder {
         <-2, 0, 0>, <0, 0, 0>, 0.01
         rotate <0, 0, RotV>
      }
      cylinder{
         <-2,0, 0>, <0, 0, 0>, 0.01
         rotate <0, 0, 180-RotV>
      }
   }
   #declare DailV = union {
      object {HourlinesV}
      object {DailV}
   }
   #declare VCount= VCount+15;
#end //while

#declare VGnomon= cylinder {
    <0, -1.05, 0>, <0, 0, 0>, 0.02
    rotate <90-LAT, 0, 0>
    pigment {rgb <1,0,0>}
}

    // Put it all pieces together.
#declare Sundail= union {
   difference {
      box { <-1, 0, 0>, <1, 2.5, 2.5> }
      object {
         DailH
         translate <0, 2.5, 0.3>
         pigment {rgb <0,0,1>}
      }
      object {
         DailV
         translate <0, 2.2, 0>
         pigment {rgb <0,0,1>}
      }
   }
   object {
      HGnomon
      translate <0, 2.5, 0.3>
   }
   object {
      VGnomon
      translate <0, 2.2, 0>
   }
}

object {
   Sundail
   pigment{rgb 1}
}
