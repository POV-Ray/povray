// Persistence Of Vision raytracer version 3.1 sample file.

//------------ world sphere { w/ Violet Clouds

#declare World12_Inc_Temp = version;


#declare Violet_Sky = texture {
   pigment {
      gradient y
      colour_map {
         [0.000 0.339   colour red  0.208  green  0.067  blue  0.235
                        colour red  0.498  green  0.165  blue  0.514]
         [0.339 0.559   colour red  0.498  green  0.165  blue  0.514
                        colour red  0.624  green  0.165  blue  0.514]
         [0.559 0.686   colour red  0.624  green  0.165  blue  0.514
                        colour red  0.835  green  0.400  blue  0.514]
         [0.686 0.983   colour red  0.835  green  0.400  blue  0.514
                        colour red  0.988  green  0.651  blue  0.631]
         [0.983 1.001   colour red  0.988  green  0.651  blue  0.631
                        colour red  0.208  green  0.067  blue  0.235]
      }
   }
}

#declare deeper12 = texture {
   pigment {
      gradient < 0 1 0 >
      colour_map {
         [0.000 0.558   colour red  0.133  green  0.008  blue  0.212  filter  0.375
                        colour red  0.471  green  0.400  blue  0.506  filter  0.9000]
         [0.558 0.983   colour red  0.471  green  0.400  blue  0.506  filter  0.9000
                        colour red  1.000  green  0.996  blue  0.996  filter  0.8000]
         [0.983 1.001   colour red  1.000  green  0.996  blue  0.996  filter  0.8000
                        colour red  0.133  green  0.008  blue  0.212  filter  0.375]
      }
   }
}

#declare cloud12 = texture {
   pigment {
      bozo
      turbulence 0.8
      colour_map {
         [0.000 0.212   colour red  1.000  green  1.000  blue  1.000  filter  1.000
                        colour red  1.000  green  1.000  blue  1.000  filter  1.000]
         [0.212 0.534   colour red  1.000  green  1.000  blue  1.000  filter  1.000
                        colour red  0.341  green  0.137  blue  0.302  filter  0.604]
         [0.534 0.627   colour red  0.341  green  0.137  blue  0.302  filter  0.604
                        colour red  0.259  green  0.196  blue  0.302  filter  0.471]
         [0.627 0.983   colour red  0.259  green  0.196  blue  0.302  filter  0.471
                        colour red  0.271  green  0.216  blue  0.365  filter  0.404]
         [0.983 1.001   colour red  0.271  green  0.216  blue  0.365  filter  0.404
                        colour red  1.000  green  1.000  blue  1.000  filter  1.000]
      }
      phase 0.5
   }
}


#declare world12 = object {
   Ellipsoid
   scale <20000 10000 20000>
   hollow on

   texture {
      Violet_Sky scale <40000 8000 400000> translate <0 5000 0>
      finish { ambient 1.0 diffuse 0.0 }
   }

   texture { cloud12 scale <20000 8000 10000> translate <20000 3000 0>
      rotate <0 0 0>  }

   texture {
      deeper12
      finish { ambient 0.2 diffuse 0.3 }
      scale <40000 10000 10000>
      translate <6000 6000 0>
      rotate <0 0 20>
   }

}

#version World12_Inc_Temp;
