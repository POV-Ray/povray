// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence of Vision Raytracer Scene Description File
// File: sample.inc
// Vers: 3.5
// Desc: general demonstration scene for materials
// Date: July/August 2001
// Auth: Christoph Hormann

#include "colors.inc"
#include "shapes.inc"

#ifndef (Type)
  #declare Type=0;
#end

#ifndef (Scale)
  #declare Scale=1;
#end

#ifndef (Obj_Nbr)
  #declare Obj_Nbr=0;
#end

#ifndef (SpaceY)
  #declare SpaceY=0.8;
#end

#ifndef (Obj_Nbr)
  #declare Obj_Nbr=0;
#end

#ifndef (SpaceX)
  #declare SpaceX=2;
#end

#ifndef (SpaceZ)
  #declare SpaceZ=2;
#end


#ifndef (Plane_Pigment)
  #declare Plane_Pigment=
    pigment {
      color rgb 0.1   
    }
#end

#switch (Type)
  #case (1)

    light_source {
      <5, 10, 10>*1000
      color rgb 1.0
    }

    camera {
      location <0, 75, 0>
      direction z
      right (4/3)*x
      look_at 0
      angle 22
    }

    plane { y, 0
      texture {
        pigment { Plane_Pigment }
        finish { ambient 0.8 diffuse 0.0 }
      }
    }

  #break

  #case (2)

    global_settings {
      assumed_gamma 1
      max_trace_level 25
      photons {
        spacing 0.05
        autostop 0
        jitter 0
        max_trace_level 3
      }
    }

    light_source {
      <5, 10, 10>*1000
      color rgb 1.1
      photons {reflection on refraction on}
    }

    camera {
      location <0, 75, 0>
      direction z
      right (4/3)*x
      look_at 0
      angle 22
    }

    plane { y, 0
      texture {
        pigment { Plane_Pigment }
        finish { ambient 0.2 diffuse 1.0 }
      }
    }

  #break

  #case (3)

    light_source {
      <10, 10, -5>*1000
      color rgb 1.1
    }

    camera {
      location <0, 40, -60>
      direction z
      right (4/3)*x
      look_at 1*y
      angle 28
    }

    plane { y, 0
      texture {
        pigment { Plane_Pigment }
        finish { ambient 0.15 diffuse 0.8 }
      }
    }

  #break

  #case (4)

    global_settings {
      assumed_gamma 1
      max_trace_level 25
      photons {
        spacing 0.02
        autostop 0
        jitter 0
        max_trace_level 3
      }
    }

    light_source {
      <10, 10, -5>*1000
      color rgb 1.1
      photons {reflection on refraction on}
    }

    camera {
      location <0, 40, -60>
      direction z
      right (4/3)*x
      look_at 1*y
      angle 28
    }

    plane { y, 0
      texture {
        pigment { Plane_Pigment }
        finish { ambient 0.2 diffuse 0.8 }
      }
    }

  #break

  #case (5)

    light_source {
      <10, 10, -5>*1000
      color rgb 1.1
    }

    camera {
      location <0, 50, -60>
      direction z
      right (4/3)*x
      look_at 5*y
      angle 28
    }

    plane { y, 0
      texture {
        pigment { Plane_Pigment }
        finish { ambient 0.15 diffuse 0.8 }
      }
    }

  #break

  #case (6)

    global_settings {
      assumed_gamma 1
      max_trace_level 25
      photons {
        spacing 0.02
        autostop 0
        jitter 0
        max_trace_level 3
      }
    }

    light_source {
      <10, 10, -5>*1000
      color rgb 1.1
    }

    camera {
      location <0, 50, -60>
      direction z
      right (4/3)*x
      look_at 5*y
      angle 28
    }

    plane { y, 0
      texture {
        pigment { Plane_Pigment }
        finish { ambient 0.15 diffuse 0.8 }
      }
    }

  #break

  #case (7)

    light_source {
      <10, 10, -5>*1000
      color rgb 1.1
    }

    camera {
      location <-30, 55, -60>
      direction z
      right (4/3)*x
      look_at 5*y
      angle 28
    }

    plane { y, 0
      texture {
        pigment { Plane_Pigment }
        finish { ambient 0.15 diffuse 0.8 }
      }
    }

  #break

  #case (8)

    global_settings {
      assumed_gamma 1
      max_trace_level 25
      photons {
        spacing 0.02
        autostop 0
        jitter 0
        max_trace_level 3
      }
    }

    light_source {
      <10, 10, -5>*1000
      color rgb 1.1
    }

    camera {
      location <-30, 55, -60>
      direction z
      right (4/3)*x
      look_at 5*y
      angle 28
    }

    plane { y, 0
      texture {
        pigment { Plane_Pigment }
        finish { ambient 0.15 diffuse 0.8 }
      }
    }

  #break

  #case (9)

    light_source {
      <10, 10, -5>*1000
      color rgb 1.3
    }

    camera {
      location <30, 55, -60>
      direction z
      right (4/3)*x
      look_at 5*y
      angle 28
    }

    plane { y, 0
      texture {
        pigment { Plane_Pigment }
        finish { ambient 0.15 diffuse 0.8 }
      }
    }

  #break

  #case (10)

    global_settings {
      assumed_gamma 1
      max_trace_level 25
      photons {
        spacing 0.02
        autostop 0
        jitter 0
        max_trace_level 3
      }
    }

    light_source {
      <10, 10, -5>*1000
      color rgb 1.3
    }

    camera {
      location <30, 55, -60>
      direction z
      right (4/3)*x
      look_at 5*y
      angle 28
    }

    plane { y, 0
      texture {
        pigment { Plane_Pigment }
        finish { ambient 0.15 diffuse 0.8 }
      }
    }

  #break

  #case (11)

    light_source {
      <-10, 10, 10>*1000
      color rgb 1.3
    }

    camera {
      orthographic
      location 20*y
      up (MaxZ)*SpaceZ*z
      right (MaxX)*SpaceX*x
      look_at 0*y
    }

    plane { y, 0
      texture {
        pigment { Plane_Pigment }
        finish { ambient 0.1 diffuse 0.8 }
      }
    }

  #break

  #else

    light_source { <5, 10, 10>*1000 color rgb 1.2 }

    camera {
      location <0, 75, 0>
      direction z
      right (4/3)*x
      look_at 0*y
      angle 22
    }

    plane { y, 0
      texture {
        pigment { Plane_Pigment }
        finish { ambient 0.1 diffuse 0.8 }
      }
    }

