// Persistence Of Vision raytracer version 3.1 sample file.
// File by Alexander Enzmann (modified by Dieter Bayer)



global_settings { assumed_gamma 2.2 }

camera {
  location  <0, 5, -5>
  look_at   <0, 0, 0>
  angle 58
}

light_source { <-20, 30, -25> color red 0.6 green 0.6 blue 0.6 }
light_source { < 20, 30, -25> color red 0.6 green 0.6 blue 0.6 }

blob {
  threshold 0.5
  sphere { <-2, 0, 0>, 1, 2 }
  cylinder { <-2, 0, 0>, <2, 0, 0>, 0.5, 1 }
  cylinder { <0, 0, -2>, <0, 0, 2>, 0.5, 1 }
  cylinder { <0, -2, 0>, <0, 2, 0>, 0.5, 1 }

  pigment { color red 1 green 0 blue 0 }
  finish { ambient 0.2 diffuse 0.8 phong 1 }

  rotate <0, 20, 0>
}
