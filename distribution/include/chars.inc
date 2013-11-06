// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

//    Persistence of Vision Ray Tracer version 3.5 Include File
//    File: chars.inc
//    Last updated: 2001.7.21
//    Description: This file contains letters as objects.

#ifndef(Chars_Inc_Temp)
#declare Chars_Inc_Temp=version;
#version 3.5;

#ifdef(View_POV_Include_Stack)
#   debug "including chars.inc\n"
#end

/*

		   Characters for POV-Ray Version 3.1
		     Original by Ken Maeno 1992-93
		  Revised and extended by Chris Young
	    Revised for internal web removal by John Andersen

 This file includes 26 upper-case letter and other characters defined
 as objects. The size of all characters is 4 * 5 * 1. The center of the
 bottom side of a character face is set to the origin, so please
 translate a character appropriately before rotating it about
 the x or z axes.

	Usage:

	object{
	  char_A
	  pigment{ color Red }
	}

*/

#declare char_A =
 intersection {
   box{<-2,0,-0.1>,<2,5,1.1>}
   union {
     box{<-0.5,-6,0>,<0.5,0,1> rotate  z*16.699 }
     box{<-0.5,-6,0>,<0.5,0,1> rotate -z*16.699 }
     box{<-1,  -4,0>,<1, -3,1> }
     translate y*5
   }
   bounded_by{ box { <-2,0,0>,<2,5,1> } }
 }

#declare char_P =
  union {
    difference{
      cylinder{0*z,z,1.5}
      cylinder{-z,2*z,0.5}
      plane{x,0}
      bounded_by{ box { <0,-1.5,0>, <1.5, 1.5, 1> } }
      translate<0.5, 3.5, 0>
    }
    box{ <-1.001, 4, 0>, <0.501, 5, 1> }
    box{ <-1.001, 2, 0>, <0.501, 3, 1> }
    box{ <-2, 0, 0>, <-1, 5, 1 > }
  }

#declare char_B =
  union {
    object{char_P}
    difference{
      cylinder{0*z,z,1.5}
      cylinder{-z,2*z,0.5}
      plane{x,0}
      bounded_by{ box { <0,-1.5,0>, <1.5, 1.5, 1> } }
      translate<0.5, 1.5, 0>
    }
    box{ <-1.001, 0, 0>, <0.501, 1, 1> }
  }

#declare char_C =
  union {
    difference{
      cylinder{0*z,z,2}
      cylinder{-z,2*z,1}
      plane{y,0}
      bounded_by{ box { <-2,0,0>, <2,2,1> } }
      translate 3*y
    }
    difference{
      cylinder{0*z,z,2}
      cylinder{-z,2*z,1}
      plane{-y,0}
      bounded_by{ box { <-2,-2,0>, <2,0,1> } }
      translate 2*y
    }
    box{ <-2,1.999,0>,<-1,3.001,1> }
  }

#declare char_D =
  union {
    difference{
      cylinder{0*z,z,2.5}
      cylinder{-z,2*z,1.5}
      plane{x,0}
      bounded_by{ box { <0,-2.5,0>, <2.5,2.5,1> } }
      translate <-0.5,2.5,0>
    }
    box{ <-1.001,4,0>,<-0.499,5,1> }
    box{ <-1.001,0,0>,<-0.499,1,1> }
    box{ <-2,0,0>,<-1,5,1> }
  }

#declare char_F =
  union {
    box{ <-2,0,0>,<-1,5,1> }
    box{ <-1.001,2,0>,<1.5,3,1> }
    box{ <-1.001,4,0>,<2,5,1> }
  }

#declare char_E =
  union {
    object{char_F}
    box{ <-1.001,0,0>,<2,1,1> }
  }

#declare char_G =
  union {
    object{char_C}
    box{ < 0,1.5,0>,<2,2.3,1> }
  }

#declare char_H =
  union {
    box{ <-2,0,0>,<-1,5,1> }
    box{ < 1,0,0>,< 2,5,1> }
    box{ <-1.001,2,0>,< 1.001,3,1> }
  }

