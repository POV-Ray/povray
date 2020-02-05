// Persistence Of Vision raytracer version 3.1 sample file.

global_settings { assumed_gamma 2.2 }

julia_fractal {
        <.49,.5,-.34,.1>
        quaternion
        max_iteration 7
        precision 400
	texture {  pigment { color rgb <.7,.9,.8> }
		   finish {phong .3 phong_size 200 }
   }
   rotate <110,50,-60>
}

background { color rgb <1,1,1> }

light_source { <2,4.2,10> color rgb <.9,.95,.9> }

camera { location <0,3.3,0>
         up       <0,0,1>
         right    <1,0,0>
	 sky	  <0,0,1>
         look_at  <0,0,0>
}
