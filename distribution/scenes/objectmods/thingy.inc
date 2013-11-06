// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94// Persistence Of Vision raytracer sample file.

//This file contains geometry descriptions for some object modifiers
//demo scenes.

// Updated: 10Aug2008 (jh) for v3.7 distribution

//some macros first...

//box made of square rods
#macro BarBox (StartPoint,EndPoint,Thick)
#declare SmallCorner=<min(StartPoint.x,EndPoint.x),min(StartPoint.y,EndPoint.y),min(StartPoint.z,EndPoint.z)>;
#declare BigCorner=<max(StartPoint.x,EndPoint.x),max(StartPoint.y,EndPoint.y),max(StartPoint.z,EndPoint.z)>;
#declare Amplitude=BigCorner-SmallCorner;
union {
box {<0,0,0>,<Amplitude.x,Thick,Thick>}
box {<0,Amplitude.y,0>,<Amplitude.x,Amplitude.y-Thick,Thick>}
box {<0,0,Amplitude.z>,<Amplitude.x,Thick,Amplitude.z-Thick>}
box {<0,Amplitude.y,Amplitude.z>,<Amplitude.x,Amplitude.y-Thick,Amplitude.z-Thick>}

box {<0,0,0>,<Thick,Amplitude.y,Thick> translate 0}
box {<0,0,0>,<Thick,Amplitude.y,Thick> translate <Amplitude.x-Thick,0,0>}
box {<0,0,0>,<Thick,Amplitude.y,Thick> translate <0,0,Amplitude.z-Thick>}
box {<0,0,0>,<Thick,Amplitude.y,Thick> translate <Amplitude.x-Thick,0,Amplitude.z-Thick>}

box {0,<Thick,Thick,Amplitude.z>}
box {0,<Thick,Thick,Amplitude.z> translate <Amplitude.x-Thick,Amplitude.y-Thick,0>}
box {0,<Thick,Thick,Amplitude.z> translate <0,Amplitude.y-Thick,0>}
box {0,<Thick,Thick,Amplitude.z> translate <Amplitude.x-Thick,0,0>}

translate SmallCorner
}
#end

//box with round corners
#macro RoundedBox(v_SP,v_EP,R)
 union {
               #local vSP=(v_SP)*<1,1,1>;
               #local vEP=(v_EP)*<1,1,1>;
             #if (vSP.x > vEP.x)
               #local tSave=vSP.x;
               #local vSP=<vEP.x,vSP.y,vSP.z>;
               #local vEP=<tSave,vEP.y,vEP.z>;
             #end
             #if (vSP.y > vEP.y)
               #local tSave=vSP.y;
               #local vSP=<vSP.x,vEP.y,vSP.z>;
               #local vEP=<vEP.x,tSave,vEP.z>;
             #end
             #if (vSP.z > vEP.z)
               #local tSave=vSP.z;
               #local vSP=<vSP.x,vSP.y,vEP.z>;
               #local vEP=<vEP.x,vEP.y,tSave>;
             #end

               box {<vSP.x,vSP.y+R,vSP.z+R>,<vEP.x,vEP.y-R,vEP.z-R>}
               box {<vSP.x+R,vSP.y,vSP.z+R>,<vEP.x-R,vEP.y,vEP.z-R>}
               box {<vSP.x+R,vSP.y+R,vSP.z>,<vEP.x-R,vEP.y-R,vEP.z>}

               #local vSP=vSP+R;
               #local vEP=vEP-R;

               sphere { <vSP.x,vSP.y,vSP.z>,R}
               sphere { <vEP.x,vSP.y,vSP.z>,R}
               sphere { <vSP.x,vEP.y,vSP.z>,R}
               sphere { <vEP.x,vEP.y,vSP.z>,R}
               sphere { <vSP.x,vSP.y,vEP.z>,R}
               sphere { <vEP.x,vSP.y,vEP.z>,R}
               sphere { <vSP.x,vEP.y,vEP.z>,R}
               sphere { <vEP.x,vEP.y,vEP.z>,R}

               cylinder { <vSP.x,vSP.y,vSP.z>,<vEP.x,vSP.y,vSP.z>,R }
               cylinder { <vSP.x,vEP.y,vSP.z>,<vEP.x,vEP.y,vSP.z>,R }
               cylinder { <vSP.x,vSP.y,vEP.z>,<vEP.x,vSP.y,vEP.z>,R }
               cylinder { <vSP.x,vEP.y,vEP.z>,<vEP.x,vEP.y,vEP.z>,R }
               cylinder { <vSP.x,vSP.y,vSP.z>,<vSP.x,vEP.y,vSP.z>,R }
               cylinder { <vEP.x,vSP.y,vSP.z>,<vEP.x,vEP.y,vSP.z>,R }
               cylinder { <vSP.x,vSP.y,vEP.z>,<vSP.x,vEP.y,vEP.z>,R }
               cylinder { <vEP.x,vSP.y,vEP.z>,<vEP.x,vEP.y,vEP.z>,R }
               cylinder { <vSP.x,vSP.y,vSP.z>,<vSP.x,vSP.y,vEP.z>,R }
               cylinder { <vEP.x,vSP.y,vSP.z>,<vEP.x,vSP.y,vEP.z>,R }
               cylinder { <vSP.x,vEP.y,vSP.z>,<vSP.x,vEP.y,vEP.z>,R }
               cylinder { <vEP.x,vEP.y,vSP.z>,<vEP.x,vEP.y,vEP.z>,R }
               }
             #end

//pyramid
#macro Pyramid (Nbfaces,Apot,Height)
 #declare RotAngle=(360/Nbfaces);
 #declare AngHeight=degrees(atan2(Apot/Height,1));
 #declare Loop=0;
 intersection {
 #while (Loop < Nbfaces)
   plane {x,0 rotate z*AngHeight translate x*Apot rotate y*RotAngle*Loop}
 #declare Loop=Loop+1;
 #end
 plane {y,0 inverse}
  }
#end

//the thingy !
#declare Thingy =
union {
object {Pyramid (6,30,55)}

object {BarBox (<-40,0,-10>,<70,60,20>,3)}
object {RoundedBox (<10,0,-10>,<-30,80,-50>,5)}
sphere {<70,10,50>,10}
box {<-20,0,40>,<40,40,120>}
object {RoundedBox (<0,0,60>,<-30,70,75>,3)}
cylinder {<0,0,0>,<0,0,80>,1 translate <-5,81,-30>}
cylinder {<0,0,0>,<0,0,80>,1 translate <-10,81,-30>}
union {
  cone {<0,0,0>,10,<0,60,0>,1}
  torus {8,1 translate y*50}
  torus {8,1 translate y*45}
  translate <90,0,-20>
  }
}
