// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer version 3.5 sample file.
// Demo showing several surfaces of surface_of_revolution ... Dieter Bayer, June 1994
//
// -w320 -h240
// -w800 -h600 +a0.3
#version 3.7;
global_settings { 
  assumed_gamma 2.2 
  max_trace_level 5
}

#include "colors.inc"
#include "textures.inc"
#include "shapes.inc"

camera {
  location <40, 40, -80>
  right     x*image_width/image_height
  angle 21
  look_at <1.9, 5, 0>
}

light_source { <40, 40, -80> colour Gray40 }

light_source { <0, 50, 20> colour Gray40 }

light_source { <50, 50, -50> colour Gray40 }

light_source { <-50, 50, -50> colour Gray40 }

background { color SkyBlue }

plane { y, 0
   pigment {
      checker colour Yellow colour rgb<0.75,1,0> 
      scale 5
   }
   finish {
      ambient 0.2
      diffuse 0.8
   }
}

#declare Glas1 =
sor {
  7,
  <0.000000, 0.000000>
  <0.118143, 0.000000>
  <0.620253, 0.540084>
  <0.210970, 0.827004>
  <0.194093, 0.962025>
  <0.286920, 1.000000>
  <0.468354, 1.033755>
  open
}

#declare Glas2 =
sor {
  12,
  <.517241379, -.132625995>
  <.249336870, 0.000000>
  <.068965517, .031830239>
  <.021220159, .050397878>
  <.058355438, .347480106>
  <.132625995, .381962865>
  <.196286472, .464190981>
  <.238726790, .602122016>
  <.249336870, .721485411>
  <.233421751, .864721485>
  <.167108753, 1.000000000>
  <.084880637, 1.055702918>
  open
}


#declare Glas3 =
sor {
  18,
  <0.125628, -0.035176>
  <0.394472, 0.000000>
  <0.281407, 0.030151>
  <0.108040, 0.052764>
  <0.125628, 0.090452>
  <0.221106, 0.185930>
  <0.125628, 0.216080>
  <0.090452, 0.339196>
  <0.185930, 0.402010>
  <0.251256, 0.522613>
  <0.208543, 0.645729>
  <0.150754, 0.703518>
  <0.082915, 0.771357>
  <0.082915, 0.844221>
  <0.155779, 0.917085>
  <0.261307, 0.942211>
  <0.238693, 1.000000>
  <0.329146, 1.115578>
  open
}


#declare Glas4 =
sor {
  10,
  <0.000000, -0.062814>
  <0.062814, 0.000000>
  <0.351759, 0.311558>
  <0.125628, 0.462312>
  <0.233668, 0.575377>
  <0.163317, 0.693467>
  <0.256281, 0.786432>
  <0.165829, 0.876884>
  <0.155779, 1.000000>
  <0.311558, 1.130653>
  open
}

#declare Glas5 =
sor {
  13,
  <0.148225, 0.000000>
  <0.189979, 0.000000>
  <0.154489, 0.096033>
  <0.075157, 0.123173>
  <0.070981, 0.164927>
  <0.129436, 0.223382>
  <0.070981, 0.281837>
  <0.108559, 0.354906>
  <0.075157, 0.436326>
  <0.175365, 0.536534>
  <0.240084, 0.672234>
  <0.327766, 1.000000>
  <0.290188, 1.000000>
  open
}

#declare Glas6 =
sor {
  11,
  <0.150754, 0.000000>
  <0.150754, 0.000000>
  <0.150754, 0.155779>
  <0.150754, 0.391960>
  <0.140704, 0.449749>
  <0.082915, 0.489950>
  <0.125628, 0.530151>
  <0.140704, 0.582915>
  <0.047739, 0.919598>
  <0.047739, 1.000000>
  <0.000000, 1.035176>
  open
}

object {
  Glas1
  texture { Silver1 }
  scale <10, 10, 10>
  translate <-15, 0.001, 15>
}

object {
  Glas2
  texture { Silver2 }
  scale <8, 8, 8>
  translate <0, 0.001, 15>
}

object {
  Glas3
  texture { Silver3 }
  scale <10, 10, 10>
  translate <15, 0.001, 15>
}

object {
  Glas4
  texture { Gold_Metal }
  scale <10, 10, 10>
  translate <-15, 0.001, 0>
}

object {
  Glas5
  texture { Copper_Metal }
  scale <10, 10, 10>
  translate <0, 0.001, 0>
}

object {
  Glas6
  texture { Bronze_Metal }
  scale <15, 15, 15>
  translate <15, 0.001, 0>
}

