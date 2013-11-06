// Insert menu illustration scene
// Created June-August 2001 by Christoph Hormann
// Updated to 3.7 by Friedrich A. Lohmueller, June-2012

// ----- pattern texture attributes submenu -----

// -w120 -h48 +a0.1 +am2 -j +r3

#version 3.7;
global_settings{ assumed_gamma 1.0 } 

#include "colors.inc"
#include "shapes.inc"

/*
#declare Typ=1;     // waveform
#declare Typ=2;     // frequency
#declare Typ=3;     // phase
#declare Typ=4;     // turbulence
#declare Typ=5;     // turbulence warp
#declare Typ=6;     // black_hole warp
#declare Typ=7;     // repeat warp
#declare Typ=8;     // mapping warp
*/    

global_settings {
 // assumed_gamma 1
  max_trace_level 5
}

light_source {
  <1.5, 2.5, 2.5>*10000
  color rgb 1.0
}

camera {
  #if (Typ=1)
    location    <0, 20, 7.5>*4      // waveform
  #else
    location    <5, 20, 7.5>*4
  #end
  direction   y
  sky         z
  up          z
  right x*image_width/image_height//right       (120/48)*x
  look_at     <0, 0, -0.1>
  angle       7
}

sky_sphere {
  pigment {
    gradient y
    color_map {
      [0.0 rgb <0.6,0.7,1.0>]
      [1.0 rgb <0.2,0.2,0.8>]
    }
  }
}

// ----------------------------------------

plane
{
  z, -1
  texture
  {
    pigment { color rgb 1 }
    finish {
      diffuse 0.7
      specular 0.4
      roughness 0.01

      reflection { 0.5 , 1.0
        fresnel on
        metallic 0.8
      }
      conserve_energy

    }
  }
}


// =============================================

#if (Typ=1)       // waveform

#declare Cnt=0;

#while (Cnt < 5)

  #local fn_grad=
  function {
    pigment {
      gradient x
      color_map { [0 rgb 0][1 rgb 1] }
      #switch (Cnt)
        #case (1)
          triangle_wave
          #break
        #case (2)
          scallop_wave
          #break
        #case (3)
          sine_wave
          #break
        #case (4)
          poly_wave 2
          #break
        #else
          ramp_wave
      #end

      scale 1.2

    }
  }

  #declare Tex1=
  texture {
    pigment {
      function { fn_grad(x, y, z).gray }
      color_map { [0 rgb 0][1 rgb 1] }
    }
    finish { ambient 0.2 specular 0.4 }
  }

  object {
    isosurface {
      function { z-fn_grad(x, y, 0).gray*0.5 }

      max_gradient
      #switch (Cnt)
        #case (0) 1000 #break
        #case (1) 0.4 #break
        #case (2) 0.4 #break
        #case (3) 0.4 #break
        #case (4) 975 #break
      #end
      accuracy 0.001
      contained_by { box { <-0.8, -1.2, -1.2> <0.8, 1.2, 1.2> } }
    }

    texture { Tex1 }
    no_shadow
    translate <-4.0+Cnt*2, 0, 0>
  }           
  
  #undef fn_grad
  
  #declare Cnt=Cnt+1;
#end

#end

// =============================================

#if ((Typ=2) | (Typ=3) | (Typ=4))

#declare Cnt=0;

#while (Cnt < 3)

  #declare Tex1=
  texture {
    pigment {
      crackle
      triangle_wave

      //-----------------

      #if (Typ=2)
        frequency (Cnt)+1
        color_map { [0 color SteelBlue][1 rgb <1.2, 1.2, 1.4>] }
      #end

      //-----------------

      #if (Typ=3)
        phase Cnt*(1/4)
        color_map { [0 color MediumSeaGreen][1 rgb <1.2, 1.2, 1.4>] }
      #end
      //-----------------

      #if (Typ=4)
        turbulence Cnt*(1/2)
        color_map { [0 color Firebrick][1 rgb <1.2, 1.2, 1.4>] }
      #end

      //-----------------

    }
    finish { diffuse 1 brilliance 2 }
    
  }

  object {
    Round_Box (<-1.2, -1.2, -1.2>, <1.2, 1.2, 1.2>, 0.1, 0)

    texture { Tex1 }
    no_shadow
    translate <-3.4+Cnt*3.2, 0, 0>
  }
  #declare Cnt=Cnt+1;
#end

#end

// =============================================

#if ((Typ=5) | (Typ=6) | (Typ=7) | (Typ=8))

#declare Cnt=0;

#while (Cnt < 2)

  #declare Tex1=
  texture {
    pigment {

      //-----------------

      #if (Typ=5)

      marble
      color_map { [0.3 color NavyBlue][0.7 rgb <1.2, 1.2, 1.4>] }

      scale 1+Cnt*2
      warp {
        turbulence 0.7
      }
      scale 1/(1+Cnt*2)

      #end

      //-----------------

      #if (Typ=6)

      marble
      color_map { [0.3 color Maroon][0.7 rgb <1.2, 1.2, 1.4>] }

      scale 0.6
      warp {
        black_hole <0, 1.2, 0>, 2
        strength 2.2
        falloff 2.5

        #if (Cnt=0)
          inverse
        #end
      }

      #end

      //-----------------

      #if (Typ=7)

      wood
      color_map { [0.3 color MediumSeaGreen][0.7 rgb <1.2, 1.2, 1.4>] }

      rotate 90*x

      scale 0.4
      warp { repeat x }

      #if (Cnt=0)
        warp { repeat z }
      #end

      #end

      //-----------------

      #if (Typ=8)

      checker
      color Copper,
      color rgb <1.2, 1.2, 1.4> 

      scale 0.5

      #if (Cnt=0)
        warp {
          planar
        }
      #else
        warp {
          cylindrical
        }
      #end

      #end

      //-----------------

    }
    finish { diffuse 1 brilliance 2 }
  }

  object {
    object { Round_Box (<-2, -1.2, -1.2>, <2, 1.2, 1.2>, 0.1, 0) }

    texture { Tex1 }
    no_shadow
    translate <-2.5+Cnt*5, 0, 0>
  }
  #declare Cnt=Cnt+1;
#end

#end

