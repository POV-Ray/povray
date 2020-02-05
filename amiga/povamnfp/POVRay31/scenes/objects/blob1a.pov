// Persistence Of Vision raytracer version 3.1 sample file.
// File by Alexander Enzmann (modified by Dieter Bayer)


global_settings { assumed_gamma 2.2 }

camera {
  location  <0, 8, -15>
  look_at   <0, 0, 0>
  angle 46
}

light_source { <10, 30, -20> color red 1 green 1 blue 1 }

blob {
  threshold 0.5
  cylinder { <-7, 0, 0>, <7, 0, 0>, 4, 2 }
  cylinder { <0, 0, -7>, <0, 0, 7>, 4, 2 }
  sphere { <0, 3, 0>, 2.5, -4 }

  pigment { color red 1 green 0 blue 0 }
  finish { ambient 0.2 diffuse 0.8 phong 1 }

  rotate <-30, 0, 0>
}

