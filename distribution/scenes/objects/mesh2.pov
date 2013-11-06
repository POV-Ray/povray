// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence of Vision Ray Tracer Scene Description File
// File: mesh2.pov
// Vers: 3.5
// Desc: mesh2 demonstration scene
// Date: November/December 2001
// Auth: Christoph Hormann

// -w320 -h240
// -w512 -h384 +a0.3
#version 3.7;
global_settings { assumed_gamma 1.0 }

light_source {
  <-0.6, 1.6, 3.7>*10000
  rgb 1.3
}

//-------------------------------------------------------------------------
// This scene uses a non-standard camera set-up. 
// (See CAMERA in the included documentation for details.) 
// If you are new to POV-Ray, you might want to try a different demo scene.
//-------------------------------------------------------------------------
camera {
  location    <7, 20, 20>
  direction   y
  sky         z
  up          z
  right       x*image_width/image_height
  look_at     <0.0, 0, 1.2>
  angle       20
}

background {
  color rgb < 0.60, 0.70, 0.95 >
}

plane {
  z, 0

  texture {
    pigment {
      bozo
      color_map {
        [ 0.0 color rgb<0.356, 0.321, 0.274> ]
        [ 0.1 color rgb<0.611, 0.500, 0.500> ]
        [ 0.4 color rgb<0.745, 0.623, 0.623> ]
        [ 1.0 color rgb<0.837, 0.782, 0.745> ]
      }
      warp { turbulence 0.6 }
    }
    finish {
      diffuse 0.6
      ambient 0.1
      specular 0.2
      reflection {
        0.2, 0.6
        fresnel on
      }
      conserve_energy
    }
  }
  
}

#declare Mesh_TextureA=
  texture{
    pigment{
      uv_mapping
      
      spiral2 8
      color_map {
        [0.5 color rgb <0.2,0,0> ]
        [0.5 color rgb 1 ]
      }
      scale 0.8
    }
    finish {
      specular 0.3
      roughness 0.01
    }
  }


#declare Mesh_TextureB=
  texture{
    pigment{
      uv_mapping
      
      spiral2 8
      color_map {
        [0.5 color rgb 1 ]
        [0.5 color rgb <0,0,0.2> ]
      }
      scale 0.8
    }
    finish {
      specular 0.3
      roughness 0.01
    }
  }

// ------- Mesh A - without normal vectors -------     

#declare Mesh_A=
mesh2{
  vertex_vectors{
    14  
    <-1, -1, -1>, < 1, -1, -1>,
    <-1,  1, -1>, < 1,  1, -1>,
    <-1, -1,  1>, < 1, -1,  1>,
    <-1,  1,  1>, < 1,  1,  1>,
    
    -1.5*x, 1.5*x,
    -1.5*y, 1.5*y,
    -1.5*z, 1.5*z,
  }
  normal_vectors{
    14
    <-1, -1, -1>, < 1, -1, -1>,
    <-1,  1, -1>, < 1,  1, -1>,
    <-1, -1,  1>, < 1, -1,  1>,
    <-1,  1,  1>, < 1,  1,  1>,
    -x, x, -y, y, -z, z
  }
  uv_vectors{
    3
    <0.0, 0.0>,
    < 1.0,  1.0>, < 1.0, -1.0>
  }
  face_indices{
    24,
    <8,0,2>,
    <8,0,4>,
    <8,6,2>,
    <8,6,4>,
    
    <9,1,3>,
    <9,1,5>,
    <9,7,3>,
    <9,7,5>,
    
    <10,0,1>,
    <10,0,4>,
    <10,5,1>,
    <10,5,4>,
    
    <11,2,3>,
    <11,2,6>,
    <11,7,3>,
    <11,7,6>,
    
    <12,0,1>,
    <12,0,2>,
    <12,3,1>,
    <12,3,2>,
    
    <13,4,5>,
    <13,4,6>,
    <13,7,5>,
    <13,7,6>
  }
  normal_indices{
    24,
    <8,0,2>,
    <8,0,4>,
    <8,6,2>,
    <8,6,4>,
    
    <9,1,3>,
    <9,1,5>,
    <9,7,3>,
    <9,7,5>,
    
    <10,0,1>,
    <10,0,4>,
    <10,5,1>,
    <10,5,4>,
    
    <11,2,3>,
    <11,2,6>,
    <11,7,3>,
    <11,7,6>,
    
    <12,0,1>,
    <12,0,2>,
    <12,3,1>,
    <12,3,2>,
    
    <13,4,5>,
    <13,4,6>,
    <13,7,5>,
    <13,7,6>
  }
  uv_indices{
    24,
    <0,1,2>,
    <0,1,2>,
    <0,1,2>,
    <0,1,2>,
    
    <0,1,2>,
    <0,1,2>,
    <0,1,2>,
    <0,1,2>,
    
    <0,1,2>,
    <0,1,2>,
    <0,1,2>,
    <0,1,2>,
    
    <0,1,2>,
    <0,1,2>,
    <0,1,2>,
    <0,1,2>,
       
    <0,1,2>,
    <0,1,2>,
    <0,1,2>,
    <0,1,2>,
      
    <0,1,2>,
    <0,1,2>,
    <0,1,2>,
    <0,1,2>
  }
  
}

