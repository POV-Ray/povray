// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer version 3.5 sample file.
//
// -w320 -h320
// -w800 -h800 +a0.3
#version 3.7;
global_settings { assumed_gamma 1.0 }

julia_fractal {
        <-.745,0,.113,.05>
        max_iteration 9
        precision 20
        hypercomplex
	texture {  pigment { color rgb <.9,.5,.6>*0.4 }
		   finish { phong .9 phong_size 20 }
	}
        scale<1,2,1>
        rotate <135,-30,0>
}

background { color rgb <1,1,1> }

light_source { <4,3.99,10> color rgb <.9,.95,.9> }
 
//-------------------------------------------------------------------------
// This scene uses a non-standard camera set-up. 
// (See CAMERA in the included documentation for details.) 
// If you are new to POV-Ray, you might want to try a different demo scene.
//-------------------------------------------------------------------------
camera { location <0,3.0,2>
         up       <0,1.4,0>
         right     x*image_width/image_height
	 sky	  <0,0,1>
         angle 50
         look_at  <0,0,0>
}