#declare char_I =
  box{ <-0.5,0,0>,<0.5,5,1> }

#declare char_J =
  union {
    difference{
      cylinder{0*z,z,2}
      cylinder{-z,2*z,1}
      plane{-y,0}
      bounded_by{ box { <-2,-2,0>, <2,0,1> } }
      translate 2*y
    }
    box{ <1,1.999,0>,<2,5,1> }
  }

#declare char_K =
  union {
    intersection {
      union {
	box{ <0,-2,0>,<0.9,5,1> rotate z*45  translate x*0.7272 }
	box{ <0,-5,0>,<0.9,2,1> rotate -z*45 translate <0.7272,5,0> }
      }
      box { <-1,0,-0.1>,<2,5,1.1> }
      bounded_by{ box { <-2,0,0>,<2,5,1> } }
    }
    box { <-2,0,0>,<-1,5,1> }
  }

#declare char_L =
   union {
     box{ <-2,1,0>,<-1,5,1> }
     box{ <-2.001,0,0>,<2,1,1>	}
   }

#declare char_M =
  union {
    intersection{
      union {
	box{<-0.5,-1,0>,<0.5,5,1> rotate  z*26.5651}
	box{<-0.5,-1,0>,<0.5,5,1> rotate -z*26.5651}
      }
      box{<-1,0,-0.1>,<1,3,1.1>}
      bounded_by{box{<-2,0,0>,<2,4,1>}}
      translate y*2
    }
    box{ <-2,0,0>,<-1,5,1> }
    box{ < 1,0,0>,< 2,5,1> }
  }

#declare char_N =
  union {
    intersection{
      box{ <0,0,0>,<1,6,1> rotate z*32.5031 translate x}
      box{ <-1,0,-0.1>,<1,5,1.1> }
      bounded_by{box{ <-1,0,0>,<1,5,1> } }
    }
    box{ <-2,0,0>,<-1,5,1> }
    box{ <1,0,0>,<2,5,1> }
  }

#declare char_O =
  union {
    object {char_C}
    box{ <1,1.999,0>,<2,3.001,1> }
  }

#declare char_Q =
  union {
    object {char_O}
    box{ <0,0,0>,<1,2.2,1> rotate z*45 translate x*1.29289}
  }

#declare char_R =
  union {
    object {char_P}
    intersection {
      box{ <0,-2,0>,<1,5,1> rotate z*36.8699 translate x}
      box{ <-1,0,-0.1>,<2,2,1.1> }
      bounded_by{box{ <-1,0,0>,<2,2,1> }}
    }
  }

#declare char_S =
  union {
    intersection{
      cylinder{0*z,z,1.5}
      cylinder{-z,2*z,0.5 inverse}
      box{<0,0,-0.1>,<1.5,1.5,1.1>}
      bounded_by{ box { <0,0,0>, <1.5, 1.5, 1> } }
      translate <0.5,3.5,0>
    }
    difference{
      cylinder{0*z,z,1.5}
      cylinder{-z,2*z,0.5}
      plane{x,0}
      bounded_by{ box { <0,-1.5,0>, <1.5, 1.5, 1> } }
      translate<0.5, 1.5, 0>
    }
    intersection{
      cylinder{0*z,z,1.5}
      cylinder{-z,2*z,0.5 inverse}
      box{<-1.5,-1.5,-0.1>,<0,0,1.1>}
      bounded_by{ box { <-1.5,-1.5,0>, <0, 0, 1> } }
      translate <-0.5,1.5,0>
    }
    difference{
      cylinder{0*z,z,1.5}
      cylinder{-z,2*z,0.5}
      plane{-x,0}
      bounded_by{ box { <-1.5,-1.5,0>, <0, 1.5, 1> } }
      translate<-0.5, 3.5, 0>
    }
    box{ <-0.502,4,0>,<0.502,5,1> }
    box{ <-0.502,2,0>,<0.502,3,1> }
    box{ <-0.502,0,0>,<0.502,1,1> }
  }

