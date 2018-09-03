#version 3.7;
global_settings { assumed_gamma 1.0 }

#default{ finish{ emission 0.2 diffuse 0.8 reflection 0.0 } }

camera { omni_directional_stereo
distance .13
location 0
up 10*y
right 10*x
direction 10*z
}

light_source{ 40*y, 1 }

#include "colors.inc" 
background { White }

#declare Depth = 0.5;
#declare Obj = text{ internal 1 "+X" Depth, 0 texture { pigment { red 1 } } }
object{ Obj translate -x*max_extent(Obj).x/2-y*max_extent(Obj).y/2 
rotate 90*y
translate 4*x
}

#declare Obj = text{ internal 2 "-X" Depth, 0 texture { pigment { rgb<0,1,1> } } }
object{ Obj translate -x*max_extent(Obj).x/2-y*max_extent(Obj).y/2
rotate -90*y
translate -4*x
}

#for(i,0,359,90)

#declare Obj = text{ internal 1 "+Y" Depth, 0 texture { pigment { green 1 } } }
object{ Obj translate -x*max_extent(Obj).x/2-y*max_extent(Obj).y/2
translate 4*z
rotate -70*x
rotate i*y
}

#declare Obj = text{ internal 2 "-Y" Depth, 0 texture { pigment { rgb<1,0,1> } } }
object{ Obj translate -x*max_extent(Obj).x/2-y*max_extent(Obj).y/2
translate 4*z
rotate 70*x
rotate i*y
}
#end

#declare Obj = text{ internal 1 "+Z" Depth, 0 texture { pigment { blue 1 } } }
object{ Obj translate -x*max_extent(Obj).x/2-y*max_extent(Obj).y/2
translate 4*z
}

#declare Obj= text{ internal 2 "-Z" Depth, 0 texture { pigment { rgb<1,1,0> } } }
object{ Obj translate -x*max_extent(Obj).x/2-y*max_extent(Obj).y/2
translate 4*z
rotate 180*y
}

#declare Distance = 50;
#declare Size = 1;
#for(Longitude,0,359,15)
#for(Latitude,-75,75,15)
sphere{ <0,0,Distance>,Size texture { pigment { color rgbft CHSV2RGB(<Longitude, 1, 1>) } }
rotate -Latitude*x
rotate Longitude*y
}
cylinder{ 
Distance*<cos(radians(Longitude))*cos(radians(Latitude)),sin(radians(Latitude)),sin(radians(Longitude))*cos(radians(Latitude))>, 
Distance*<cos(radians(Longitude+15))*cos(radians(Latitude)),sin(radians(Latitude)),sin(radians(Longitude+15))*cos(radians(Latitude))>, 
Size/5
texture { pigment { color rgb 0.5 } }
}
#if ( Latitude < 75 )
cylinder{ 
Distance*<cos(radians(Longitude))*cos(radians(Latitude)),sin(radians(Latitude)),sin(radians(Longitude))*cos(radians(Latitude))>, 
Distance*<cos(radians(Longitude))*cos(radians(Latitude+15)),sin(radians(Latitude+15)),sin(radians(Longitude))*cos(radians(Latitude+15))>, 
Size/5
texture { pigment { color rgb 0.5 } }
}
#end

#end
#end
