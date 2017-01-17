// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.
//
// Persistence Of Vision Ray Tracer ('POV-Ray') sample file.
//
// Crackle pattern example crackle_form.pov.
//
// Date: July/August 2001
// Auth: Christoph Hormann
// Updated: January 2017
//
// -w320 -h240 +a0.3 +sf1 +ef11 +kff20

#version 3.7;
global_settings { assumed_gamma 1.0 }
#default { finish { ambient 0.006 diffuse 0.456 } }

#if (!clock_on)
    #warning concat("This scene should be rendered as an animation\n",
                    "use '+sf1 +ef11' for rendering all versions.\n")
#end

light_source {
    <1.9,1.6,2.7>*10000
    rgb 1.3
}

//-------------------------------------------------------------------------
// This scene uses a non-standard camera set-up.
// (See CAMERA in the included documentation for details.)
// If you are new to POV-Ray, you might want to try a different demo scene.
//-------------------------------------------------------------------------
camera {
    location  <-7, 24, 12>
    direction y
    sky       z
    up        z
    right     (4/3)*x
    look_at   <0, 0, 0>
    angle     10
}

#macro P_Crack()
    crackle
    #if (frame_number=1)
      form <-1,1,0>
    #end
    #if (frame_number=2)
      form <1,0,0>
    #end
    #if (frame_number=3)
      form <-0.5,0.9,0>
    #end
    #if (frame_number=4)
      form <1.1,0,0>
      metric 4
    #end
    #if (frame_number=5)
      form <1.1,0,0>
      metric 6
    #end
    #if (frame_number=6)
      form <-0.2,0,0.9>
    #end
    #if (frame_number=7)
      form <-0.5,-0.2,0.6>
    #end
    #if (frame_number=8)
      form <0.5,0.5,-0.6>
    #end
    #if (frame_number=9)
      form <0.4,0.4,-0.4>
      metric 4
    #end
    #if (frame_number=10)
      form <-0.5,1.0,-0.5>
    #end
    #if (frame_number=11)
      form <1,-0.5,-0.25>
    #end
#end

#declare P_Crack2 = pigment {
    P_Crack()
}

#declare FloorColor00 = srgb <0.2353,0.1333,0.4824>;
#declare FloorColor01 = srgb <0.5647,0.4353,0.8000>;
#declare FloorColor02 = srgb <0.6549,0.4000,0.8275>;
#declare FloorColor03 = srgb <0.7294,0.4039,0.8471>;
#declare FloorColor04 = srgb <0.9059,0.5255,0.8980>;
#declare FloorColor05 = srgb <1.0000,0.6784,0.8549>;
#declare FloorColor06 = srgb <0.9300,0.6800,0.5922>;
#declare FloorColor07 = srgb <0.9000,0.7000,0.4078>;
#declare FloorColor08 = srgb <0.9200,0.7000,0.3294>;
#declare FloorColor09 = srgb <1.0000,0.2000,0.8000>;
#declare P_Crack_Col = pigment {
    P_Crack()
    color_map {
        [ 0.00 color FloorColor00 ]
        [ 0.10 color FloorColor01 ]
        [ 0.20 color FloorColor02 ]
        [ 0.30 color FloorColor03 ]
        [ 0.70 color FloorColor04 ]
        [ 0.80 color FloorColor05 ]
        [ 0.90 color FloorColor06 ]
        [ 0.93 color FloorColor07 ]
        [ 0.98 color FloorColor08 ]
        [ 1.00 color FloorColor09 ]
    }
    scale 0.9
}

plane {
    z, -1.0
    texture { pigment { P_Crack_Col } finish { ambient 0.05 } }
}

#declare fn_Crack = function { pigment { P_Crack2 scale 0.9 } }

#declare IsoPigmColor = srgb <0.48628,0.90588,0.73725>;
isosurface {
    #if (frame_number=0)
      function { -(fn_Crack(x, y, z).gray-0.4) }
    #end
    #if (frame_number=1)
      function { -(fn_Crack(x, y, z).gray-0.4) }
    #end
    #if (frame_number=2)
      function { (fn_Crack(x, y, z).gray-0.4) }
    #end
    #if (frame_number=3)
      function { -(fn_Crack(x, y, z).gray-0.5) }
    #end
    #if (frame_number=4)
      function { (fn_Crack(x, y, z).gray-0.4) }
    #end
    #if (frame_number=5)
      function { (fn_Crack(x, y, z).gray-0.4) }
    #end
    #if (frame_number=6)
      function { (fn_Crack(x, y, z).gray-0.5) }
    #end
    #if (frame_number=7)
      function { -(fn_Crack(x, y, z).gray-0.25) }
    #end
    #if (frame_number=8)
      function { -(fn_Crack(x, y, z).gray-0.28) }
    #end
    #if (frame_number=9)
      function { -(fn_Crack(x, y, z).gray-0.17) }
    #end
    #if (frame_number=10)
      function { -(fn_Crack(x, y, z).gray-0.2) }
    #end
    #if (frame_number=11)
      function { -(fn_Crack(x, y, z).gray-0.17) }
    #end

    #if (frame_number=0)
      max_gradient 2.4
    #end
    #if (frame_number=1)
      max_gradient 2.4
    #end
    #if (frame_number=2)
      max_gradient 1.2
    #end
    #if (frame_number=3)
      max_gradient 1.7
    #end
    #if (frame_number=4)
      max_gradient 1.2
    #end
    #if (frame_number=5)
      max_gradient 1.2
    #end
    #if (frame_number=6)
      max_gradient 1.4
    #end
    #if (frame_number=7)
      max_gradient 1.5
    #end
    #if (frame_number=8)
      max_gradient 1.8
    #end
    #if (frame_number=9)
      max_gradient 1.5
    #end
    #if (frame_number=10)
      max_gradient 2.25
    #end
    #if (frame_number=11)
      max_gradient 2
    #end

    accuracy 0.005
    contained_by { box { <-1.0,-1.0,-1.0>,<1.0,1.0,1.0> } }

    texture {
      pigment { IsoPigmColor }
      finish {
        specular 0.2
        roughness 0.04
        diffuse 0.8
        brilliance 2.0
        ambient 0.05
      }
    }
}



