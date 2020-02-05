// Persistence Of Vision raytracer version 3.1 sample file.
#include "lizard.inc"

#declare BSnake_Inc_Temp = version;


/*-----------LOWER PART OF SNAKE   /   WRAPS INSIDE DOOR---*/
#declare toad1 = union {
   object {
      Dot
      texture { bones }
      scale  < 2 2 2>
      translate < 38 0 0>
   }

   object {
      Connector_2
      texture { skin  }
      clipped_by {
	 plane { < 1 0 0>  2.113991 }
	 plane { < 1 0 0>  1.999721 inverse }
      }
      translate <-1.999721  0 0>
      scale     < 59.901072  1  1>
      translate < 0.033384  0  0>
      rotate    < 0  0  5.473296>
      rotate    < 0  94.869458  0>
      translate < 38 0 0>
   }

   object {
      Dot
      texture { bones }
      scale  < 2.114286  2.114286  2.114286>
      translate < 37.421453  0.653061 -6.790992>
   }

   object {
      Connector_2
      texture { skin  }
      clipped_by {
	 plane { < 1 0 0> 2.228261 }
	 plane { < 1 0 0> 2.113992 inverse }
      }
      translate <-2.113992   0 0>
      scale     < 59.952599  1 1>
      translate < 0.035261   0 0>
      rotate    < 0 0  5.468627>
      rotate    < 0  105.155411  0>
      translate < 37.421453  0.653061 -6.790992>
   }

   object {
      Dot
      texture { bones }
      scale < 2.228571  2.228571  2.228571>
      translate < 35.638067  1.306122 -13.375190>
   }

   object {
      Connector_2
      texture { skin }
      clipped_by {
	 plane { < 1.000000  0.000000  0.000000>  2.342532  }
	 plane { < 1.000000  0.000000  0.000000>  2.228262 inverse  }
      }
      translate <-2.228262  0.000000  0.000000>
      scale     < 60.003073  1.000000  1.000000>
      translate < 0.037136  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.463975>
      rotate    < 0.000000  115.441357  0.000000>
      translate < 35.638067  1.306122 -13.375190>
   }

   object {
      Dot
      texture { bones }
      scale  < 2.342857  2.342857  2.342857>
      translate < 32.705147  1.959184 -19.540416>
   }

   object {
      Connector_2
      texture { skin }
      clipped_by {
	 plane { < 1.000000  0.000000  0.000000>  2.456802  }
	 plane { < 1.000000  0.000000  0.000000>  2.342532 inverse  }
      }
      translate <-2.342532  0.000000  0.000000>
      scale     < 60.054059  1.000000  1.000000>
      translate < 0.039007  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.459315>
      rotate    < 0.000000  125.727309  0.000000>
      translate < 32.705147  1.959184 -19.540416>
   }

   object {
      Dot
      texture { bones }
      scale  < 2.457143  2.457143  2.457143>
      translate < 28.715076  2.612245 -25.087606>
   }

   object {
      Connector_2
      texture { skin }
      clipped_by {
	 plane { < 1.000000  0.000000  0.000000>  2.571073 }
	 plane { < 1.000000  0.000000  0.000000>  2.456803 inverse }
      }
      translate <-2.456803  0.000000  0.000000>
      scale     < 60.105059  1.000000  1.000000>
      translate < 0.040875  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.454670>
      rotate    < 0.000000  136.013251  0.000000>
      translate < 28.715076  2.612245 -25.087606>
   }

   object {
      Dot
      texture { bones }
      scale < 2.571429  2.571429  2.571429>
      translate < 23.794407  3.265306 -29.837242>
   }

}

