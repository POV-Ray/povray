// Persistence Of Vision raytracer version 3.1	sample file.
// File by Alexander Enzmann

global_settings { assumed_gamma 2.2 }

camera {
   location  <0, 0, -8>
   direction <0, 0, 1.2071>
   look_at   <0, 0, 0>
}

sphere { <0.0, 0.0, 0.0>, 2
   finish {
      ambient 0.2
      diffuse 0.8
      phong 1
   }
   pigment { color red 1 green 0 blue 0 }
}

box { <-2.0, -0.2, -2.0>, <2.0, 0.2, 2.0>
    finish {
       ambient 0.2
       diffuse 0.8
    }
    pigment { color red 1 green 0 blue 1 }

    rotate <-20, 30, 0>
}

light_source { <-10, 3, -20> color red 1 green 1 blue 1 }
