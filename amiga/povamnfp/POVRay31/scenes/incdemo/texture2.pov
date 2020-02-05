// Persistence Of Vision raytracer version 3.1 sample file.
// The TEXTUREn.POV files demonstrate all textures in TEXTURES.INC

global_settings { assumed_gamma 2.2 }

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
