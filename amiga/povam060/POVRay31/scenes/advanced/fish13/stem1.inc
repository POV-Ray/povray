// Persistence Of Vision raytracer version 3.1 sample file.

//--------------------- Spiral Twist or SexWorms

#declare Stem1_Inc_Temp = version;
#version 1.0

#declare ccc = texture {
   marble
   turbulence 0.4
   colour_map {
      [0.000 0.210   colour red  0.333  green  0.467  blue  0.404  alpha  0.000
                     colour red  0.341  green  0.302  blue  0.039  alpha  0.000]
      [0.210 0.555   colour red  0.341  green  0.302  blue  0.039  alpha  0.000
                     colour red  0.584  green  0.388  blue  0.373  alpha  0.000]
      [0.555 0.773   colour red  0.584  green  0.388  blue  0.373  alpha  0.000
                     colour red  0.816  green  0.647  blue  0.518  alpha  0.000]
      [0.773 1.001   colour red  0.816  green  0.647  blue  0.518  alpha  0.000
                     colour red  0.333  green  0.467  blue  0.404  alpha  0.000]

   }
   ambient 0.2 diffuse 0.5 phong 1.0 phong_size 80
}

#declare YourTexture =    /* change to your preference */
texture {
   ccc scale <10 20 10>
}

#declare Dot = sphere { 0, 1}
#declare Connector_1 = cylinder { -x, x, 1 }

// Infinite cone in Y axis, rotated to X axis
#declare Connector_2 =
quadric {
   < 1.0 -1.0  1.0>
   < 0.0  0.0  0.0>
   < 0.0  0.0  0.0>
   0.0
   rotate < 0.0  0.0  90.0>
}

/*----------------------------------------------------------------------------*/