#declare toad2 =  union {

   object { Connector_1
      texture { skin }
      scale < 0.00001  2.571429  2.571429>
      translate < 23.794407  3.265306 -29.837242>
   }

   object {
      Dot
      texture { bones }
      scale  < 2.571429  2.571429  2.571429>
      translate < 23.794407  3.265306 -29.837242>
   }

   object {
      Connector_2
      texture { skin }
      clipped_by {
	 plane { < 1.000000  0.000000  0.000000>  2.685343  }
	 plane { < 1.000000  0.000000  0.000000>  2.571074 inverse  }
      }
      translate <-2.571074  0.000000  0.000000>
      scale     < 60.156586  1.000000  1.000000>
      translate < 0.042740  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.450032>
      rotate    < 0.000000  146.299200  0.000000>
      translate < 23.794407  3.265306 -29.837242>
   }

   object {
      Dot
      texture { bones }
      scale     < 2.685714  2.685714  2.685714>
      translate < 18.099849  3.918367 -33.635155>
   }

   object {
      Connector_2
      texture { skin }
      clipped_by {
	 plane { < 1.000000  0.000000  0.000000>  2.799614 }
	 plane { < 1.000000  0.000000  0.000000>  2.685344 inverse }
      }
      translate <-2.685344  0.000000  0.000000>
      scale     < 60.207060  1.000000  1.000000>
      translate < 0.044602  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.445411>
      rotate    < 0.000000  156.585149  0.000000>
      translate < 18.099849  3.918367 -33.635155>
   }

   object {
      Dot
      texture { bones }
      scale     < 2.800000  2.800000  2.800000>
      translate < 11.813278  4.571429 -36.357532>
   }

   object {
      Connector_2
      texture { skin }
      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  2.913885  }
	 plane {     < 1.000000  0.000000  0.000000>  2.799615 inverse  }
      }
      translate <-2.799615  0.000000  0.000000>
      scale     < 60.258051  1.000000  1.000000>
      translate < 0.046460  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.440782>
      rotate    < 0.000000  166.871097  0.000000>
      translate < 11.813278  4.571429 -36.357532>
   }

   object {
      Dot
      texture { bones }
      scale < 2.914286  2.914286  2.914286>
      translate < 5.135929  5.224490 -37.914951>
   }

   object {
      Connector_2
      texture { skin }
      clipped_by {
	 plane { < 1.000000  0.000000  0.000000>  3.028155  }
	 plane { < 1.000000  0.000000  0.000000>  2.913885 inverse  }
      }
      translate <-2.913885  0.000000  0.000000>
      scale     < 60.309575  1.000000  1.000000>
      translate < 0.048315  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.436169>
      rotate    < 0.000000  177.157040  0.000000>
      translate < 5.135929  5.224490 -37.914951>
   }

   object {
      Dot
      texture { bones }
      scale < 3.028571  3.028571  3.028571>
      translate <-1.718048  5.877551 -38.255318>
   }

   object {
      Connector_2
      texture { skin }
      clipped_by {
	 plane { < 1.000000  0.000000  0.000000>  3.142426  }
	 plane { < 1.000000  0.000000  0.000000>  3.028155 inverse  }
      }
      translate <-3.028155  0.000000  0.000000>
      scale     < 60.360050  1.000000  1.000000>
      translate < 0.050168  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.431563>
      rotate    < 0.000000 -172.557011  0.000000>
      translate <-1.718048  5.877551 -38.255318>
   }

   object {
      Dot
      texture { bones }
      scale  < 3.142857  3.142857  3.142857>
      translate <-8.528455  6.530612 -37.365604>
   }

}

#declare toad3 =  union {
   object {  Connector_1
      texture { skin }
      scale < 0.00001  3.142857  3.142857>
      translate <-8.528455  6.530612 -37.365604>
   }

   object {
      Dot
      texture { bones }
      scale     < 3.142857  3.142857  3.142857>
      translate <-8.528455  6.530612 -37.365604>
   }

   object {
      Connector_2
      texture { skin }
      clipped_by {
	 plane { < 1.000000  0.000000  0.000000>  3.256697  }
	 plane { < 1.000000  0.000000  0.000000>  3.142427 inverse  }
      }
      translate <-3.142427  0.000000  0.000000>
      scale     < 60.411047  1.000000  1.000000>
      translate < 0.052017  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.426966>
      rotate    < 0.000000 -162.271074  0.000000>
      translate <-8.528455  6.530612 -37.365604>
   }

   object {
      Dot
      texture { bones }
      scale     < 3.257143  3.257143  3.257143>
      translate <-15.076119  7.183673 -35.272335>
   }

   object {
      Connector_2
      texture { skin  }
      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  3.370968  }
	 plane {     < 1.000000  0.000000  0.000000>  3.256698 inverse  }
      }
      translate <-3.256698  0.000000  0.000000>
      scale     < 60.462054  1.000000  1.000000>
      translate < 0.053863  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.422383>
      rotate    < 0.000000 -151.985122  0.000000>
      translate <-15.076119  7.183673 -35.272335>
   }

   object {
      Dot
      texture { bones }
      scale     < 3.371429  3.371429  3.371429>
      translate <-21.149947  7.836735 -32.040800>
   }

   object {
      Connector_2
      texture { skin }
      clipped_by {
	 plane { < 1.000000  0.000000  0.000000>  3.485238  }
	 plane { < 1.000000  0.000000  0.000000>  3.370969 inverse  }
      }
      translate <-3.370969  0.000000  0.000000>
      scale     < 60.513578  1.000000  1.000000>
      translate < 0.055706  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.417793>
      rotate    < 0.000000 -141.699182  0.000000>
      translate <-21.149947  7.836735 -32.040800>
   }

   object {
      Dot texture { bones  }
      scale     < 3.485714  3.485714  3.485714>
      translate <-26.553730  8.489796 -27.773024>
   }

   object {

      Connector_2 texture { skin  }

      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  3.599509  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  3.485239 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-3.485239  0.000000  0.000000>
      scale     < 60.564049  1.000000  1.000000>
      translate < 0.057546  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.413218>
      rotate    < 0.000000 -131.413229  0.000000>
      translate <-26.553730  8.489796 -27.773024>

   }      

   object {
      Dot texture { bones  }
      scale     < 3.600000  3.600000  3.600000>
      translate <-31.112482  9.142857 -22.604541>
   }


   object {
      Connector_2 texture { skin  }
      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  3.713781  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  3.599510 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-3.599510  0.000000  0.000000>
      scale     < 60.615043  1.000000  1.000000>
      translate < 0.059383  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.408652>
      rotate    < 0.000000 -121.127296  0.000000>
      translate <-31.112482  9.142857 -22.604541>
   }

   object {
      Dot texture { bones  }
      scale     < 3.714286  3.714286  3.714286>
      translate <-34.678108  9.795918 -16.700097>
   }
   object {
      Connector_2 texture { skin  }
      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  3.828051  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  3.713782 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-3.713782  0.000000  0.000000>
      scale     < 60.666582  1.000000  1.000000>
      translate < 0.061216  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.404100>
      rotate    < 0.000000 -110.841345  0.000000>
      translate <-34.678108  9.795918 -16.700097>
   }

   object {

      Dot texture { bones  }
      scale     < 3.828571  3.828571  3.828571>
      translate <-37.134210  10.448980 -10.248391>
   }
}

