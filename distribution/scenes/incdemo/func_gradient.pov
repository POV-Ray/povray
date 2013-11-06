// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence of Vision Ray Tracer Scene Description File
// File: func_gradient.pov
// Desc: Gradient macros example
// Date: January 2002
// Auth: Christoph Hormann
//
// -w512 -h512 +a0.3

// =============================================================
//
//  This scene file demonstrates the vector analysis macros in
//  'math.inc'. In the upper half the basis function is
//  displayed.  The lower left shows the length of the gradient
//  returned by the fn_Gradient() macro.  The box on the lower
//  right is textured with the directional gradient in y
//  direction calculated with the fn_Gradient_Directional()
//  macro, positive values in blue color, negative values in
//  green color.  The whole picture is covered with a
//  rectangular grid of arrows pointing in direction of the
//  gradient vector and demonstrating use of the vGradient()
//  macro.
//
// =============================================================

#version 3.6;

global_settings {
  assumed_gamma 1.0
  noise_generator 1
}

#default { finish { ambient 1 diffuse 0 } }

camera {
  orthographic
  location <0,0,1>
  look_at  <0,0,0>
  right 1*x
  up 1*y
}

// ------ the basis function for calculating the gradient from ------
#declare fn_test=
function {
  pattern {
    pigment_pattern {
      agate
      color_map {
        [0.4 rgb 0]
        [0.6 rgb 1]
      }
      scale 0.8
    }
  }
}


#include "functions.inc"
#include "math.inc"


#declare fn_grad = fn_Gradient(fn_test)
#declare fn_gradDy = fn_Gradient_Directional(fn_test, y )


// ------ lower left quarter ------
box {
  <0, -0.5, 0>, <0.5, 0.0, 0>

  texture {
    pigment {
      function { fn_gradDy(x, y, z)*0.004 + 0.5 }

      color_map {
        [0.15 color rgb y ]
        [0.45 color rgb 0 ]
        [0.55 color rgb 0 ]
        [0.85 color rgb z ]
      }
    }
  }
}

// ------ lower right quarter ------
box {
  <-0.5, -0.5, 0>, <0, 0.0, 0>

  texture {
    pigment {
      function { fn_grad(x, y, z)*0.004 }

      color_map {
        [0.0 color rgb 0.0 ]
        [1.0 color rgb 1.0 ]
      }
    }
  }
}

// ------ upper half ------
box {
  <-0.5, -0.0, 0>, <0.5, 0.5, 0>
  texture {
    pigment {
      function { fn_test(x, y, z) }

      color_map {
        [0.0 color rgb 0.0 ]
        [1.0 color rgb 1.0 ]
      }
    }
  }
}


#declare PosX=-0.5;
#declare Spacing=0.018;

union {
  #while (PosX < 0.5)

    #declare PosY=-0.5;

      #while (PosY < 0.5)

        #declare Pos=<PosX, PosY, 0>;

        #declare Vgrd=vGradient(fn_test, Pos);

        #declare Vgrd=<Vgrd.x, Vgrd.y, 0>*0.1;

        #if (vlength(Vgrd)>0.008)

          cylinder {
            <PosX, PosY, 0>,
            <PosX, PosY, 0>+Vgrd*0.005,
            0.002
            texture {
              pigment { color rgb z*Gradient_Length(fn_test, Pos)*0.01 }
            }
          }

          sphere {
            <PosX, PosY, 0>,
            0.003
            texture {
              pigment { color rgb x }
            }
          }

        #end

        #declare PosY=PosY+Spacing;

      #end

    #declare PosX=PosX+Spacing;
  #end
}