#end

sky_sphere {
  pigment {
    gradient y
    color_map {
      [0 color rgb 0.5 ]
      [1 color rgb 0.8 ]
    }
  }
}

#if (Obj_Nbr > 4)
  #declare Obj_Nbr=0;
#end

#declare ObjArr = array[5]

#declare ObjArr[0]=object { sphere{<0, 1, 0>, 1} }

#declare ObjArr[1]=
merge {
   sphere{<0, 5, 0>, 1}
   cone { y*1.9,1, y*4.1, 0.5 }
   box { <-1.1, 0, -1.1>, <1.1, 2, 1.1> } 
   translate 0.01*y
}

#declare ObjArr[2]=object { torus { 0.8, 0.2 } }

#declare ObjArr[3]=
object {
  box { <-1, -1, -1>, <1, 1, 1> }
}

#declare ObjArr[4]=
object {
  Round_Box_Merge ( <-1, -1, -1>, <1, 1, 1>, 0.08 )
}

#ifndef (Stack)
  #declare Stack = object { ObjArr[Obj_Nbr] }
#end


#if ((Type=5) | (Type=6) | (Type=7) | (Type=8) | (Type=9) | (Type=10))
  union {
    #declare CnZ=0;
    #while (CnZ < MaxZ)
      box {
        <-(MaxX/2 + 0.4)*SpaceX, -20, -SpaceZ*0.5>,
        < (MaxX/2 + 0.4)*SpaceX,   0,  SpaceZ*0.5>
        translate <0, (MaxZ-CnZ)*SpaceY, -(CnZ-(MaxZ-1)/2)*SpaceZ>

      }
      #declare CnZ=CnZ+1;
    #end

    scale Scale

    texture {
      pigment { color rgb 0.8 }
      finish { ambient 0.05 diffuse 0.8 }
    }

  }
#end


union {
  #declare CnZ=0;
  #while (CnZ < MaxZ)
    #if (CnZ*MaxX < MaxCnt)
      union {
        #declare CnX=0;
        #while (CnX < MaxX)
          #if (CnX+CnZ*MaxX < MaxCnt)
            object {
              Stack
              material{MArr[CnX+CnZ*MaxX]}
              translate <-(CnX-(MaxX-1)/2)*SpaceX, 0, -(CnZ-(MaxZ-1)/2)*SpaceZ>

              #switch (Type)
                #case (1)
                  no_shadow
                #break
                #case (2)
                  photons { target reflection on refraction on }
                #break
                #case (4)
                  photons { target reflection on refraction on }
                #break
                #case (5)
                  translate (MaxZ-CnZ)*SpaceY*y
                #break
                #case (6)
                  photons { target reflection on refraction on }
                  translate (MaxZ-CnZ)*SpaceY*y
                #break
                #case (7)
                  translate (MaxZ-CnZ)*SpaceY*y
                #break
                #case (8)
                  photons { target reflection on refraction on }
                  translate (MaxZ-CnZ)*SpaceY*y
                #break
                #case (9)
                  translate (MaxZ-CnZ)*SpaceY*y
                #break
                #case (10)
                  photons { target reflection on refraction on }
                  translate (MaxZ-CnZ)*SpaceY*y
                #break
              #end
            }
          #end
          #declare CnX=CnX+1;
        #end
      }
    #end
    #declare CnZ=CnZ+1;
  #end

  scale Scale
}