#version 3.7;
global_settings{ assumed_gamma 1.0 }
#default{ finish{ ambient  0 emission 0.5 diffuse 0.5 reflection 0 } }
#declare EarthRadius=6371;
#declare GeoStat=35784;
camera { 
orthographic
location -GeoStat*z
direction GeoStat*z
up EarthRadius*2.2*y
right EarthRadius*2.2*image_width/image_height*x
angle 29
}

#include "colors.inc"
#declare DEP=500;
background { Black }

#declare Basic=texture {pigment { 
spiral1 2  
pigment_map{
[0.1  Black]
[0.2  Gray10 ]
[0.3  Gray60 ]
[0.4  Gray40 ]
[0.6  Gray50 ]
[0.7  Gray40 ]
[0.89  Gray90 ]
[0.95  White ]
}
scale EarthRadius/1.3
rotate -(90-27)*x
rotate 130*y
}
}
#local MT=0.007*DEP/50;
#declare Planet = texture { pigment {
spherical 
pigment_map{
[0  White ]
[MT/10  Brown]
[MT/2  ForestGreen]
[MT  ForestGreen]
[MT  Yellow]
[MT*4/3  Blue]
[MT*2  Black]
}
scale EarthRadius+DEP

}}
#declare Source=sphere { 0, EarthRadius texture {  Basic } }
#debug concat(str(now,9,12)," start tessel\n")
#declare MSource= tessel { original Source accuracy 251 albinos  } 
#debug concat(str(now,9,12)," end tessel - start planet\n")
#local Itera=100000;
#declare Di= planet { original  MSource origin <0,0,0> amount DEP repeat Itera,23 jitter EarthRadius*2/3 }
#debug concat(str(now,9,12)," end planet - start smooth\n")

light_source { <-1,1,-1>*35786 , rgb<1,1,1> } 
light_source { -1*<-1,1,-1>*35786 , rgb<1,1,1> } 
object { Di texture { Planet } }
