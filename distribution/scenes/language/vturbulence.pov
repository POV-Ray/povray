// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision Ray Tracer Scene Description File
// File: vturbulence.pov
// Desc: vturbulence sample
// Date: June 2001
// Auth: Christoph Hormann

// -w320 -h240
// -w512 -h384 +a0.3

#version 3.6;

global_settings {
  assumed_gamma 1.0
}

//#declare Style=0;    // just the spheres
#declare Style=1;    // with connecting cylinders

camera {
  location    <20, 16, 25>
  right     x*image_width/image_height
  look_at     <0, 0, 0>
  angle       7
}

light_source {
  <3.2, 1.8, -0.6>*100000
  color rgb <1.5, 1.4, 1.1>
}

sphere {
  0,1
  texture {
    pigment {
      agate
      color_map {
        [0.0 color rgb <1.0, 0.6, 0.4> ]
        [1.0 color rgb <1.0, 0.8, 0.6> ]
      }
    }
    finish { ambient 1 diffuse 0 }
  }
  scale 1000
  no_shadow
  hollow on
}


#declare T_Sphere=
texture {
  pigment {
    gradient y
    color_map {
      [0 color rgb <1, 0, 0> ]
      [1 color rgb <1, 1, 0> ]
    }
    translate -0.5*y
  }
}

#declare T_Sphere2=
texture {
  pigment { color rgb <0.1, 0, 1> }
}

#declare T_Cyl=
texture {
  pigment { color rgb <1, 0.2, 0.3> }
}

#declare Spacing=0.12;
#declare Radius=0.03;

#declare PosX=-1.5;

union {
  #while (PosX < 1.5)

    #declare PosZ=-1.5;
    union {
      #while (PosZ < 1.5)

        #if (clock_on)
          #declare Turb=vturbulence(2, 0.5, 6, <PosX, 0, PosZ>)*clock*0.5;
        #else
          #declare Turb=vturbulence(2, 0.5, 6, <PosX, 0, PosZ>)*0.5;
        #end

        sphere {
          <PosX, 0, PosZ>+Turb,
          Radius
          texture { T_Sphere }
        }

        #if (Style=1)
          sphere {
            <PosX, 0, PosZ>,
            Radius
            texture { T_Sphere2 }
          }
          #if (vlength(Turb)>Radius)
            cylinder {
              <PosX, 0, PosZ>,
              <PosX, 0, PosZ>+Turb ,
              Radius*0.7
              texture { T_Cyl }
            } 
          #end  
        #end

        #declare PosZ=PosZ+Spacing;

      #end
    }

    #declare PosX=PosX+Spacing;
  #end
}


