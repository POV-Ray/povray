#version 3.7;
global_settings{assumed_gamma 1.0 }
#declare Test_object = julia_fractal { <0.1,0.8,-0.1,0.01> quaternion sqr max_iteration 8 precision 1000
 scale 60 translate y*60 rotate y*45
	};

light_source { < 0, 1000, 200> rgb 1 }

#macro Zorglub(BnW,Position,Radius)
object { Test_object texture { proximity { Test_object } radius Radius
	texture_map {
#if( BnW = true )
		[ 0.4  pigment { rgb 0 } finish { ambient 1.0 diffuse 0.0 } ]
		[ 1.0-0.4  pigment { rgb 1 }  finish { ambient 1.0 diffuse 0.0 }]
#else
		[ 0.0 pigment { rgb <0,169,224>/255 } ]
		[ 0.2 pigment { rgb <50,52,144>/255 } ]
		[ 0.4 pigment { rgb <234,22,136>/255 } ]
		[ 0.6 pigment { rgb <235,46,46>/255 } ]
		[ 0.8 pigment { rgb <253,233,45>/255 } ]
		[ 1.0 pigment { rgb <0,158,84>/255 } ]
#end
	}
}
translate Position
}
#end

Zorglub(true,0,4)
Zorglub(false,200*x+600*z,20)
Zorglub(true,-200*x+600*z,20)
Zorglub(false,100*x+1100*z,1)
Zorglub(false,-100*x+1100*z,10)

camera {
location  <0.0, 220.0, -500.0>
look_at   <0.0, 90,  0.0>
right     x*image_width/image_height
angle 30
}

plane { y, -1.0 pigment { checker rgb 1 rgb 0.8 scale 100 } }