#declare toad4 =
union {
   object {
      Connector_1
      texture { skin }
      scale        < 0.00001  3.828571  3.828571>
      translate <-37.134210  10.448980 -10.248391>
   }

   object {
      Dot texture { bones  }
      scale     < 3.828571  3.828571  3.828571>
      translate <-37.134210  10.448980 -10.248391>
   }


   object {
      Connector_2 texture { skin  }
      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  3.942322  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  3.828052 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-3.828052  0.000000  0.000000>
      scale     < 60.717050  1.000000  1.000000>
      translate < 0.063047  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.399541>
      rotate    < 0.000000 -100.555409  0.000000>
      translate <-37.134210  10.448980 -10.248391>
   }

   object {
      Dot texture { bones  }
      scale     < 3.942857  3.942857  3.942857>
      translate <-38.399891  11.102041 -3.456053>
   }

   object {
      Connector_2 texture { skin  }
      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  4.056594  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  3.942323 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-3.942323  0.000000  0.000000>
      scale     < 60.768057  1.000000  1.000000>
      translate < 0.064875  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.394996>
      rotate    < 0.000000 -90.269457  0.000000>
      translate <-38.399891  11.102041 -3.456053>
   }

   object {
      Dot texture { bones  }
      scale     < 4.057143  4.057143  4.057143>
      translate <-38.432412  11.755102  3.458980>
   }

   object {
      Connector_2 texture { skin  }
      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  4.170865  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  4.056595 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-4.056595  0.000000  0.000000>
      scale     < 60.819055  1.000000  1.000000>
      translate < 0.066699  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.390460>
      rotate    < 0.000000 -79.983520  0.000000>
      translate <-38.432412  11.755102  3.458980>
   }

   object {
      Dot texture { bones  }
      scale     < 4.171429  4.171429  4.171429>
      translate <-37.228639  12.408163  10.274452>
   }

   object {
      Connector_2 texture { skin  }
      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  4.285136  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  4.170866 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-4.170866  0.000000  0.000000>
      scale     < 60.870587  1.000000  1.000000>
      translate < 0.068520  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.385932>
      rotate    < 0.000000 -69.697580  0.000000>
      translate <-37.228639  12.408163  10.274452>
   }

   object {
      Dot texture { bones  }
      scale     < 4.285714  4.285714  4.285714>
      translate <-34.825205  13.061224  16.770935>
   }

   object {
      Connector_2 texture { skin  }
      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  4.399407  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  4.285137 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-4.285137  0.000000  0.000000>
      scale     < 60.921057  1.000000  1.000000>
      translate < 0.070339  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.381419>
      rotate    < 0.000000 -59.411630  0.000000>
      translate <-34.825205  13.061224  16.770935>
   }

   object {
      Dot texture { bones  }
      scale     < 4.400000  4.400000  4.400000>
      translate <-31.297400  13.714286  22.738892>
   }

