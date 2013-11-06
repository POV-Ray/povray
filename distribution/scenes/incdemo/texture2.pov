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
  assumed_gamma 1.0 
  max_trace_level 5
}

#include "textures.inc"

#declare Textures=array[3][9]
{
  {
     texture { pigment { DMFWood4                } },
     texture { pigment { DMFWood5                } },
     texture {           DMFWood6                  },

     texture { pigment { DMFLightOak             } },
     texture { pigment { DMFDarkOak              } },
     texture {           EMBWood1                  },

     texture {           Yellow_Pine               },
     texture {           Rosewood                  },
     texture {           Sandalwood                }
  },
  {
     texture {           Glass                     },
     texture {           Glass2                    },
     texture {           Glass3                    },

     texture {           Green_Glass               },
     texture {           NBglass                   },
     texture {           NBoldglass                },

     texture {           NBwinebottle              },
     texture {           NBbeerbottle              },
     texture {           Ruby_Glass                }
  },
  {
     texture {           Dark_Green_Glass          },
     texture {           Yellow_Glass              },
     texture {           Orange_Glass              },

     texture {           Vicks_Bottle_Glass        },
     texture {           Chrome_Metal              },
     texture {           Brass_Metal               },

     texture {           Bronze_Metal              },
     texture {           Gold_Metal                },
     texture {           Silver_Metal              }
  }
}

#include "shotxtr.inc"