#declare YourThing1 =
union {
   object {
      sphere { < 0.000000  0.000000 -5.000000>  2.950000  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { <-1.018770  0.666670 -4.792920>  2.900000  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { <-1.952340  1.333330 -4.385020>  2.850000  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { <-2.762590  2.000000 -3.802380>  2.800000  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { <-3.418470  2.666670 -3.078000>  2.750000  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { <-3.897110  3.333330 -2.250000>  2.700000  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { <-4.184650  4.000000 -1.359670>  2.650000  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { <-4.276440  4.666670 -0.449470>  2.600000  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { <-4.176990  5.333330  0.439020>  2.550000  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { <-3.899330  6.000000  1.266970>  2.500000  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { <-3.464100  6.666670  2.000000>  2.450000  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { <-2.898270  7.333330  2.609610>  2.400000  }
      texture {
         YourTexture
      }
   }
   /*
   Fudge factor:  10.000000 percent 
   Maximum x coordinate:  2.950000
           y coordinate:  9.733330
           z coordinate:  5.009610
   Minimum x coordinate: -6.876440
           y coordinate: -2.950000
           z coordinate: -7.950000
   Center  x coordinate: -1.963220
           y coordinate:  3.391665
           z coordinate: -1.470195
*/
}
#declare YourThing2 =
union {
   object {
      sphere { <-2.233580  8.000000  3.074270>  2.350000  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { <-1.504930  8.666670  3.380120>  2.300000  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { <-0.748480  9.333330  3.521330>  2.250000  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { < 0.000000  10.000000  3.500000>  2.200000  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { < 0.706900  10.666670  3.325700>  2.150000  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { < 1.342230  11.333330  3.014700>  2.100000  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { < 1.880910  12.000000  2.588860>  2.050000  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { < 2.303750  12.666670  2.074310>  2.000000  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { < 2.598080  13.333330  1.500000>  1.950000  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { < 2.758070  14.000000  0.896150>  1.900000  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { < 2.784660  14.666670  0.292680>  1.850000  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { < 2.685210  15.333340 -0.282230>  1.800000  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { < 2.472750  16.000000 -0.803450>  1.750000  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { < 2.165070  16.666670 -1.250000>  1.700000  }
      texture {
         YourTexture
      }
   }
   /*
   Fudge factor:  10.000000 percent 
   Maximum x coordinate:  4.658070
           y coordinate:  18.366670
           z coordinate:  5.771330
   Minimum x coordinate: -4.583580
           y coordinate:  0.000000
           z coordinate: -2.950000
   Center  x coordinate:  0.037245
           y coordinate:  9.183335
           z coordinate:  1.410665
*/
}
#declare YourThing3 =
union {
   object {
      sphere { < 1.783550  17.333330 -1.605920>  1.650000  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { < 1.351910  18.000000 -1.860740>  1.600000  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { < 0.894820  18.666670 -2.009800>  1.550000  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { < 0.436610  19.333330 -2.054110>  1.500000  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { <-0.000000  20.000000 -2.000000>  1.450000  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { < 0.000000  20.000000 -2.000000>  1.418330  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { <-0.401960  20.666670 -1.891090>  1.386670  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { <-0.759240  21.333330 -1.705280>  1.355000  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { <-1.058010  22.000000 -1.456230>  1.323330  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { <-1.288120  22.666670 -1.159830>  1.291670  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { <-1.443380  23.333330 -0.833330>  1.260000  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { <-1.521690  24.000000 -0.494430>  1.228330  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { <-1.524930  24.666670 -0.160280>  1.196670  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { <-1.458630  25.333330  0.153310>  1.165000  }
      texture {
         YourTexture
      }
   }
   /*
   Fudge factor:  10.000000 percent 
   Maximum x coordinate:  3.433550
           y coordinate:  26.498330
           z coordinate:  1.318310
   Minimum x coordinate: -2.750020
           y coordinate:  0.000000
           z coordinate: -3.559800
   Center  x coordinate:  0.341765
           y coordinate:  13.249165
           z coordinate: -1.120745
*/
}
#declare YourThing4 =
union {
   object {
      sphere { <-1.331480  26.000000  0.432620>  1.133330  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { <-1.154700  26.666670  0.666670>  1.101670  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { <-0.941320  27.333330  0.847570>  1.070000  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { <-0.705340  28.000000  0.970820>  1.038330  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { <-0.460970  28.666670  1.035350>  1.006670  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { <-0.221770  29.333330  1.043360>  0.975000  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { < 0.000000  30.000000  1.000000>  0.943330  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { < 0.194050  30.666670  0.912940>  0.911670  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { < 0.352500  31.333330  0.791740>  0.880000  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { < 0.470230  32.000000  0.647210>  0.848330  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { < 0.544970  32.666670  0.490700>  0.816670  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { < 0.577350  33.333340  0.333330>  0.785000  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { < 0.570630  34.000000  0.185410>  0.753330  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { < 0.530410  34.666670  0.055750>  0.721670  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { < 0.464110  35.333340 -0.048780>  0.690000  }
      texture {
         YourTexture
      }
   }
   /*
   Fudge factor:  10.000000 percent 
   Maximum x coordinate:  1.362350
           y coordinate:  36.023340
           z coordinate:  2.042020
   Minimum x coordinate: -2.464810
           y coordinate:  0.000000
           z coordinate: -0.738780
   Center  x coordinate: -0.551230
           y coordinate:  18.011670
           z coordinate:  0.651620
*/
}
#declare YourThing5 =
union {
   object {
      sphere { < 0.380420  36.000000 -0.123610>  0.658330  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { < 0.288670  36.666670 -0.166670>  0.626670  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { < 0.198170  37.333340 -0.178430>  0.595000  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { < 0.117560  38.000000 -0.161800>  0.563330  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { < 0.054230  38.666670 -0.121810>  0.531670  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { < 0.013860  39.333340 -0.065210>  0.500000  }
      texture {
         YourTexture
      }
   }
   object {
      sphere { < 0.000000  40.000000  0.000000>  0.468330  }
      texture {
         YourTexture
      }
   }
   /*
   Fudge factor:  10.000000 percent 
   Maximum x coordinate:  1.038750
           y coordinate:  40.468330
           z coordinate:  0.534720
   Minimum x coordinate: -0.486140
           y coordinate:  0.000000
           z coordinate: -0.793340
   Center  x coordinate:  0.276305
           y coordinate:  20.234165
           z coordinate: -0.129310
*/
}

#declare stem1 = 
union {
   object { YourThing1  }
   object { YourThing2  }
   object { YourThing3  }
   object { YourThing4  }
   object { YourThing5  }
   /*
   Fudge factor:  10.000000 percent
   Maximum x coordinate:  4.658070
           y coordinate:  40.468330
           z coordinate:  5.771330
   Minimum x coordinate: -6.876440
           y coordinate: -2.950000
           z coordinate: -7.950000
   Center  x coordinate: -1.109185
           y coordinate:  18.759165
           z coordinate: -1.089335
*/
}

#version Stem1_Inc_Temp;