#declare char_T =
  union{
    box{ <-0.5,0,0>,<0.5,4,1> }
    box{ <-2,  4,0>,<  2,5,1> }
  }

#declare char_U =
  union {
    object{char_J}
    box{ <-2,1.998,0>,<-1,5,1> }
  }

#declare char_V =
  intersection {
    box{<-2,0,-0.1>,<2,5,1.1>}
    union {
      box{<-0.5,-1,0>,<0.5,6,1> rotate	z*16.699 }
      box{<-0.5,-1,0>,<0.5,6,1> rotate -z*16.699 }
    }
   bounded_by{ box { <-2,0,0>,<2,5,1> } }
 }

#declare char_W =
  union {
    intersection{
      union {
	box{<-0.5,-5,0>,<0.5,0,1> rotate  z*26.5651}
	box{<-0.5,-5,0>,<0.5,0,1> rotate -z*26.5651}
	translate 3*y
      }
      box{<-1,0,-0.1>,<1,3,1.1>}
      bounded_by{box{<-1,0,0>,<2,3,1>}}
    }
    box{ <-2,0,0>,<-1,5,1> }
    box{ < 1,0,0>,< 2,5,1> }
  }

#declare char_X =
  intersection {
    box{<-2,0,-0.1>,<2,5,1.1>}
    union {
      box{<-0.5,-3.5,0>,<0.5,3.5,1> rotate  z*30.9}
      box{<-0.5,-3.5,0>,<0.5,3.5,1> rotate -z*30.9}
      translate 2.5*y
    }
    bounded_by{ box { <-2,0,0>,<2,5,1> } }
 }

#declare char_Y =
  union {
    intersection {
      box{<-2,2,-0.1>,<2,5,1.1>}
      union {
	box{<-0.5,0,0>,<0.5,3.5,1> rotate  z*30.9}
	box{<-0.5,0,0>,<0.5,3.5,1> rotate -z*30.9}
	translate 2.5*y
      }
      bounded_by{ box { <-2,2,0>,<2,5,1> } }
    }
    box {<-0.5,0,0>,<0.5,3,1>}
 }

#declare char_Z =
  union {
    intersection{
      box{ <0,0,0>,<1,7,1> rotate z*-29.6749 translate -2*x}
      box{<-2,0,-0.1>,<2,5,1.1>}
      bounded_by{ box { <-2,0,0>,<2,5,1> } }
    }
    box{ <-2,4,0>,<1.2,5,1> }
    box{ <-1.2,0,0>,<2,1,1> }
  }

#declare char_Dash =
   box{ <-2,2,0>,<2,3,1> }

#declare char_Plus =
   union{
     box{ <-2,2,0>,<2,3,1> }
     box{ <-0.5,0.5,0>,<0.5,4.5,1> }
   }

#declare char_0 = object{char_O}

#declare char_1 =
   union{
     object{char_I}
     box{ <0,-1.25,0>,<1,0,1> rotate -z*45 translate <-0.5,5,0>}
     box{ <-1,0,0>,<1,1,1> }
   }

#declare char_2 =
  union{
    intersection{
      cylinder{0*z,z,1.5}
      cylinder{-z,2*z,0.5 inverse}
      box{<-1.5,0,-0.1>,<0,1.5,1.1>}
      bounded_by{ box { <-1.5,0,0>, <0, 1.5, 1> } }
      translate<-0.5, 3.5, 0>
    }
    intersection{
      cylinder{0*z,z,1.5}
      cylinder{-z,2*z,0.5 inverse}
      plane{-x,0 }
      plane{-y,0 rotate -z*54 }
      bounded_by{ box { <0,-1.5,0>, <1.5, 1.5, 1> } }
      translate <0.5,3.5,0>
    }
    box{<-0.501,4,0>,<0.501,5,1>}
    box{<-2,0,0>,<2,1,1>}
    box{<-0.999,0.5,0>,<3.5,1.5,1> rotate -z*(90+54) translate <0.5,3.5,0>}
  }