   object {
      Connector_2 texture { skin  }
      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  4.513679  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  4.399408 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-4.399408  0.000000  0.000000>
      scale     < 60.972065  1.000000  1.000000>
      translate < 0.072154  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.376897>
      rotate    < 0.000000 -49.125696  0.000000>
      translate <-31.297400  13.714286  22.738892>
   }

   object {
      Dot texture { bones  }
      scale     < 4.514286  4.514286  4.514286>
      translate <-26.756817  14.367347  27.985437>
   }


}


#declare toad5 =
union {
   object {
      Connector_1
      texture { skin }
      scale        < 0.00001  4.514286  4.514286>
      translate <-26.756817  14.367347  27.985437>
   }

   object {
      Dot texture { bones  }
      scale     < 4.514286  4.514286  4.514286>
      translate <-26.756817  14.367347  27.985437>
   }

   object {
      Connector_2 texture { skin  }
      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  4.627950  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  4.513680 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-4.513680  0.000000  0.000000>
      scale     < 61.023597  1.000000  1.000000>
      translate < 0.073966  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.372391>
      rotate    < 0.000000 -38.839756  0.000000>
      translate <-26.756817  14.367347  27.985437>
   }

   object {
      Dot texture { bones  }
      scale     < 4.628571  4.628571  4.628571>
      translate <-21.347820  15.020408  32.340565>
   }

   object {
      Connector_2 texture { skin  }
      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  4.742221  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  4.627951 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-4.627951  0.000000  0.000000>
      scale     < 61.074073  1.000000  1.000000>
      translate < 0.075776  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.367892>
      rotate    < 0.000000 -28.553815  0.000000>
      translate <-21.347820  15.020408  32.340565>
   }


   object {

      Dot texture { bones  }
      scale     < 4.742857  4.742857  4.742857>
      translate <-15.242954  15.673469  35.662665>
   }


   object {
      Connector_2 texture { skin  }
      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  4.856493  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  4.742222 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-4.742222  0.000000  0.000000>
      scale     < 61.125071  1.000000  1.000000>
      translate < 0.077582  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.363410>
      rotate    < 0.000000 -18.267873  0.000000>
      translate <-15.242954  15.673469  35.662665>
   }


   object {

      Dot texture { bones  }
      scale     < 4.857143  4.857143  4.857143>
      translate <-8.637445  16.326531  37.843120>
   }


   object {

      Connector_2 texture { skin  }

      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  4.970765  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  4.856494 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-4.856494  0.000000  0.000000>
      scale     < 61.176075  1.000000  1.000000>
      translate < 0.079386  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.358918>
      rotate    < 0.000000 -7.981935  0.000000>
      translate <-8.637445  16.326531  37.843120>
   }
   object {

      Dot texture { bones  }
      scale     < 4.971429  4.971429  4.971429>
      translate <-1.742953  16.979592  38.809861>
   }

   object {
      Connector_2 texture { skin  }
      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  5.085036  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  4.970766 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-4.970766  0.000000  0.000000>
      scale     < 61.227624  1.000000  1.000000>
      translate < 0.081185  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.354441>
      rotate    < 0.000000  2.303999  0.000000>
      translate <-1.742953  16.979592  38.809861>
   }


   object {
      Dot texture { bones  }
      scale     < 5.085714  5.085714  5.085714>
      translate < 5.219209  17.632653  38.529745>
   }

   object {
      Connector_2 texture { skin  }
      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  5.199308  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  5.085037 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-5.085037  0.000000  0.000000>
      scale     < 61.278088  1.000000  1.000000>
      translate < 0.082983  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.349973>
      rotate    < 0.000000  12.589946  0.000000>
      translate < 5.219209  17.632653  38.529745>
   }

   object {
      Dot texture { bones  }
      scale     < 5.200000  5.200000  5.200000>
      translate < 12.025176  18.285714  37.009685>
   }


}

#declare toad6 =
union {
   object {
      Connector_1
      texture { skin }
      scale        < 0.00001  5.200000  5.200000>
      translate < 12.025176  18.285714  37.009685>
   }

   object {
      Dot texture { bones  }
      scale     < 5.200000  5.200000  5.200000>
      translate < 12.025176  18.285714  37.009685>
   }


   object {
      Connector_2 texture { skin  }
      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  5.313580  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  5.199309 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-5.199309  0.000000  0.000000>
      scale     < 61.329094  1.000000  1.000000>
      translate < 0.084777  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.345520>
      rotate    < 0.000000  22.875884  0.000000>
      translate < 12.025176  18.285714  37.009685>
   }

   object {
      Dot texture { bones  }
      scale     < 5.314286  5.314286  5.314286>
      translate < 18.455734  18.938776  34.296500>
   }

