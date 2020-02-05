// Persistence Of Vision raytracer version 3.1 sample file.
// The TEXTUREn.POV files demonstrate all textures in TEXTURES.INC

global_settings { assumed_gamma 2.2 }

#include "textures.inc"

#declare Textures=array[3][9]
{
  {
     texture {           Copper_Metal      },
     texture {           Polished_Chrome   },
     texture {           Polished_Brass    },
                                          
     texture {           New_Brass         },
     texture {           Spun_Brass        },
     texture {           Brushed_Aluminum  },
                                          
     texture {           Silver1           },
     texture {           Silver2           },
     texture {           Silver3           }
  },                                      
  {                                       
     texture {           Brass_Valley      },
     texture {           Rust              },
     texture {           Rusty_Iron        },
                                          
     texture {           Soft_Silver       },
     texture {           New_Penny         },
     texture {           Tinny_Brass       },
                                          
     texture {           Gold_Nugget       },
     texture {           Aluminum          },
     texture {           Bright_Bronze     }
  },                                     
  {                                      
     texture { pigment { Candy_Cane      } },
     texture { pigment { Peel            } },
     texture { pigment { Y_Gradient      } },
                                         
     texture { pigment { X_Gradient      } },
     texture {           Water             },
     texture {           Cork              },
                                         
     texture {           Lightening1       },
     texture {           Lightening2       },
     texture {           Starfield         }
  }
}

#include "shotxtr.inc"