#declare char_3 =
  union {
    intersection{
      cylinder{0*z,z,1.5}
      cylinder{-z,2*z,0.5 inverse}
      box{<-1.5,0,-0.1>,<0,1.5,1.1>}
      bounded_by{ box { <-1.5,0,0>, <0, 1.5, 1> } }
      translate<-0.5, 3.5, 0>
    }
    difference{
      cylinder{0*z,z,1.5}
      cylinder{-z,2*z,0.5}
      plane{x,0}
      bounded_by{ box { <0,-1.5,0>, <1.5, 1.5, 1> } }
      translate<0.5, 3.5, 0>
    }
    difference{
      cylinder{0*z,z,1.5}
      cylinder{-z,2*z,0.5}
      plane{x,0}
      bounded_by{ box { <0,-1.5,0>, <1.5, 1.5, 1> } }
      translate<0.5, 1.5, 0>
    }
    intersection{
      cylinder{0*z,z,1.5}
      cylinder{-z,2*z,0.5 inverse}
      box{<-1.5,-1.5,-0.1>,<0,0,1.1>}
      bounded_by{ box { <-1.5,-1.5,0>, <0, 0, 1> } }
      translate <-0.5,1.5,0>
    }
    box{ <-0.501,4,0>,<0.501,5,1> }
    box{ <-0.5,2,0>,<0.501,3,1> }
    box{ <-0.501,0,0>,<0.501,1,1> }
  }

#declare char_4 =
  union {
    box {<0,0,0>,<1,5,1>}
    box {<-2,1,0>,<2,2,1>}
    box {<0,-1,0>,<3.6,0,1> rotate z*56.31 translate <-2,2,0>}
  }

#declare char_5 =
  union {
    difference{
      cylinder{0*z,z,1.5}
      cylinder{-z,2*z,0.5}
      plane{x,0}
      bounded_by{ box { <0,-1.5,0>, <1.5, 1.5, 1> } }
      translate<0.5, 1.5, 0>
    }
    intersection{
      cylinder{0*z,z,1.5}
      cylinder{-z,2*z,0.5 inverse}
      box{<-1.5,-1.5,-0.1>,<0,0,1.1>}
      bounded_by{ box { <-1.5,-1.5,0>, <0, 0, 1> } }
      translate <-0.5,1.5,0>
    }
    box{ <-1.001,4,0>,<2,5,1> }
    box{ <-2,2,0>,<-1,5,1> }
    box{ <-1.001,2,0>,<0.501,3,1> }
    box{ <-0.501,0,0>,<0.501,1,1> }
  }

#declare char_6 =
  union {
    intersection{
      cylinder{0*z,z,1.5}
      cylinder{-z,2*z,0.5 inverse}
      box{<0,0,-0.1>,<1.5,1.5,1.1>}
      bounded_by{ box { <0,0,0>, <1.5, 1.5, 1> } }
      translate <0.5,3.5,0>
    }
    intersection{
      cylinder{0*z,z,1.5}
      cylinder{-z,2*z,0.5 inverse}
      box{<-1.5,0,-0.1>,<0,1.5,1.1>}
      bounded_by{ box { <-1.5,0,0>, <0,1.5,1> } }
      translate<-0.5, 3.5, 0>
    }
    difference{
      cylinder{0*z,z,1.5}
      cylinder{-z,2*z,0.5}
      plane{x,0}
      bounded_by{ box { <0,-1.5,0>, <1.5, 1.5, 1> } }
      translate<0.5, 1.5, 0>
    }
    difference{
      cylinder{0*z,z,1.5}
      cylinder{-z,2*z,0.5}
      plane{-x,0}
      bounded_by{ box { <-1.5,-1.5,0>, <0, 1.5, 1> } }
      translate <-0.5,1.5,0>
    }
    box{ <-0.501,4,0>,<0.501,5,1> }
    box{ <-2,1.5,0>,<-1,3.501,1> }
    box{ <-0.501,2,0>,<0.501,3,1> }
    box{ <-0.501,0,0>,<0.501,1,1> }
  }