   object {
      Connector_2 texture { skin  }
      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  5.427851  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  5.313581 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-5.313581  0.000000  0.000000>
      scale     < 61.380634  1.000000  1.000000>
      translate < 0.086568  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.341058>
      rotate    < 0.000000  33.161819  0.000000>
      translate < 18.455734  18.938776  34.296500>
   }

   object {
      Dot texture { bones  }
      scale     < 5.428571  5.428571  5.428571>
      translate < 24.303378  19.591837  30.475472>
   }

   object {
      Connector_2 texture { skin  }
      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  5.542123  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  5.427852 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-5.427852  0.000000  0.000000>
      scale     < 61.431101  1.000000  1.000000>
      translate < 0.088357  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.336612>
      rotate    < 0.000000  43.447758  0.000000>
      translate < 24.303378  19.591837  30.475472>
   }

   object {
      Dot texture { bones  }
      scale     < 5.542857  5.542857  5.542857>
      translate < 29.379008  20.244898  25.667667>
   }

   object {
      Connector_2 texture { skin  }
      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  5.656395  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  5.542124 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-5.542124  0.000000  0.000000>
      scale     < 61.482119  1.000000  1.000000>
      translate < 0.090142  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.332172>
      rotate    < 0.000000  53.733690  0.000000>
      translate < 29.379008  20.244898  25.667667>
   }

   object {
      Dot texture { bones  }
      scale     < 5.657143  5.657143  5.657143>
      translate < 33.518046  20.897959  20.026100>
   }


   object {
      Connector_2 texture { skin  }
      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  5.770667  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  5.656396 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-5.656396  0.000000  0.000000>
      scale     < 61.533116  1.000000  1.000000>
      translate < 0.091924  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.327741>
      rotate    < 0.000000  64.019633  0.000000>
      translate < 33.518046  20.897959  20.026100>
   }

   object {
      Dot texture { bones  }
      scale     < 5.771429  5.771429  5.771429>
      translate < 36.585766  21.551020  13.730868>
   }

   object {
      Connector_2 texture { skin  }
      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  5.884938  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  5.770668 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-5.770668  0.000000  0.000000>
      scale     < 61.584667  1.000000  1.000000>
      translate < 0.093703  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.323325>
      rotate    < 0.000000  74.305568  0.000000>
      translate < 36.585766  21.551020  13.730868>
   }

   object {
      Dot texture { bones  }
      scale     < 5.885714  5.885714  5.885714>
      translate < 38.481687  22.204082  6.983397>
   }


}

#declare toad7 =
union {
   object {
      Connector_1
      texture { skin }
      scale        < 0.00001  5.885714  5.885714>
      translate < 38.481687  22.204082  6.983397>
   }

   object {
      Dot texture { bones  }
      scale     < 5.885714  5.885714  5.885714>
      translate < 38.481687  22.204082  6.983397>
   }

   object {
      Connector_2 texture { skin  }
      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  5.999210  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  5.884939 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-5.884939  0.000000  0.000000>
      scale     < 61.635138  1.000000  1.000000>
      translate < 0.095480  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.318899>
      rotate    < 0.000000  84.591505  0.000000>
      translate < 38.481687  22.204082  6.983397>
   }


   object {
      Dot texture { bones  }
      scale     < 6.000000  6.000000  6.000000>
      translate < 39.142857  22.857143 -0.000000>
   }

   object {
      Connector_2 texture { skin  }
      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  6.113483  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  5.999212 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-5.999212  0.000000  0.000000>
      scale     < 61.686139  1.000000  1.000000>
      translate < 0.097254  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.314490>
      rotate    < 0.000000  94.877436  0.000000>
      translate < 39.142857  22.857143 -0.000000>
   }


   object {
      Dot texture { bones  }
      scale     < 6.114286  6.114286  6.114286>
      translate < 38.545944  23.510204 -6.995057>
   }


   object {
      Connector_2 texture { skin  }
      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  6.227754  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  6.113484 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-6.113484  0.000000  0.000000>
      scale     < 61.737700  1.000000  1.000000>
      translate < 0.099024  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.310087>
      rotate    < 0.000000  105.163373  0.000000>
      translate < 38.545944  23.510204 -6.995057>
   }


   object {
      Dot texture { bones  }
      scale     < 6.228571  6.228571  6.228571>
      translate < 36.708050  24.163265 -13.776762>
   }


   object {
      Connector_2 texture { skin  }
      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  6.342026  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  6.227755 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-6.227755  0.000000  0.000000>
      scale     < 61.788157  1.000000  1.000000>
      translate < 0.100792  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.305701>
      rotate    < 0.000000  115.449319  0.000000>
      translate < 36.708050  24.163265 -13.776762>
   }


