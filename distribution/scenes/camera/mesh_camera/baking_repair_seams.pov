// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

/* Persistence Of Vision Raytracer sample file.

   Mitigates the problem with the visible seams on UV maps
   + used by the bake.sh script to automate the process
  
   --
   Jaime Vives Piqueres, Jan. 2011  <jaime@ignorancia.org> */

/********************************************************************************************
 * $File: //depot/povray/smp/distribution/scenes/camera/mesh_camera/baking_repair_seams.pov $
 * $Revision: #2 $
 * $Change: 5377 $
 * $DateTime: 2011/01/09 19:56:00 $
 * $Author: jholsenback $
 *******************************************************************************************/
#version 3.7;

#declare bake_padding=.005;

#declare t_base_image=
texture{
  pigment{
  #switch (clock)
    #case (1)
      image_map{png "im_vase1_baked.png"}
      #break
    #case (2)
      image_map{png "im_vase2_baked.png"}
      #break
    #case (3)
      image_map{png "im_room_baked.png"}
      #break
  #end
  }
  finish{emission 1 diffuse 0 ambient 0}
}

#declare t_output_image=
texture{t_base_image translate bake_padding*x}
texture{t_base_image translate -bake_padding*x}
texture{t_base_image translate bake_padding*y}
texture{t_base_image translate -bake_padding*y}
texture{t_base_image}

#include "screen.inc"
object{Screen_Plane(texture{t_output_image},1,<0,0,0>,<1,1,0>)}