#declare char_7 =
  union{
    box{ <-2,  4,0>,<  2,5,1> }
    intersection {
      box{ <-1,-1,0>,<0,4.272,1> rotate -z*20.556 translate x/2}
      box{<-0.5,0,-0.1>,<2,4.5,1.1>}
      bounded_by{box{<-0.5,0,-0>,<2,4.5,1>}}
    }
  }

#declare char_8 =
  union {
    difference{
      cylinder{0*z,z,1.5}
      cylinder{-z,2*z,0.5}
      plane{-x,0}
      bounded_by{ box { <-1.5,-1.5,0>, <0, 1.5, 1> } }
      translate<-0.5, 3.5, 0>
    }
    difference{
      cylinder{0*z,z,1.5}
      cylinder{-z,2*z,0.5}
      plane{x,0}
      bounded_by{ box { <0,-1.5,0>, <1.5, 1.5, 1> } }
      translate<0.5, 3.5, 0>
    }
    difference{
      cylinder{0*z,z,1.5}
      cylinder{-z,2*z,0.5}
      plane{x,0}
      bounded_by{ box { <0,-1.5,0>, <1.5, 1.5, 1> } }
      translate<0.5, 1.5, 0>
    }
    difference{
      cylinder{0*z,z,1.5}
      cylinder{-z,2*z,0.5}
      plane{-x,0}
      bounded_by{ box { <-1.5,-1.5,0>, <0, 1.5, 1> } }
      translate <-0.5,1.5,0>
    }
    box{ <-0.501,4,0>,<0.501,5,1> }
    box{ <-0.501,2,0>,<0.501,3,1> }
    box{ <-0.501,0,0>,<0.501,1,1> }
  }

#declare char_9 =
  object{char_6 translate -2.5*y rotate z*180 translate 2.5*y}

#declare char_ExclPt=
  union {
    cylinder {<0,4.5,0>,<0,4.5,1>,0.5}
    cylinder {<0,0.5,0>,<0,0.5,1>,0.5}
    intersection {
      box{<-0.5,-3,0>,<0.5,0,1> rotate	z*9.5}
      box{<-0.5,-3,-0.1>,<0.5,0,1.1> rotate -z*9.5 }
      bounded_by{box{<-0.5,-3,0>,<0.5,0,1> }}
      translate y*4.5
    }
  }

#declare char_AtSign =
  union {
    difference{
      cylinder{0*z,z,2}
      cylinder{-z,2*z,1.5}
      box { <0,-1.25,-0.1>, <2.5,0,1.1> }
      bounded_by{ box { <-2,-2,0>, <2,2,1> } }
      translate 2.5*y
    }
    difference{
      cylinder{0*z,z,0.75}
      cylinder{-z,2*z,0.25}
      plane{-y,0}
      bounded_by{ box { <-2,-2,0>, <2,0,1> } }
      translate <1.25,2.5,0>
    }
    difference{
      cylinder{0*z,z,1}
      cylinder{-z,2*z,0.5}
      bounded_by{ box { <-1,-1,0>, <1,1,1> } }
      translate 2.5*y
    }
  }

#declare char_Num =
  union {
    box{ <-0.5,-2,0>,<0.5,2,1> rotate -10*z translate <-1,2.5,0>}
    box{ <-0.5,-2,0>,<0.5,2,1> rotate -10*z translate < 1,2.5,0>}
    box{ <-1.5,3,0>,< 2,4,1> }
    box{ <-2,1,0>,< 1.5,2,1> }
  }

