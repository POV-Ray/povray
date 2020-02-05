// Persistence Of Vision raytracer version 3.1 sample file.
// File by Alexander Enzmann (modified by Dieter Bayer)


global_settings { assumed_gamma 2.2 }

camera {
  location  <0, 8, -10>
  right <2, 0, 0>
  look_at   <0, 0, -1>
  angle 46
}

light_source { <10, 30, -20> color red 1 green 1 blue 1 }

blob {
  threshold 0.5
  cylinder { <-7, 0, -3>, <7, 0, -3>, 0.8, 1 }
  cylinder { <-7, 0, -2>, <7, 0, -2>, 0.8, 1 }
  cylinder { <-7, 0, -1>, <7, 0, -1>, 0.8, 1 }
  cylinder { <-7, 0,  0>, <7, 0,  0>, 0.8, 1 }
  cylinder { <-7, 0,  1>, <7, 0,  1>, 0.8, 1 }
  cylinder { <-7, 0,  2>, <7, 0,  2>, 0.8, 1 }
  cylinder { <-7, 0,  3>, <7, 0,  3>, 0.8, 1 }

  sphere { <-1, 1,  1>, 1.5, 1 }
  sphere { < 1, 1,  1>, 1.5, 1 }
  sphere { <-1, 1, -1>, 1.5, 1 }
  sphere { < 1, 1, -1>, 1.5, 1 }

  pigment { color red 1 green 0 blue 0 }
  finish { ambient 0.2 diffuse 0.8 phong 1 }
}
