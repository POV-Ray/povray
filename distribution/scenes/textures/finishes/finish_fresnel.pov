// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// This file demonstrates the effect of the "fresnel" keyword in the finish block.
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.8;

#declare FRESNEL = off;

global_settings {
  assumed_gamma 1.0
  radiosity {
    pretrace_start 0.1
    pretrace_end 0.001
    count 1600
    maximum_reuse 0.1
  }
}

#default{ finish{ ambient 0.1 diffuse 0.9 }} 

camera {
  perspective angle 75
  location <0, 1.5,-3>
  right    x*image_width/image_height
  look_at  <0,1,0>
}

light_source {
  <-3000,3000,-3000> color rgb 0.01
}

plane{ <0,1,0>, 0 
  pigment{ checker color rgb 1 color rgb 0 }
  finish { diffuse albedo 1 }
  rotate y*45
}

#macro SampleSphere(P,R,C)
  sphere { P+y*R,R
    pigment { color C }
    finish {
      ambient 0
      emission 1
      fresnel FRESNEL
      phong 0
      #if (FRESNEL)
        diffuse albedo 1
        specular albedo 1 roughness 0.001
        reflection FRESNEL
      #else
        diffuse albedo 0.5
        specular albedo 0.5 roughness 0.001
        reflection { 1 fresnel on }
      #end
    }
    interior { ior 1.7 }
  }
#end


SampleSphere(<0,0,0>,1,rgb<1,0.8,0.3>)
SampleSphere(<2,0,2>,1,rgb<0.3,0.8,1.0>)
SampleSphere(<-2,0,2>,1,rgb<0.3,1.0,0.3>)