// ------- Mesh B - with normal vectors -------

#declare Mesh_B=
mesh2{
  vertex_vectors{
    14
    <-1, -1, -1>, < 1, -1, -1>,
    <-1,  1, -1>, < 1,  1, -1>,
    <-1, -1,  1>, < 1, -1,  1>,
    <-1,  1,  1>, < 1,  1,  1>,
    
    -1.5*x, 1.5*x,
    -1.5*y, 1.5*y,
    -1.5*z, 1.5*z,
  }
  uv_vectors{
    5
    <0.0, 0.0>,
    < 1.0,  1.0>, < 1.0, -1.0>
    <-1.0,  1.0>, <-1.0, -1.0>
  }
  face_indices{
    24,
    <8,0,2>,
    <8,0,4>,
    <8,6,2>,
    <8,6,4>,
    
    <9,1,3>,
    <9,1,5>,
    <9,7,3>,
    <9,7,5>,
    
    <10,0,1>,
    <10,0,4>,
    <10,5,1>,
    <10,5,4>,
    
    <11,2,3>,
    <11,2,6>,
    <11,7,3>,
    <11,7,6>,
    
    <12,0,1>,
    <12,0,2>,
    <12,3,1>,
    <12,3,2>,
    
    <13,4,5>,
    <13,4,6>,
    <13,7,5>,
    <13,7,6>
  }
  uv_indices{
    24,
    <0,1,2>,
    <0,1,3>,
    <0,4,2>,
    <0,4,3>,
    
    <0,1,2>,
    <0,1,3>,
    <0,4,2>,
    <0,4,3>,
    
    <0,1,2>,
    <0,1,3>,
    <0,4,2>,
    <0,4,3>,
    
    <0,1,2>,
    <0,1,3>,
    <0,4,2>,
    <0,4,3>,
    
    <0,1,2>,
    <0,1,3>,
    <0,4,2>,
    <0,4,3>,
    
    <0,1,2>,
    <0,1,3>,
    <0,4,2>,
    <0,4,3>
  }
  
}

object {
  Mesh_A
  texture { Mesh_TextureA }
  rotate 180*z
  rotate 90*x
  translate <-2, -2, 1.5>
}

object {
  Mesh_A
  texture { Mesh_TextureB }
  translate < 2,  2, 1.5>
}

object {
  Mesh_B
  texture { Mesh_TextureA }
  rotate 180*z
  rotate 90*x
  translate < 2, -2, 1.5>
}

object {
  Mesh_B
  texture { Mesh_TextureB }
  translate <-2,  2, 1.5>
}
