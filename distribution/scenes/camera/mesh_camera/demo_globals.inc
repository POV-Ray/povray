// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

/* Persistence Of Vision Raytracer sample file.

   Jaime Vives Piqueres, Jan. 2011  <jaime@ignorancia.org> */

/*************************************************************************************
 * $File: //depot/povray/smp/distribution/scenes/camera/mesh_camera/demo_globals.inc $
 * $Revision: #2 $
 * $Change: 5377 $
 * $DateTime: 2011/01/09 19:56:00 $
 * $Author: jholsenback $
 ************************************************************************************/
global_settings{
 radiosity{
  count 80
  error_bound .5
  nearest_count 10
  recursion_limit 1
 }
}
#default{texture{finish{ambient 0 diffuse 1}}}
