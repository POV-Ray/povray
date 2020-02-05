#include "colors.inc"
#include "woods.inc"

#declare IOR = 1.45;
#declare Fade_Distance = 2;
#declare Fade_Power = 3;

#declare Texture01 = texture {
   pigment {
     color rgbf <1, 1, 1, 1>
   }
   finish {
      diffuse 0.000001
      metallic on
      ambient 0
      reflection 0.05
      specular 1
      roughness 0.001
      irid {
         0.65             // contribution to overall color
         thickness 0.8    // affects frequency, or "busy-ness"
         turbulence 0.1   // Variance in film thickness
      }
   }
}

#declare Interior01 =
   interior {
      fade_distance Fade_Distance
      fade_power Fade_Power
      ior IOR
      caustics 1.0
   }      


#declare Texture02a = texture {
    T_Wood1
    scale 2
    rotate x*90
    translate x*5
    finish {
        ambient 0.4
    }
}
#declare Texture02 = texture {
   pigment {
     color rgb<0.800, 0.800, 0.800>
   }
   finish {
     brilliance 0.5
     metallic on
     diffuse 0.200
     ambient 0.000
     specular 0.300
     roughness 0.02
   }

}

#declare Texture03 = texture { Texture01 }

camera {  //  Camera StdCam
  angle 90
  location  <3.50, -15.00, 3.00>
  direction <0.0,     0.0,  1.6542>
  sky       <0.0,     0.0,  1.0>  // Use right handed-system!
  up        <0.0,     0.0,  1.0>  // Where Z is up
  right     <1.3333,  0.0,  0.0>
  look_at   <0.000, 0.000, -2.7500>
}

#declare Intensity = 20;
#declare L_Fade_Distance = 20;
#declare L_Fade_Power = 2;
#declare ALL = 8;
#declare ALW = 8;
#declare ALR = 6;

#declare Area_Light=off;

light_source {   // Light1
  <-0.2, 100, 65>
  color Cyan * Intensity
#if(Area_Light)
  area_light x*ALL, z*ALW, ALR, ALR
  adaptive 1
  jitter
#end
  fade_distance L_Fade_Distance
  fade_power L_Fade_Power
}

light_source {   // Light1
  <0,  95, 65>
  color Yellow * Intensity
#if(Area_Light)
  area_light x*ALL, z*ALW, ALR, ALR
  adaptive 1
  jitter
#end
  fade_distance L_Fade_Distance
  fade_power L_Fade_Power
}

light_source {   // Light1
  <0.2,  90, 65>
  color Magenta * Intensity
#if(Area_Light)
  area_light x*ALL, z*ALW, ALR, ALR
  adaptive 1
#end
  jitter
  fade_distance L_Fade_Distance
  fade_power L_Fade_Power
}

sky_sphere {
    pigment {
        gradient y
        color_map {
           [0.0 color Gray10]
           [1.0 color Gray30]
        }
    }
}

union {
  cylinder {   <-3,0,0>, <3,0,0>, 0.3 }
  torus { 1.0, 0.25
    rotate z*90
    }
  texture {Texture01}
  interior {Interior01}
  translate  <0.0, -4.0, -0.5>
}

box { <-1, -1, -1>, <1, 1, 1>
  texture {Texture01}
  interior {Interior01}

  scale <3.0, 0.5, 0.5>
  translate  -1.75*z
  rotate x*45
  translate  -1.5*y
}

sphere { <0,0,0>,1
  texture {Texture03}
  interior {Interior01}
  translate <3, 3, -1>
}
sphere { <0,0,0>,1
  texture {Texture03}
  interior {Interior01}

  translate  <0,3.0, -0.5>
}
sphere { <0,0,0>,1
  texture {Texture03}
  interior {Interior01}
  translate  <-3.0, 3.0, -1>
}
cone { 0, 1, -2*z, 0
  texture {Texture03}
  interior {Interior01}
  translate  <-4.0, 0.3, 0>
}
cone { 0, 1, -2*z, 0
  texture {Texture03}
  interior {Interior01}
  translate  <4.0, 0.3, 0>
}

plane { z, -2
    hollow on
    pigment { Gray60 }
}
