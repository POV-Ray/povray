// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

/* Persistence Of Vision Raytracer sample file.

   Test objects, lighting and textures common to all demo scenes

   --
   Jaime Vives Piqueres, Jan. 2011  <jaime@ignorancia.org> */

/************************************************************************************
 * $File: //depot/povray/smp/distribution/scenes/camera/mesh_camera/demo_common.inc $
 * $Revision: #2 $
 * $Change: 5377 $
 * $DateTime: 2011/01/09 19:56:00 $
 * $Author: jholsenback $
 ***********************************************************************************/

// if we are not rendering the baking demo, ensure the switch is off
#ifndef(use_baking) 
  #declare use_baking=0;
#end


// *** TEXTURES ***

// vase textures
// + these use the baked texture maps when use_baking is set to 2 (load)
// + reflections are not used when baking, to avoid camera dependant textures
// + in this case I didn't use the uv mapping for the baking step, to allow to bake procedural textures
#declare wm_vase1_auv = 
texture{
#if (use_baking<2)
	pigment{
    agate color_map{Red_Marble_Map}
  }
#else
  uv_mapping
	pigment{
     image_map{png "im_vase1_baked.png" interpolate 2}
  }
  finish{emission 1}
#end
#if (use_baking!=1)
  finish{reflection{0.01,0.99 falloff 5}}
#end
}
#declare wm_vase2_auv = 
texture{
#if (use_baking<2)
	pigment{
    granite color_map{Blood_Marble_Map} 
    turbulence .33 lambda 3 scale 2
  }
#else
  uv_mapping
	pigment{
     image_map{png "im_vase2_baked.png" interpolate 2} 
  }
  finish{emission 1}
#end
#if (use_baking!=1)
  finish{reflection{0.01,0.99 falloff 5}}
#end
}

// room walls texture
// + uses the baked texture map when use_baking is set to 2 (load)
#declare wm_t_room_auv = 
texture{
  uv_mapping
#if (use_baking<2)
	pigment{
     brick color Gray color White mortar .2 scale .005
  }
#else
	pigment{
     image_map{png "im_room_baked.png" interpolate 2}
  }
  finish{emission 1}
#end
}


// *** OBJECTS ***

// vases modeled with Wings3D
// + scale and position declared for use in the mesh camera when baking
#declare scl_vase1=.67;
#declare pos_vase1=<1.1,1.8276*.67,1>+.01*y;
#include "vase1.inc" 
object{vase1 
  scale scl_vase1
  translate pos_vase1
}
#declare scl_vase2=.75;
#declare pos_vase2=<-1,1.8276*.75,0.5>+.01*y;
#include "vase2.inc" 
object{vase2 scale scl_vase2
  translate pos_vase2
}

// test room modeled with Wings3D
// + scale and position declared for use in the mesh camera when baking
#declare scl_room=10;
#declare pos_room=0.8726*10*y;
#include "demo_room.inc"
object{room
  scale scl_room
  translate pos_room
}


// *** LIGHTING ***

// simple two point lighting
#declare lc1=White*20+Gold*5;
#declare lc2=(White+Blue+Green*.5)*12;
// do not use lights when loading the baked textures, emission on the textures will do the job
#if (use_baking<2)
light_source{
  <-2,8,-2>
  rgb lc1
  spotlight radius 90 falloff -90 tightness 1 point_at <0,0,0>
  area_light .5*x,.5*z,4,4 jitter adaptive 1 orient circular
  fade_distance 1
  fade_power 2
}
light_source{
  <1,1,0>*10
  rgb lc2
  spotlight radius 90 falloff -90 tightness 1 point_at <0,1,0>
  area_light 1*x,1*z,4,4 jitter adaptive 1 orient circular
  fade_distance 1
  fade_power 2
}
#end
// something to reflect when there are no lights
// using media to not affect radiosity
sphere{0,1
  pigment{rgbt 1}
  finish{ambient 0 diffuse 0 brilliance 0}
  interior{media{emission lc1}}
  hollow no_shadow
  translate <-2,8,-2>
}
difference{
  sphere{0,1000}
  sphere{0,999}
  pigment{rgbt 1}
  finish{ambient 0 diffuse 0 brilliance 0}
  interior{media{emission lc2}}
  hollow no_shadow
}