   object {
      Dot texture { bones  }
      scale     < 6.342857  6.342857  6.342857>
      translate < 33.686231  24.816327 -20.126586>
   }


   object {
      Connector_2 texture { skin  }

      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  6.456299  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  6.342028 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-6.342028  0.000000  0.000000>
      scale     < 61.839168  1.000000  1.000000>
      translate < 0.102557  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.301305>
      rotate    < 0.000000  125.735243  0.000000>
      translate < 33.686231  24.816327 -20.126586>
   }


   object {
      Dot texture { bones  }
      scale     < 6.457143  6.457143  6.457143>
      translate < 29.575729  25.469388 -25.839536>
   }


   object {
      Connector_2 texture { skin  }
      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  6.570571  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  6.456300 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-6.456300  0.000000  0.000000>
      scale     < 61.890184  1.000000  1.000000>
      translate < 0.104319  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.296924>
      rotate    < 0.000000  136.021179  0.000000>
      translate < 29.575729  25.469388 -25.839536>
   }

    object {
       Dot texture { bones  }
      scale     < 6.571429  6.571429  6.571429>
      translate < 24.506966  26.122449 -30.730764>
   }


}

#declare toad8 =
union {
    object {
      Connector_1
      texture { skin }
      scale        < 0.00001  6.571429  6.571429>
      translate < 24.506966  26.122449 -30.730764>
   }
   object {
      Dot texture { bones  }
      scale     < 6.571429  6.571429  6.571429>
      translate < 24.506966  26.122449 -30.730764>
   }

   object {
      Connector_2 texture { skin  }
      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  6.684843  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  6.570573 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-6.570573  0.000000  0.000000>
      scale     < 61.941727  1.000000  1.000000>
      translate < 0.106077  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.292551>
      rotate    < 0.000000  146.307110  0.000000>
      translate < 24.506966  26.122449 -30.730764>
   }

   object {
      Dot texture { bones  }
      scale     < 6.685714  6.685714  6.685714>
      translate < 18.641413  26.775510 -34.641550>
   }

   object {
      Connector_2 texture { skin  }
      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  6.799115  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  6.684844 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-6.684844  0.000000  0.000000>
      scale     < 61.992205  1.000000  1.000000>
      translate < 0.107834  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.288184>
      rotate    < 0.000000  156.593047  0.000000>
      translate < 18.641413  26.775510 -34.641550>
   }

   object {
      Dot texture { bones  }
      scale     < 6.800000  6.800000  6.800000>
      translate < 12.166440  27.428571 -37.444454>
   }

   object {
      Connector_2 texture { skin  }
      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  6.913388  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  6.799117 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-6.799117  0.000000  0.000000>
      scale     < 62.043201  1.000000  1.000000>
      translate < 0.109587  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.283834>
      rotate    < 0.000000  166.878983  0.000000>
      translate < 12.166440  27.428571 -37.444454>
   }

   object {
      Dot texture { bones  }
      scale     < 6.914286  6.914286  6.914286>
      translate < 5.289339  28.081633 -39.047465>
   }

   object {
      Connector_2 texture { skin  }
      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  7.027660  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  6.913390 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-6.913390  0.000000  0.000000>
      scale     < 62.094765  1.000000  1.000000>
      translate < 0.111336  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.279473>
      rotate    < 0.000000  177.164906  0.000000>
      translate < 5.289339  28.081633 -39.047465>
   }

   object {
      Dot texture { bones  }
      scale     < 7.028571  7.028571  7.028571>
      translate <-1.769322  28.734694 -39.397025>
   }

   object {

      Connector_2 texture { skin  }

      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  7.141932  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  7.027661 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-7.027661  0.000000  0.000000>
      scale     < 62.145236  1.000000  1.000000>
      translate < 0.113084  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.275128>
      rotate    < 0.000000 -172.549153  0.000000>
      translate <-1.769322  28.734694 -39.397025>
   }

   object {

      Dot texture { bones  }
      scale     < 7.142857  7.142857  7.142857>
      translate <-8.782765  29.387755 -38.479808>
   }

   object {
      Connector_2 texture { skin  }
      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  7.256205  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  7.141934 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-7.141934  0.000000  0.000000>
      scale     < 62.196245  1.000000  1.000000>
      translate < 0.114829  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.270791>
      rotate    < 0.000000 -162.263225  0.000000>
      translate <-8.782765  29.387755 -38.479808>
   }

   object {
      Dot texture { bones  }
      scale     < 7.257143  7.257143  7.257143>
      translate <-15.525291  30.040816 -36.323224>
   }

}

