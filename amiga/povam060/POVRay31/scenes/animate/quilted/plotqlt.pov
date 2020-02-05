// Persistence Of Vision raytracer version 3.1 sample file.
// By Chris Young 76702,1655
// Try changing C0 and C1 to various values from 0.0 to 1.0 in this scene.


global_settings { assumed_gamma 2.2 }

#include "colors.inc"

#declare C1=1;
#declare C0=clock;
#declare Rad=1/6/10;

#declare Xval=-0.5;

camera {
    location  <0, 0, -150>
    direction <0, 0,  12>
    look_at   <0, 0,   0>
}

light_source { <5000, 10000, -20000> color White}
plane { z, Rad hollow on pigment {checker color rgb <1,.8,.8> color rgb <1,1,.8>} }

#declare Font="cyrvetic.ttf"
text{ ttf Font
    concat("C0 =",str(C0,1,3)),0.5, 0
    scale <1.25, 1.25, 4>
    translate <-2.75, 4, -30>
    pigment { rgb <0.2, 0.8, 0.6> }
}
text{ ttf Font
    concat("C1 =",str(C1,1,3)),0.5, 0
    scale <1.25, 1.25, 4>
    translate <-2.75, 2.5, -30>
    pigment { rgb <0.2, 0.6, 0.8> }
}

union {
    #while (Xval <= 0.5)
        // This is the function that the "quilted" pattern uses
        #declare T=sqrt(3*Xval*Xval);
        #declare IT=1.0-T;
        #declare TQ=T*T*T+3*T*IT*IT*C0+3*T*T*IT*C1;

        sphere{<Xval,TQ,0>,Rad pigment{Red}}

        #declare Xval=Xval+0.01;
    #end
    scale 10
    translate -5*y
}

