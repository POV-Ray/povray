// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision Ray Tracer Scene Description File
// File: crackle_form.pov
// Vers: 3.5
// Desc: crackle form demonstration scene
// Date: July/August 2001
// Auth: Christoph Hormann

// -w240 -h180 +sf1 +ef11 +kff20
// -w320 -h240 +a0.3 +sf1 +ef11 +kff20

#version 3.7;

global_settings { assumed_gamma 1.0 }

#if (!clock_on)
  #warning concat("This scene should be rendered as an animation\n",
                  "use '+sf1 +ef11' for rendering all versions.\n")
#end

light_source {
  <1.9, 1.6, 2.7>*10000
  rgb 1.3
}

//-------------------------------------------------------------------------
// This scene uses a non-standard camera set-up. 
// (See CAMERA in the included documentation for details.) 
// If you are new to POV-Ray, you might want to try a different demo scene.
//-------------------------------------------------------------------------
camera {
  location    <-7, 24, 12>
  direction   y
  sky         z
  up          z
  right       (4/3)*x
  look_at     <0, 0, 0>
  angle       10
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

#declare P_Crack2=
pigment {
  P_Crack()
}


#declare P_Crack_Col=
  pigment {
    P_Crack()
    color_map {
      [ 0.0000 color rgb<0.2353, 0.1333, 0.4824> ]
      [ 0.1000 color rgb<0.5647, 0.4353, 0.8000> ]
      [ 0.2000 color rgb<0.6549, 0.4000, 0.8275> ]
      [ 0.3000 color rgb<0.7294, 0.4039, 0.8471> ]
      [ 0.7000 color rgb<0.9059, 0.5255, 0.8980> ]
      [ 0.8000 color rgb<1.0000, 0.6784, 0.8549> ]
      [ 0.9000 color rgb<0.9300, 0.6800, 0.5922> ]
      [ 0.9300 color rgb<0.9000, 0.7000, 0.4078> ]
      [ 0.9800 color rgb<0.9200, 0.7000, 0.3294> ]
      [ 1.0000 color rgb<1.3000, 0.2000, 0.8000> ]
    }
    scale 0.9
  }


plane { z, -1.0

  texture {
    pigment {
      P_Crack_Col
    }
  }
}

#declare fn_Crack=
  function {
    pigment {
      P_Crack2
      color_map { [0 rgb 0][1 rgb 1] }
      scale 0.9
    }
  }

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
    pigment { rgb <0.2, 0.8, 0.5> }
    finish {
      specular 0.5
      roughness 0.04
      diffuse 0.8
      brilliance 2.0
      ambient 0.1
    }
  }
}



