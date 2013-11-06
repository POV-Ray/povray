// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer version 3.5 sample file.
//
// -w320 -h320
// -w800 -h800 +a0.3
#version 3.6;
global_settings { assumed_gamma 2.2 }
#default{ finish{ ambient 0.1 diffuse 0.9 }} 

julia_fractal {
        <.49,.5,-.34,.1>
        quaternion
        max_iteration 7
        precision 400
	texture {  pigment{ color rgb <0.6,1,0.5>*1 }
		   finish { phong 1 phong_size 400 }
   }
   rotate <110,50,-60>
}

background { color rgb <1,1,1>*1 }

light_source { <2,4.2,10> color rgb <1,0.95,0.9>*0.9 }
light_source { <-6, 4.2,-10> color rgb <1,0.90,0.55>*0.5 }
light_source { <0,3.3,0>  color rgb <0.9,0.95,1>*0.2 shadowless }

//-------------------------------------------------------------------------
// This scene uses a non-standard camera set-up. 
// (See CAMERA in the included documentation for details.) 
// If you are new to POV-Ray, you might want to try a different demo scene.
//-------------------------------------------------------------------------
camera { location <0,3.3,0>
         up       <0,0,1>
         right     x*image_width/image_height
	 sky	  <0,0,1>
         look_at  <0,0,0>
}
