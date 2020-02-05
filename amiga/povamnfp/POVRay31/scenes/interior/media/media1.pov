// Persistence Of Vision raytracer version 3.1 sample file.
//
// Participating media environment with spotlights.
//

//
// The camera.
//

camera {
  location <10, 6, -20>
  right <4/3, 0, 0>
  up <0, 1, 0>
  direction <0, 0, 1.5>
  look_at <0, 4, 0>
}

//
// Add media. 
//

media {
  intervals 10
  scattering { 1, rgb 0.03}
  samples 1, 10         
  confidence 0.9999
  variance 1/1000
  ratio 0.9
}

//
// Light source not interacting with the atmosphere. 
//

light_source { <0, 15, 0> color rgb 0.7
  media_interaction off 
  shadowless 
}

//
// Spotlights pointing at balls. 
//

#declare Intensity = 2;

light_source { 
  <-10, 15, -5> color rgb<1, .3, .3> * Intensity
  spotlight
  point_at <0, 5, 0>
  radius 10
  falloff 15
  tightness 1
  media_attenuation on
}

light_source { 
  <0, 15, -5> color rgb<.3, 1, .3> * Intensity
  spotlight
  point_at <0, 5, 0>
  radius 10
  falloff 15
  tightness 1
  media_attenuation on
}

light_source { 
  <10, 15, -5> color rgb<.3, .3, 1> * Intensity
  spotlight
  point_at <0, 5, 0>
  radius 10
  falloff 15
  tightness 1
  media_attenuation on
}

//
// Room. 
//

box { <-20, 0, -20>, <20, 20, 20>
  pigment { rgb 1 }
  finish { ambient 0.2 diffuse 0.5 }
  hollow
}

//
// Ball. 
//

sphere { <0, 5, 0>, 1
  pigment { rgb 1 }
  finish { ambient 0.3 diffuse 0.7 phong 1 }
}

