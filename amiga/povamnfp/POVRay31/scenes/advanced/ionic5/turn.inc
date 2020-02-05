// Persistence Of Vision raytracer version 3.1 sample file.
/*--------   IONIC CAPITAL TURN -------------------------------*/

#declare Turn_Inc_Temp = version;
#version 1.0;


#declare YourTexture =

texture {  marble1 ambient 0.4 diffuse 1.0 
   scale <5 13 4>   }
texture { marble4 diffuse 1.0 phong 0.6 phong_size 20 scale <5 7 3> rotate <0 0 50>  }

/*  ***** dot declaration *****  */

// #declare Dot =
// quadric {
//    < 1.0  1.0  1.0>
//    < 0.0  0.0  0.0>
//    < 0.0  0.0  0.0>
//    -1.0
// }
#declare Dot = sphere { 0, 1 }

/*  ***** cylindrical connector *****  */

// #declare Connector_1 =
// object {
//    quadric {
//       < 0.0  1.0  1.0>
//       < 0.0  0.0  0.0>
//       < 0.0  0.0  0.0>
//       -1.0
//    }
//    clipped_by {
//       plane { < 1.0  0.0  0.0> 1.0  }
//       plane { <-1.0  0.0  0.0> 0.0  }
//    }
// }
#declare Connector_1 = cylinder { -x, x, 1 }

/*  ***** conical connector *****  */

// Infinite cone in Y axis, rotated to X axis
#declare Connector_2 =
quadric {
   < 1.0 -1.0  1.0>
   < 0.0  0.0  0.0>
   < 0.0  0.0  0.0>
   0.0
   rotate < 0.0  0.0  90.0>
}