#declare toad9 =
union {

   object {
      Connector_1
      texture { skin }
      scale        < 0.00001  7.257143  7.257143>
      translate <-15.525291  30.040816 -36.323224>
   }

   object {
      Dot texture { bones  }
      scale     < 7.257143  7.257143  7.257143>
      translate <-15.525291  30.040816 -36.323224>
   }

   object {
      Connector_2 texture { skin  }
      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  7.370478  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  7.256207 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-7.256207  0.000000  0.000000>
      scale     < 62.247261  1.000000  1.000000>
      translate < 0.116571  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.266468>
      rotate    < 0.000000 -151.977288  0.000000>
      translate <-15.525291  30.040816 -36.323224>
   }


   object {
      Dot texture { bones  }
      scale     < 7.371429  7.371429  7.371429>
      translate <-21.779544  30.693878 -32.994598>
   }

   object {
      Connector_2 texture { skin  }
      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  7.484750  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  7.370480 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-7.370480  0.000000  0.000000>
      scale     < 62.298813  1.000000  1.000000>
      translate < 0.118309  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.262137>
      rotate    < 0.000000 -141.691359  0.000000>
      translate <-21.779544  30.693878 -32.994598>
   }

   object {
      Dot texture { bones  }
      scale     < 7.485714  7.485714  7.485714>
      translate <-27.343516  31.346939 -28.599075>
   }

   object {
      Connector_2 texture { skin  }
      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  7.599023  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  7.484751 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-7.484751  0.000000  0.000000>
      scale     < 62.349277  1.000000  1.000000>
      translate < 0.120046  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.257821>
      rotate    < 0.000000 -131.405427  0.000000>
      translate <-27.343516  31.346939 -28.599075>
   }

   object {
      Dot texture { bones  }
      scale     < 7.600000  7.600000  7.600000>
      translate <-32.037073  32.000000 -23.276296>
   }

   object {
      Connector_2 texture { skin  }
      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  7.713296  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  7.599024 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-7.599024  0.000000  0.000000>
      scale     < 62.400293  1.000000  1.000000>
      translate < 0.121779  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.253511>
      rotate    < 0.000000 -121.119502  0.000000>
      translate <-32.037073  32.000000 -23.276296>
   }

   object {
      Dot texture { bones  }
      scale     < 7.714286  7.714286  7.714286>
      translate <-35.707787  32.653061 -17.195964>
   }

   object {
      Connector_2 texture { skin  }
      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  7.827568  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  7.713297 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-7.713297  0.000000  0.000000>
      scale     < 62.451854  1.000000  1.000000>
      translate < 0.123508  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.249209>
      rotate    < 0.000000 -110.833564  0.000000>
      translate <-35.707787  32.653061 -17.195964>
   }
   object {

      Dot texture { bones  }
      scale     < 7.828571  7.828571  7.828571>
      translate <-38.235882  33.306122 -10.552433>

   }
   object {

      Connector_2 texture { skin  }

      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  7.941841  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  7.827569 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-7.827569  0.000000  0.000000>
      scale     < 62.502319  1.000000  1.000000>
      translate < 0.125236  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.244922>
      rotate    < 0.000000 -100.547638  0.000000>
      translate <-38.235882  33.306122 -10.552433>
   }
   object {
      Dot texture { bones  }
      scale     < 7.942857  7.942857  7.942857>
      translate <-39.538147  33.959184 -3.558498>
   }
}

#declare toad10 =
union {

   object {
      Connector_1
      texture { skin }
      scale        < 0.00001  7.942857  7.942857>
      translate <-39.538147  33.959184 -3.558498>
   }

   object {

      Dot texture { bones  }
      scale     < 7.942857  7.942857  7.942857>
      translate <-39.538147  33.959184 -3.558498>

   }

   object {

      Connector_2 texture { skin  }
      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  8.056114  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  7.941842 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-7.941842  0.000000  0.000000>
      scale     < 62.553341  1.000000  1.000000>
      translate < 0.126961  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.240625>
      rotate    < 0.000000 -90.261703  0.000000>
      translate <-39.538147  33.959184 -3.558498>
   }

   object {
      Dot texture { bones  }
      scale     < 8.057143  8.057143  8.057143>
      translate <-39.570668  34.612245  3.561425>
   }


   object {
      Connector_2 texture { skin  }
      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  8.170387  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  8.056115 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-8.056115  0.000000  0.000000>
      scale     < 62.604348  1.000000  1.000000>
      translate < 0.128683  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.236344>
      rotate    < 0.000000 -79.975782  0.000000>
      translate <-39.570668  34.612245  3.561425>
   }

