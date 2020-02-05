// Persistence Of Vision raytracer version 3.1 sample file.
// Density_file pattern example

box{0,1
 texture{
   pigment{rgbt 1}
 }
   interior{
     media{
       emission 1
       scattering{1,0.1}
       intervals 10
       samples 1, 15
       confidence 0.9999
       variance 1/1000
       density{
         density_file df3 "spiral.df3" interpolate 1
         color_map{
          [0    rgb 0]
          [0.1  rgb <0.5,0.5,0.7>]
          [0.5  rgb <1.0,0.5,0.7>]
          [0.7  rgb <1.0,1.0,0.7>]
          [1    rgb 1]
         }
       }
     }
   }
   hollow
   translate <-0.5,-0.5,-0.5>
   scale <1,1,0.1>
   scale 5
   rotate <60,30,0>
}

camera {direction z*2 location <0,0,-10>}


light_source {<500,500,-500> rgb 1}
