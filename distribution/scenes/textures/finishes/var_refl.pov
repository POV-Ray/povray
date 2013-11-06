// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision Ray Tracer Scene Description File
// File: var_refl.pov
// Desc: Demo of the three reflection types
// Date: 2001/04/15
// Auth: ingo
// Updated 28. Aug 01 by Christoph Hormann
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.6;

global_settings {
  assumed_gamma 1.0
}

light_source {
   < 100, 500,-500>
   rgb 1
}

camera {
   location  <0.0, 1.5, -8.0>
   right     x*image_width/image_height
   look_at   <0.0, 0.0, 20.0>
   angle 55
}

sky_sphere {
   pigment {
      function { abs(y) }
      turbulence 0.1
      color_map { [0.0, rgb <0,0,0.6>] [1.0, rgb 1] }
   }
}

#declare Water= box{
   <-1,-2,-5>,<1,0.1,20>
}

difference {
   plane {
      y, 0
      pigment {rgb <0,0.2,0>}
   }
   object{
      Water
      translate <-2.5,0,0>
      texture {
         pigment {checker color rgb 1 color blue 1 scale 0.1}
      }
   }
   object{
      Water
      texture {
         pigment {checker color rgb 1 color blue 1 scale 0.1}
      }
   }
   object{
      Water
      translate <2.5,0,0>
      texture {
         pigment {checker color rgb 1 color blue 1 scale 0.1}
      }
   }
}
difference {
   box {<-4,-0.001,-5.5>,<4,0.2,21>}
   object{
      Water
      scale <1,3,1>
      translate <-2.5,0,0>
   }
   object{
      Water
      scale <1,3,1>
   }
   object{
      Water
      scale <1,3,1>
      translate <2.5,0,0>
   }
   pigment {rgb 0.75}
}

#declare WaterNormal=normal{
    bozo 1
    normal_map {
        [ 0.3 waves translate -0.5 scale <1, 0.05, 1>*100000 frequency 100000]
        [ 0.7 ripples translate -0.5 scale <1, 0.7, 1>*100000 frequency 100000]
        [ 0.85 ripples translate -0.5 scale <1, 0.6, 1>*100000 frequency 100000]
        [ 1.0 ripples translate -0.5  scale 100000 frequency 100000]
    }
    scale 1
}

object{
   Water
   scale 1.01
   translate <-2.5,0,0>
   material{
      texture {
         pigment {rgbf <1,1,1,1>}
         normal{WaterNormal}
         finish {
            ambient 0
            diffuse 0.1
            reflection {
              0.04, 1
            }
            specular 1.5
            roughness 0.001
            brilliance 0.01
            conserve_energy
         }
      }
      interior {
         ior 1.33
      }
   }
}
object{
   Water
   scale 1.01
   material{
      texture {
         pigment {rgbf <1,1,1,1>}
         normal{WaterNormal}
         finish {
            ambient 0
            diffuse 0.1
            reflection {
              0.04, 1
              fresnel on
            }
            specular 1.5
            roughness 0.001
            brilliance 0.01
            conserve_energy
         }
      }
      interior {
         ior 1.33
      }
   }
}
object{
   Water
   scale 1.01
   translate <2.5,0,0>
   material{
      texture {
         pigment {rgbf <1,1,1,1>}
         normal{WaterNormal}
         finish {
            ambient 0
            diffuse 0.1
            reflection 1
            specular 1.5
            roughness 0.001
            brilliance 0.01
            conserve_energy
         }
      }
      interior {
         ior 1.33
      }
   }
}


#declare I=0;
#declare N=10;
#while (I<N)
   cylinder {
      <0, 0, 22>,<0,8,22>, 0.2
      translate <-9+2*I,0,0>
      pigment {rgb <1,0,0>}
   }
   #declare I=I+1;
#end