#declare s1 =
composite {
   object {
      quadric {
         Dot
         scale     < 1.200000  1.200000  1.200000>
         translate < 4.000000  0.000000  0.000000>
      }            
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.199990  }
         plane {     < 1.000000  0.000000  0.000000>  1.194990 inverse  }
      }            
      translate <-1.199990  0.000000  0.000000>
      scale     < 248.644324  1.000000  1.000000>
      translate <-0.004826  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000 -101.846098  0.000000>
      translate < 4.000000  0.000000  0.000000>
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Dot
         scale     < 1.195000  1.195000  1.195000>
         translate < 3.744785  0.000000  1.216754>
      }            
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.194990  }
         plane {     < 1.000000  0.000000  0.000000>  1.189990 inverse  }
      }            
      translate <-1.194990  0.000000  0.000000>
      scale     < 244.738491  1.000000  1.000000>
      translate <-0.004883  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000 -119.891533  0.000000>
      translate < 3.744785  0.000000  1.216754>
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Dot
         scale     < 1.190000  1.190000  1.190000>
         translate < 3.134941  0.000000  2.277668>
      }            
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.189990  }
         plane {     < 1.000000  0.000000  0.000000>  1.184990 inverse  }
      }            
      translate <-1.189990  0.000000  0.000000>
      scale     < 240.832580  1.000000  1.000000>
      translate <-0.004941  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000 -137.938517  0.000000>
      translate < 3.134941  0.000000  2.277668>
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Dot
         scale     < 1.185000  1.185000  1.185000>
         translate < 2.240931  0.000000  3.084377>
      }            
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.184989  }
         plane {     < 1.000000  0.000000  0.000000>  1.179989 inverse  }
      }            
      translate <-1.184989  0.000000  0.000000>
      scale     < 236.926837  1.000000  1.000000>
      translate <-0.005001  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000 -155.986945  0.000000>
      translate < 2.240931  0.000000  3.084377>
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Dot
         scale     < 1.180000  1.180000  1.180000>
         translate < 1.158814  0.000000  3.566462>
      }            
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.179989  }
         plane {     < 1.000000  0.000000  0.000000>  1.174989 inverse  }
      }            
      translate <-1.179989  0.000000  0.000000>
      scale     < 233.021465  1.000000  1.000000>
      translate <-0.005064  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000 -174.037084  0.000000>
      translate < 1.158814  0.000000  3.566462>
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Dot
         scale     < 1.175000  1.175000  1.175000>
         translate <-0.000000  0.000000  3.687500>
      }            
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.174989  }
         plane {     < 1.000000  0.000000  0.000000>  1.169989 inverse  }
      }            
      translate <-1.174989  0.000000  0.000000>
      scale     < 229.116173  1.000000  1.000000>
      translate <-0.005128  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  167.911115  0.000000>
      translate <-0.000000  0.000000  3.687500>
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Dot
         scale     < 1.170000  1.170000  1.170000>
         translate <-1.120187  0.000000  3.447580>
      }            
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.169988  }
         plane {     < 1.000000  0.000000  0.000000>  1.164989 inverse  }
      }            
      translate <-1.169988  0.000000  0.000000>
      scale     < 225.210938  1.000000  1.000000>
      translate <-0.005195  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  149.857475  0.000000>
      translate <-1.120187  0.000000  3.447580>
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Dot
         scale     < 1.165000  1.165000  1.165000>
         translate <-2.093985  0.000000  2.882123>
      }            
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.164988  }
         plane {     < 1.000000  0.000000  0.000000>  1.159988 inverse  }
      }            
      translate <-1.164988  0.000000  0.000000>
      scale     < 221.306048  1.000000  1.000000>
      translate <-0.005264  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  131.801954  0.000000>
      translate <-2.093985  0.000000  2.882123>
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Dot
         scale     < 1.160000  1.160000  1.160000>
         translate <-2.831559  0.000000  2.057248>
      }            
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.159988  }
         plane {     < 1.000000  0.000000  0.000000>  1.154988 inverse  }
      }            
      translate <-1.159988  0.000000  0.000000>
      scale     < 217.401336  1.000000  1.000000>
      translate <-0.005336  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  113.744531  0.000000>
      translate <-2.831559  0.000000  2.057248>
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Dot
         scale     < 1.155000  1.155000  1.155000>
         translate <-3.269257  0.000000  1.062246>
      }            
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.154987  }
         plane {     < 1.000000  0.000000  0.000000>  1.149987 inverse  }
      }            
      translate <-1.154987  0.000000  0.000000>
      scale     < 213.496899  1.000000  1.000000>
      translate <-0.005410  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  95.684872  0.000000>
      translate <-3.269257  0.000000  1.062246>
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Dot
         scale     < 1.150000  1.150000  1.150000>
         translate <-3.375000  0.000000 -0.000000>
      }            
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.149987  }
         plane {     < 1.000000  0.000000  0.000000>  1.144987 inverse  }
      }            
      translate <-1.149987  0.000000  0.000000>
      scale     < 209.592676  1.000000  1.000000>
      translate <-0.005487  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  77.623080  0.000000>
      translate <-3.375000  0.000000 -0.000000>
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Dot
         scale     < 1.145000  1.145000  1.145000>
         translate <-3.150375  0.000000 -1.023619>
      }            
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.144986  }
         plane {     < 1.000000  0.000000  0.000000>  1.139987 inverse  }
      }            
      translate <-1.144986  0.000000  0.000000>
      scale     < 205.688666  1.000000  1.000000>
      translate <-0.005567  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  59.558860  0.000000>
      translate <-3.150375  0.000000 -1.023619>
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Dot
         scale     < 1.140000  1.140000  1.140000>
         translate <-2.629305  0.000000 -1.910302>
      }            
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.139986  }
         plane {     < 1.000000  0.000000  0.000000>  1.134986 inverse  }
      }            
      translate <-1.139986  0.000000  0.000000>
      scale     < 201.785032  1.000000  1.000000>
      translate <-0.005650  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  41.492248  0.000000>
      translate <-2.629305  0.000000 -1.910302>
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Dot
         scale     < 1.135000  1.135000  1.135000>
         translate <-1.873565  0.000000 -2.578742>
      }            
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.134986  }
         plane {     < 1.000000  0.000000  0.000000>  1.129986 inverse  }
      }            
      translate <-1.134986  0.000000  0.000000>
      scale     < 197.881435  1.000000  1.000000>
      translate <-0.005736  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  23.422970  0.000000>
      translate <-1.873565  0.000000 -2.578742>
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Dot
         scale     < 1.130000  1.130000  1.130000>
         translate <-0.965678  0.000000 -2.972052>
      }            
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.129985  }
         plane {     < 1.000000  0.000000  0.000000>  1.124985 inverse  }
      }            
      translate <-1.129985  0.000000  0.000000>
      scale     < 193.978333  1.000000  1.000000>
      translate <-0.005825  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  5.350866  0.000000>
      translate <-0.965678  0.000000 -2.972052>
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Dot
         scale     < 1.125000  1.125000  1.125000>
         translate < 0.000000  0.000000 -3.062500>
      }            
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.124984  }
         plane {     < 1.000000  0.000000  0.000000>  1.119985 inverse  }
      }            
      translate <-1.124984  0.000000  0.000000>
      scale     < 190.075522  1.000000  1.000000>
      translate <-0.005919  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000 -12.724115  0.000000>
      translate < 0.000000  0.000000 -3.062500>
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Dot
         scale     < 1.120000  1.120000  1.120000>
         translate < 0.927051  0.000000 -2.853170>
      }            
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.119984  }
         plane {     < 1.000000  0.000000  0.000000>  1.114984 inverse  }
      }            
      translate <-1.119984  0.000000  0.000000>
      scale     < 186.173110  1.000000  1.000000>
      translate <-0.006016  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000 -30.802369  0.000000>
      translate < 0.927051  0.000000 -2.853170>
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Dot
         scale     < 1.115000  1.115000  1.115000>
         translate < 1.726619  0.000000 -2.376487>
      }            
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.114983  }
         plane {     < 1.000000  0.000000  0.000000>  1.109983 inverse  }
      }            
      translate <-1.114983  0.000000  0.000000>
      scale     < 182.270846  1.000000  1.000000>
      translate <-0.006117  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000 -48.883790  0.000000>
      translate < 1.726619  0.000000 -2.376487>
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Dot
         scale     < 1.110000  1.110000  1.110000>
         translate < 2.325924  0.000000 -1.689883>
      }            
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.109983  }
         plane {     < 1.000000  0.000000  0.000000>  1.104983 inverse  }
      }            
      translate <-1.109983  0.000000  0.000000>
      scale     < 178.369266  1.000000  1.000000>
      translate <-0.006223  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000 -66.968981  0.000000>
      translate < 2.325924  0.000000 -1.689883>
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Dot
         scale     < 1.105000  1.105000  1.105000>
         translate < 2.674846  0.000000 -0.869110>
      }            
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.104982  }
         plane {     < 1.000000  0.000000  0.000000>  1.099982 inverse  }
      }            
      translate <-1.104982  0.000000  0.000000>
      scale     < 174.467798  1.000000  1.000000>
      translate <-0.006333  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000 -85.057792  0.000000>
      translate < 2.674846  0.000000 -0.869110>
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Dot
         scale     < 1.100000  1.100000  1.100000>
         translate < 2.750000  0.000000  0.000000>
      }            
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.099981  }
         plane {     < 1.000000  0.000000  0.000000>  1.094981 inverse  }
      }            
      translate <-1.099981  0.000000  0.000000>
      scale     < 170.566935  1.000000  1.000000>
      translate <-0.006449  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000 -103.150812  0.000000>
      translate < 2.750000  0.000000  0.000000>
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Dot
         scale     < 1.095000  1.095000  1.095000>
         translate < 2.555964  0.000000  0.830483>
      }            
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.094980  }
         plane {     < 1.000000  0.000000  0.000000>  1.089980 inverse  }
      }            
      translate <-1.094980  0.000000  0.000000>
      scale     < 166.666419  1.000000  1.000000>
      translate <-0.006570  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000 -121.248055  0.000000>
      translate < 2.555964  0.000000  0.830483>
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Dot
         scale     < 1.090000  1.090000  1.090000>
         translate < 2.123670  0.000000  1.542936>
      }            
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.089979  }
         plane {     < 1.000000  0.000000  0.000000>  1.084980 inverse  }
      }            
      translate <-1.089979  0.000000  0.000000>
      scale     < 162.766634  1.000000  1.000000>
      translate <-0.006697  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000 -139.350064  0.000000>
      translate < 2.123670  0.000000  1.542936>
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Dot
         scale     < 1.085000  1.085000  1.085000>
         translate < 1.506200  0.000000  2.073106>
      }            
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.084979  }
         plane {     < 1.000000  0.000000  0.000000>  1.079979 inverse  }
      }            
      translate <-1.084979  0.000000  0.000000>
      scale     < 158.867319  1.000000  1.000000>
      translate <-0.006829  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000 -157.457091  0.000000>
      translate < 1.506200  0.000000  2.073106>
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Dot
         scale     < 1.080000  1.080000  1.080000>
         translate < 0.772542  0.000000  2.377641>
      }            
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.079978  }
         plane {     < 1.000000  0.000000  0.000000>  1.074978 inverse  }
      }            
      translate <-1.079978  0.000000  0.000000>
      scale     < 154.968286  1.000000  1.000000>
      translate <-0.006969  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000 -175.569394  0.000000>
      translate < 0.772542  0.000000  2.377641>
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Dot
         scale     < 1.075000  1.075000  1.075000>
         translate <-0.000000  0.000000  2.437500>
      }            
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.074976  }
         plane {     < 1.000000  0.000000  0.000000>  1.069977 inverse  }
      }            
      translate <-1.074976  0.000000  0.000000>
      scale     < 151.070126  1.000000  1.000000>
      translate <-0.007116  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  166.312395  0.000000>
      translate <-0.000000  0.000000  2.437500>
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Dot
         scale     < 1.070000  1.070000  1.070000>
         translate <-0.733915  0.000000  2.258759>
      }            
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.069975  }
         plane {     < 1.000000  0.000000  0.000000>  1.064975 inverse  }
      }            
      translate <-1.069975  0.000000  0.000000>
      scale     < 147.172614  1.000000  1.000000>
      translate <-0.007270  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  148.188022  0.000000>
      translate <-0.733915  0.000000  2.258759>
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Dot
         scale     < 1.065000  1.065000  1.065000>
         translate <-1.359253  0.000000  1.870852>
      }            
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.064974  }
         plane {     < 1.000000  0.000000  0.000000>  1.059974 inverse  }
      }            
      translate <-1.064974  0.000000  0.000000>
      scale     < 143.275894  1.000000  1.000000>
      translate <-0.007433  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  130.056813  0.000000>
      translate <-1.359253  0.000000  1.870852>
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Dot
         scale     < 1.060000  1.060000  1.060000>
         translate <-1.820288  0.000000  1.322517>
      }            
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.059973  }
         plane {     < 1.000000  0.000000  0.000000>  1.054973 inverse  }
      }            
      translate <-1.059973  0.000000  0.000000>
      scale     < 139.379846  1.000000  1.000000>
      translate <-0.007605  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  111.918291  0.000000>
      translate <-1.820288  0.000000  1.322517>
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Dot
         scale     < 1.055000  1.055000  1.055000>
         translate <-2.080436  0.000000  0.675975>
      }            
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.054971  }
         plane {     < 1.000000  0.000000  0.000000>  1.049971 inverse  }
      }            
      translate <-1.054971  0.000000  0.000000>
      scale     < 135.484782  1.000000  1.000000>
      translate <-0.007787  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  93.771796  0.000000>
      translate <-2.080436  0.000000  0.675975>
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Dot
         scale     < 1.050000  1.050000  1.050000>
         translate <-2.125000  0.000000 -0.000000>
      }            
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.049970  }
         plane {     < 1.000000  0.000000  0.000000>  1.044970 inverse  }
      }            
      translate <-1.049970  0.000000  0.000000>
      scale     < 131.590588  1.000000  1.000000>
      translate <-0.007979  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  75.616629  0.000000>
      translate <-2.125000  0.000000 -0.000000>
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Dot
         scale     < 1.045000  1.045000  1.045000>
         translate <-1.961554  0.000000 -0.637348>
      }            
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.044968  }
         plane {     < 1.000000  0.000000  0.000000>  1.039968 inverse  }
      }            
      translate <-1.044968  0.000000  0.000000>
      scale     < 127.697296  1.000000  1.000000>
      translate <-0.008183  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  57.452008  0.000000>
      translate <-1.961554  0.000000 -0.637348>
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Dot
         scale     < 1.040000  1.040000  1.040000>
         translate <-1.618034  0.000000 -1.175571>
      }            
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.039966  }
         plane {     < 1.000000  0.000000  0.000000>  1.034966 inverse  }
      }            
      translate <-1.039966  0.000000  0.000000>
      scale     < 123.805083  1.000000  1.000000>
      translate <-0.008400  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  39.276962  0.000000>
      translate <-1.618034  0.000000 -1.175571>
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Dot
         scale     < 1.035000  1.035000  1.035000>
         translate <-1.138834  0.000000 -1.567470>
      }            
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.034964  }
         plane {     < 1.000000  0.000000  0.000000>  1.029964 inverse  }
      }            
      translate <-1.034964  0.000000  0.000000>
      scale     < 119.914366  1.000000  1.000000>
      translate <-0.008631  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  21.090736  0.000000>
      translate <-1.138834  0.000000 -1.567470>
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Dot
         scale     < 1.030000  1.030000  1.030000>
         translate <-0.579407  0.000000 -1.783231>
      }            
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.029962  }
         plane {     < 1.000000  0.000000  0.000000>  1.024962 inverse  }
      }            
      translate <-1.029962  0.000000  0.000000>
      scale     < 116.024850  1.000000  1.000000>
      translate <-0.008877  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000  2.891863  0.000000>
      translate <-0.579407  0.000000 -1.783231>
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Dot
         scale     < 1.025000  1.025000  1.025000>
         translate < 0.000000  0.000000 -1.812500>
      }            
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.024959  }
         plane {     < 1.000000  0.000000  0.000000>  1.019959 inverse  }
      }            
      translate <-1.024959  0.000000  0.000000>
      scale     < 112.136832  1.000000  1.000000>
      translate <-0.009140  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000 -15.320731  0.000000>
      translate < 0.000000  0.000000 -1.812500>
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Dot
         scale     < 1.020000  1.020000  1.020000>
         translate < 0.540780  0.000000 -1.664349>
      }            
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.019956  }
         plane {     < 1.000000  0.000000  0.000000>  1.014957 inverse  }
      }            
      translate <-1.019956  0.000000  0.000000>
      scale     < 108.250447  1.000000  1.000000>
      translate <-0.009422  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000 -33.548648  0.000000>
      translate < 0.540780  0.000000 -1.664349>
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Dot
         scale     < 1.015000  1.015000  1.015000>
         translate < 0.991888  0.000000 -1.365216>
      }            
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.014953  }
         plane {     < 1.000000  0.000000  0.000000>  1.009954 inverse  }
      }            
      translate <-1.014953  0.000000  0.000000>
      scale     < 104.365808  1.000000  1.000000>
      translate <-0.009725  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000 -51.793472  0.000000>
      translate < 0.991888  0.000000 -1.365216>
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Dot
         scale     < 1.010000  1.010000  1.010000>
         translate < 1.314653  0.000000 -0.955151>
      }            
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Connector_2
      }            
      clipped_by {
         plane {     < 1.000000  0.000000  0.000000>  1.009950  }
         plane {     < 1.000000  0.000000  0.000000>  1.004950 inverse  }
      }            
      translate <-1.009950  0.000000  0.000000>
      scale     < 100.483298  1.000000  1.000000>
      translate <-0.010051  0.000000  0.000000>
      rotate    < 0.000000  180.000000  0.000000>
      rotate    < 0.000000 -70.057277  0.000000>
      translate < 1.314653  0.000000 -0.955151>
      texture {
         YourTexture
      }            
   }      
   object {
      quadric {
         Dot
         scale     < 1.005000  1.005000  1.005000>
         translate < 1.486026  0.000000 -0.482839>
      }            
      texture {
         YourTexture
      }            
   }      
   /*
   Fudge factor:  10.000000 percent 
   Maximum x coordinate:  5.200000
           y coordinate:  1.200000
           z coordinate:  4.862500
   Minimum x coordinate: -4.525000
           y coordinate: -1.200000
           z coordinate: -4.187500
   Center  x coordinate:  0.337500
           y coordinate:  0.000000
           z coordinate:  0.337500
*/
}/* end_composite */


#version Turn_Inc_Temp;
