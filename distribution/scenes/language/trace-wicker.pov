// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
//
//=================================================================================
//                                 WICKER WORK with "trace ()".
//=================================================================================
// [Fabien Mosen - August 2001]
//
// -w320 -h240
// -w800 -h600 +a0.3

//This scene demonstrates the use of the "trace" function to create shapes
//that fits irregular shapes.  We are going to model a wicker work surrounding
//a bottle, like the old Italian wine bottles.

#version 3.6;
global_settings {assumed_gamma 1.0}

#include "colors.inc"

//=========================================== WAVY RINGS =========================
//A macro to produce the wavy wicker rings
//freq : frequency, amplit : amplitude, phase : phase between -1 et 1 (usually 1)
//radius1 : big radius of ring, radius2 : radius of wicker
//Degree_Resolution : lenght of the little cylinders making the ring

// Updated: 10Aug2008 (jh) for v3.7 distribution

#macro WickerRing (Freq, Amplit, Phase, Radius1, Radius2,Degree_Resolution)
 union {
  #local P1=<Radius1,0,0>+.0001;
  #local Boucle=0;
  #while (Boucle < 360)
   #local Position = Phase*Amplit*sin(radians(Boucle*Freq));
   #local P2 = P1;
   #local P1 = vrotate(<Radius1+Position, 0, 0>,y*Boucle);
   cylinder {P1,P2, Radius2}
  #local Boucle=Boucle+Degree_Resolution;
  #end
  cylinder {P2,<Radius1,0,0>, Radius2}
 }
#end

//=========================================== THE BOTTLE ======================
//This bottle was produced by SpilinEditor, and exported with height normalized
//to 1 unit (which allows easy scaling afterwards).

#declare BottleLathe =
lathe{
	cubic_spline
	15,
	<0.036140,0.004819>,<0.228886,0.036140>,<0.293938,0.279482>,
	<0.106010,0.599923>,<0.055415,0.850493>,<0.103601,0.951684>,
	<0.072280,0.990234>,<0.038549,0.963731>,<0.062643,0.619197>,
	<0.277073,0.257798>,<0.185518,0.038549>,<0.014456,0.060233>,
	<0.036140,0.004819>,<0.228886,0.036140>,<0.293938,0.279482>
}

#declare BigBottle = object {
 BottleLathe
 scale 50
 pigment {MediumForestGreen} finish {phong .9}
}

object {BigBottle}

//================================== PLACING THE RINGS ========================

//"scanning" the bottle from bottom to top.

#declare Loop=0;    //from 0
#while (Loop < 25)  //to 25, wicker stopping at middle height
                    //but you can change that.

   #declare StartTrace = <50,Loop,0>; //50 is the distance for the start of
                                      //the tracing ray.  Adjust to your
                                      //bottle's maximum diameter

   #declare FoundPoint = trace (BigBottle, StartTrace, <-1,0,0>);
   //now, we know the radius of the bottle at a given height.

   //then we call the ring macro, using the found radius, with a frequency
   //of 20

   object {WickerRing (20, .5, 1, 1+FoundPoint.x, .2, 2)
           translate y*Loop pigment {rgb <255/255,212/255,117/255>}
                              finish {phong .5 specular .5}
                              normal {bumps .2 scale .3}
           rotate y*Loop*18 //rotate each time to alternate the rings '
                            //phases (360/20 = 18)
           }

#declare Loop=Loop+.5;
#end

//================================== VERTICAL FIBERS ========================
//creating the vertical fiber passing trough all rings, using the
//same method than previously.
#declare V_Fiber = union {
   #declare StartTrace = <50,0,0>;
   #declare FoundPoint = trace (BigBottle, StartTrace, <-1,0,0>);
   #declare P1=FoundPoint+.0001;
#declare Loop=0;
#while (Loop < 35)
   #declare StartTrace = <50,Loop,0>;
   #declare FoundPoint = trace (BigBottle, StartTrace, <-1,0,0>);
   #declare P2=P1;
   #declare P1=FoundPoint+.5;
    cylinder {P1,P2,.3}
#declare Loop=Loop+1;
#end
   }//end of union

object {BigBottle}

//placing vertical fibers
#declare I=-1;
#while (I < 360)
 object {V_Fiber rotate y*I
                              pigment {rgb <255/255,201/255,107/255>}
                              finish {phong .5 specular .5}
                              normal {bumps .2 scale .3}
                              }
#declare I=I+9;//rotation  = 0.5*(rings rotation)
#end

//=========================================== SCENERY =========================
light_source {<100,300,200> White*2}
camera { location <100,100,100> 
         right x*image_width/image_height
         angle 25 //  direction z*3 
         look_at <0,25,0>
       }
plane {y,0 pigment {White}}
