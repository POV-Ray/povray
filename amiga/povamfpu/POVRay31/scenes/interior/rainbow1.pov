// Persistence Of Vision raytracer version 3.1 sample file.
// File by Dieter Bayer.


global_settings { assumed_gamma 2.2 }

#include "colors.inc"

camera {
   location  <0, 20, -100>
   direction <0,  0,    0.7>
   up        <0,  1,    0>
   right   <4/3,  0,    0>
}

background { color SkyBlue }

// declare rainbow's colours

#declare r_violet1 = colour red 1.0 green 0.5 blue 1.0 filter 1.0
#declare r_violet2 = colour red 1.0 green 0.5 blue 1.0 filter 0.8
#declare r_indigo  = colour red 0.5 green 0.5 blue 1.0 filter 0.8
#declare r_blue    = colour red 0.2 green 0.2 blue 1.0 filter 0.8
#declare r_cyan    = colour red 0.2 green 1.0 blue 1.0 filter 0.8
#declare r_green   = colour red 0.2 green 1.0 blue 0.2 filter 0.8
#declare r_yellow  = colour red 1.0 green 1.0 blue 0.2 filter 0.8
#declare r_orange  = colour red 1.0 green 0.5 blue 0.2 filter 0.8
#declare r_red1    = colour red 1.0 green 0.2 blue 0.2 filter 0.8
#declare r_red2    = colour red 1.0 green 0.2 blue 0.2 filter 1.0

// create the rainbow

rainbow {
  angle 42.5
  width 5
  distance 1.0e7
  direction <-0.2, -0.2, 1>
  jitter 0.01
  colour_map {
    [0.000  colour r_violet1]
    [0.100  colour r_violet2]
    [0.214  colour r_indigo]
    [0.328  colour r_blue]
    [0.442  colour r_cyan]
    [0.556  colour r_green]
    [0.670  colour r_yellow]
    [0.784  colour r_orange]
    [0.900  colour r_red1]
  }
}

rainbow {
  angle 37 
  width 5
  distance 1.0e7
  direction <-0.2, -0.2, 1>
  jitter 0.01
  colour_map {
    [0.000  colour r_violet1]
    [0.100  colour r_violet2]
    [0.214  colour r_indigo]
    [0.328  colour r_blue]
    [0.442  colour r_cyan]
    [0.556  colour r_green]
    [0.670  colour r_yellow]
    [0.784  colour r_orange]
    [0.900  colour r_red1]
  }
}

sky_sphere {
  pigment {
    gradient y
    color_map {
      [0 colour SkyBlue]
      [1 colour MidnightBlue]
    }
    scale 2
    translate <-1, -1, -1>
  }
}

/* Put down the beloved famous raytrace green/yellow checkered floor */
plane { y, -10
   pigment {
      checker colour Yellow colour Green
      scale 20
   }
   finish {
      ambient 0.2
      diffuse 0.8
   }
}

sphere { <0, 25, 0>, 40

   pigment {Red}
   finish {
      ambient 0.2
      diffuse 0.6
      phong 1.0
      phong_size 20
   }
}

sphere { <-100, 150, 200>,  20
   pigment {Magenta}
   finish {
      ambient 0.2
      diffuse 0.6
      phong 1.0
      phong_size 20
   }
}

sphere { <100, 25, 100>, 30
   pigment {Red}
   finish {
      ambient 0.2
      diffuse 0.6
      phong 1.0
      phong_size 20
   }
}

light_source {<100, 120, 40> colour White}
