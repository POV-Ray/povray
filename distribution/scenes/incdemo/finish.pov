// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision raytracer sample file.
// FINISH.POV file demonstrates all finishes in FINISH.INC
//
// -w320 -h240
// -w800 -h600 +a0.3

#version 3.7;

global_settings {
  assumed_gamma 2.2
  max_trace_level 5
}

#include "finish.inc"

#declare Textures=array[3][9]
{
  {
     texture { pigment {rgb <0,1,1>}  finish {Dull          } },
     texture { pigment {rgb <0,1,1>}  finish {Shiny         } },
     texture { pigment {rgb <0,1,1>}  finish {Phong_Dull    } },

     texture { pigment {rgb <0,1,1>}  finish {Phong_Shiny   } },
     texture { pigment {rgb <0,1,1>}  finish {Glossy        } },
     texture { pigment {rgb <0,1,1>}  finish {Phong_Glossy  } },

     texture { pigment {rgb <0,1,1>}  finish {Luminous      } },
     texture { pigment {rgb <0,1,1>}  finish {Mirror        } },
     texture { pigment {rgbt 1}  }
  },
  {
     texture { pigment {rgb <1,0,0>}  finish {Dull          } },
     texture { pigment {rgb <1,0,0>}  finish {Shiny         } },
     texture { pigment {rgb <1,0,0>}  finish {Phong_Dull    } },

     texture { pigment {rgb <1,0,0>}  finish {Phong_Shiny   } },
     texture { pigment {rgb <1,0,0>}  finish {Glossy        } },
     texture { pigment {rgb <1,0,0>}  finish {Phong_Glossy  } },

     texture { pigment {rgb <1,0,0>}  finish {Luminous      } },
     texture { pigment {rgb <1,0,0>}  finish {Mirror        } },
     texture { pigment {rgbt 1}  }
  },
  {
     texture { pigment {rgb <0,0,0>}  finish {Dull          } },
     texture { pigment {rgb <0,0,0>}  finish {Shiny         } },
     texture { pigment {rgb <0,0,0>}  finish {Phong_Dull    } },

     texture { pigment {rgb <0,0,0>}  finish {Phong_Shiny   } },
     texture { pigment {rgb <0,0,0>}  finish {Glossy        } },
     texture { pigment {rgb <0,0,0>}  finish {Phong_Glossy  } },

     texture { pigment {rgb <0,0,0>}  finish {Luminous      } },
     texture { pigment {rgb <0,0,0>}  finish {Mirror        } },
     texture { pigment {rgbt 1}  }
  }
}

#include "shotxtr.inc"