#declare char_Dol =
  union {
    difference{
      cylinder{0*z,z,1.25}
      cylinder{-z,2*z,0.25}
      plane{x,0}
      bounded_by{ box { <0,-1.25,0>, <1.25, 1.25, 1> } }
      translate<0.75, 1.75, 0>
    }
    difference{
      cylinder{0*z,z,1.25}
      cylinder{-z,2*z,0.25}
      plane{-x,0}
      bounded_by{ box { <-1.25,-1.25,0>, <0, 1.25, 1> } }
      translate<-0.75, 3.25, 0>
    }
    box{ <-0.751,3.5,0>,<2,4.5,1> }
    box{ <-0.751,2,0>,<0.751,3,1> }
    box{ <-2,0.5,0>,<0.751,1.5,1> }
    box{ <-0.5,0,0>,<0.5,5,1> }
  }

#declare char_Perc =
  union {
    difference{
      cylinder{0*z,z,1}
      cylinder{-z,2*z,0.25}
      bounded_by{ box { <-1,-1,0>, <1, 1, 1> } }
      translate<1, 1, 0>
    }
    difference{
      cylinder{0*z,z,1}
      cylinder{-z,2*z,0.25}
      bounded_by{ box { <-1,-1,0>, <1, 1, 1> } }
      translate<-1, 4, 0>
    }
    box{ <-0.25,-2.8,0>,<0.25,2.8,1> rotate -33*z translate 2.5*y}
  }

#declare char_Hat =
  intersection {
    union {
      box{ <0,0,0>,<1,3,1>  rotate -33*z translate <-2,1.5,0>}
      box{ <-1,0,0>,<0,3,1> rotate  33*z translate < 2,1.5,0>}
    }
    box {<-2.1,1.5,-0.1>,<2.1,3.75,1.1>}
    bounded_by{box {<-2.1,1.5,0>,<2.1,3.75,1>}}
  }

#declare char_Amps =
  union {
    difference{
      cylinder{0*z,z,1.25}
      cylinder{-z,2*z,0.5}
      bounded_by{ box { <-1.25,-1.25,0>, <1.25, 1.25, 1> } }
      translate <-0.25,3.75,0>
    }
    difference{
      cylinder{0*z,z,1.75}
      cylinder{-z,2*z,0.75}
      bounded_by{ box { <-1.75,-1.75,0>, <1.75, 1.75, 1> } }
      translate <-0.25,1.75,0>
    }
    difference{
      cylinder{0*z,z,1.25}
      cylinder{-z,2*z,0.5}
      plane{-x,0}
      bounded_by{ box { <-1.25,-1.25,0>, <0, 1.25, 1> } }
      translate <2,1.75,0>
    }
  }

#declare char_Astr =
  union {
    box{ <-0.5,-2,0>,<0.5,2,1> }
    box{ <-0.5,-2,0>,<0.5,2,1> rotate 60*z}
    box{ <-0.5,-2,0>,<0.5,2,1> rotate -60*z}
    translate 2.5*y
  }

#declare char_LPar =
  intersection{
    cylinder{<7,2.5,0>,<7,2.5,1>, 7.5}
    cylinder{<7,2.5,-0.1>,<7,2.5,1.1>, 6.5 inverse}
    box{<-2,0,-0.1>,<2,5,1.1>}
    bounded_by{box{<-1,0,0>,<1,5,1>}}
  }

#declare char_RPar =
  intersection{
    cylinder{<-7,2.5,0>,<-7,2.5,1>, 7.5}
    cylinder{<-7,2.5,-0.1>,<-7,2.5,1.1>, 6.5 inverse}
    box{<-2,0,-0.1>,<2,5,1.1>}
    bounded_by{box{<-1,0,0>,<1,5,1>}}
  }

#declare char_LSqu =
  union {
    box{<-1,0,0>,<0,5,1>}
    box{<-0.001,0,0>,<1,1,1>}
    box{<-0.001,4,0>,<1,5,1>}
  }

#declare char_RSqu =
  union {
    box{<0,0,0>,<1,5,1>}
    box{<-1,0,0>,<0.001,1,1>}
    box{<-1,4,0>,<0.001,5,1>}
  }


// End of CHARS.INC
#version Chars_Inc_Temp;
#end
