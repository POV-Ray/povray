// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// Used by pigment/normal pattern example scenes

// #version 3.7;

//global_settings {
//  assumed_gamma 1.0
// }

camera {
  location <0,1,-14>
  right     x*image_width/image_height
  angle 25  
}

plane {
  y, -1.01
  pigment { checker color rgb<1,1,1>*1.1 , color rgb<1,1,1>*0 }
}

plane {
  z, 3.01
  hollow on
  pigment {White*1.1 }
}

light_source { <300, 200, -700> color rgb<1,1,1>*1.00 }
//light_source { <-50,  10, -500> color rgb<1,1,1>*0.75 }

sphere {0,1
  texture{T1}
  translate <-2, 2, 0>
}

sphere {0,1
  texture{T2}
  translate -2*x
}

cylinder{
  -z,z,1
  scale .75
  texture{T1}
  rotate <-30,30,0>
  translate 2*y
}

cylinder{
  -z,z,1
  scale .75
  texture{T2}
  rotate <-30,30,0>
}

box{<-1,-1,-1>,<1,1,1>
  scale .75
  texture{T1}
  rotate <-30,30,0>
  translate <2,2,0>
}

box{<-1,-1,-1>,<1,1,1>
  scale .75
  texture{T2}
  rotate <-30,30,0>
  translate 2*x
}