   object {
      Dot texture { bones  }
      scale     < 8.171429  8.171429  8.171429>
      translate <-38.330311  35.265306  10.578494>
   }

   object {
      Connector_2 texture { skin  }
      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  8.284659  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  8.170388 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-8.170388  0.000000  0.000000>
      scale     < 62.655912  1.000000  1.000000>
      translate < 0.130401  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.232070>
      rotate    < 0.000000 -69.689845  0.000000>
      translate <-38.330311  35.265306  10.578494>
   }

   object {
      Dot texture { bones  }
      scale     < 8.285714  8.285714  8.285714>
      translate <-35.854883  35.918367  17.266802>
   }

   object {
      Connector_2 texture { skin  }
      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  8.398932  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  8.284661 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-8.284661  0.000000  0.000000>
      scale     < 62.706378  1.000000  1.000000>
      translate < 0.132118  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.227811>
      rotate    < 0.000000 -59.403923  0.000000>
      translate <-35.854883  35.918367  17.266802>
   }

   object {
      Dot texture { bones  }
      scale     < 8.400000  8.400000  8.400000>
      translate <-32.221991  36.571429  23.410647>
   }

   object {
      Connector_2 texture { skin  }
      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  8.513205  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  8.398934 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-8.398934  0.000000  0.000000>
      scale     < 62.757393  1.000000  1.000000>
      translate < 0.133832  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.223542>
      rotate    < 0.000000 -49.117989  0.000000>
      translate <-32.221991  36.571429  23.410647>
   }
   object {

      Dot texture { bones  }
      scale     < 8.514286  8.514286  8.514286>
      translate <-27.546603  37.224490  28.811488>
   }
   object {
      Connector_2 texture { skin  }
      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  8.627478  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  8.513207 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-8.513207  0.000000  0.000000>
      scale     < 62.808956  1.000000  1.000000>
      translate < 0.135541  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.219289>
      rotate    < 0.000000 -38.832069  0.000000>
      translate <-27.546603  37.224490  28.811488>
   }
   object {
      Dot texture { bones  }
      scale     < 8.628571  8.628571  8.628571>
      translate <-21.977417  37.877551  33.294363>
   }
}

#declare toad11 =
union {
   object {
      Connector_1
      texture { skin }
      scale        < 0.00001  8.628571  8.628571>
      translate <-21.977417  37.877551  33.294363>
   }

   object {
      Dot texture { bones  }
      scale     < 8.628571  8.628571  8.628571>
      translate <-21.977417  37.877551  33.294363>
   }

   object {
      Connector_2 texture { skin  }
      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  8.741751  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  8.627479 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-8.627479  0.000000  0.000000>
      scale     < 62.859430  1.000000  1.000000>
      translate < 0.137250  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.215042>
      rotate    < 0.000000 -28.546139  0.000000>
      translate <-21.977417  37.877551  33.294363>
   }
   object {
      Dot texture { bones  }
      scale     < 8.742857  8.742857  8.742857>
      translate <-15.692126  38.530612  36.713554>
   }
   object {
      Connector_2 texture { skin  }
      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  8.856024  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  8.741753 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-8.741753  0.000000  0.000000>
      scale     < 62.910442  1.000000  1.000000>
      translate < 0.138956  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.210802>
      rotate    < 0.000000 -18.260212  0.000000>
      translate <-15.692126  38.530612  36.713554>
   }
   object {
      Dot texture { bones  }
      scale     < 8.857143  8.857143  8.857143>
      translate <-8.891755  39.183673  38.957324>
   }
   object {
      Connector_2 texture { skin  }
      clipped_by {
	 plane {     < 1.000000  0.000000  0.000000>  8.998227  }/* end_plane */
	 plane {     < 1.000000  0.000000  0.000000>  8.855398 inverse  }/* end_plane */
      }            /* end_clip */
      translate <-8.855398  0.000000  0.000000>
      scale     < 50.365771  1.000000  1.000000>
      translate < 0.175822  0.000000  0.000000>
      rotate    < 0.000000  0.000000  5.206577>
      rotate    < 0.000000 -7.974282  0.000000>
      translate <-8.891755  39.183673  38.957324>
   }
   object {
      Dot texture { bones  }
      scale     < 9.000000  9.000000  9.000000>
      translate <-1.794227  39.836735  39.951568>
   }
}

#declare lizard =
union {
   object { toad1  }
   object { toad2  }
   object { toad3  }
   object { toad4  }
   object { toad5  }
   object { toad6  }
   object { toad7  }
   object { toad8  }
   object { toad9  }
   object { toad10 }
   object { toad11 }
}

#version BSnake_Inc_Temp;
