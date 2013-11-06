// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision Raytracer sample file.
// The TEXTUREn.POV files demonstrate all textures in TEXTURES.INC
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;

global_settings {
  assumed_gamma 2.2 
  max_trace_level 5
}

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

     texture {           Lightning1       },
     texture {           Lightning2       },
     texture {           Starfield         }
  }
}

#include "shotxtr.inc"
