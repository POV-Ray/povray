// Persistence Of Vision raytracer version 3.1 sample file.
// Blackhole example, used with woodgrain pattern


global_settings { assumed_gamma 2.2 }

#include "colors.inc"
#include "textures.inc"

camera
{
  location <0,0,-15>
  direction 3*z
}

light_source { <300, 500, -500> color Gray65}
light_source { <-50,  10, -500> color Gray65}

#declare Thing = box {<-7, -3, 0>, <7, 3, 1>}

#declare Tree = pigment
{
  DMFWood4
  scale 2
  translate <1/2,0,1>
  rotate x*85
  translate 10*y
}

object
{
  Thing
  pigment
  {
    Tree
    warp
    {
      black_hole <0, 0, 0>, 0.5
      falloff 3
      strength 0.75
      inverse
      repeat <2, 1.5, 0>
      turbulence <1.0, 0.5, 0>
    }
    warp
    {
      black_hole <0.15, 0.125, 0>, 0.5
      falloff 7
      strength 1.0
      repeat <1.25, 1.25, 0>
      turbulence <0.25, 0.25, 0>
      inverse
    }
    warp
    {
      black_hole <0, 0, 0>, 1.0
      falloff 2
      strength 2
      inverse
   }
  }
}
