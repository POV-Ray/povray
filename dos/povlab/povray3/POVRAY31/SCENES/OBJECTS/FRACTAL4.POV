// Persistence Of Vision raytracer version 3.1 sample file.


global_settings { assumed_gamma 2.2 }

julia_fractal {
        <-.745,0,.113,.05>
        max_iteration 9
        precision 20
        hypercomplex
	texture {  pigment { color rgb <.9,.5,.6> } 
		   finish { phong .9 phong_size 20 }
	}
        rotate <135,-30,0>
}

background { color rgb <1,1,1> }

light_source { <4,3.99,10> color rgb <.9,.95,.9> }

camera { location <0,3.0,2>
         up       <0,1.4,0>
         right    <1.4,0,0>
	 sky	  <0,0,1>
         look_at  <0,0,0>
}
