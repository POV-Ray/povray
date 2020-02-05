// Persistence Of Vision raytracer version 3.1 sample file.
// FINISH.POV file demonstrates all finishes in FINISH.INC

global_settings { assumed_gamma 2.2 }

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
