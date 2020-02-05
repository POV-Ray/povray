// Persistence Of Vision raytracer version 3.1 sample file.
// File by Alexander Enzmann

global_settings { assumed_gamma 2.2 }

camera {
   location  <0, 0, -5>
   direction <0, 0, 1.2071>
   look_at   <0, 0, 0>
}

light_source { <-15, 30, -25> color red 1 green 1 blue 1 }
light_source { < 15, 30, -25> color red 1 green 1 blue 1 }

blob {
   threshold 0.6
   component 1.0, 1.0, <0.75, 0, 0>
   component 1.0, 1.0, <-0.375, 0.64952, 0>
   component 1.0, 1.0, <-0.375, -0.64952, 0>

   pigment { color red 1 green 0 blue 0 }
   finish {
      ambient 0.2
      diffuse 0.8
      phong 1
   }
   rotate 30*y
}
