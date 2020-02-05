// Persistence Of Vision raytracer version 3.1 sample file.
//
// This scene shows the effect of a partially transparent image
// map inside a participating medium.
//

//
// The camera.
//

camera {
  location <0, 3, -10>
  look_at <0, -0.5, 0>
  angle 10
}

//
// Add media. 
//

media {
  intervals 40          
  scattering { 1, rgb 0.1}
  samples 1, 10
  confidence 0.9999
  variance 1/1000
  ratio 0.8
}

//
// A shadowless light source that does not interact with the atmosphere.
//

light_source { <100, 100, -100> color rgb 0.3 
  media_interaction off 
  shadowless 
}

//
// A spotlight pointing at the image map.
//

light_source {
  <0, 5, 0> 
  color rgb 4
  spotlight 
  point_at <0, 0, 0>
  falloff 10
  radius 8
  media_interaction on
}

//
// The partially translucent image map.
//

polygon {
  5, <-1, -1, 0>, <1, -1, 0>, <1, 1, 0>, <-1, 1, 0>, <-1, -1, 0>
  pigment { 
    image_map { 
      png "test.png" 
      once 
      transmit 5, 1 
    } 
    translate <-0.5, -0.5, 0> 
  }
  scale 2
  rotate 90*x
  hollow
}

//
// The ground. 
//

plane { y, -1 pigment { color rgb 1 } hollow }

