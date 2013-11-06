// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

#ifndef(Stones_Inc_Temp)
#declare Stones_Inc_Temp = version;
#version 3.5;

#ifdef(View_POV_Include_Stack)
#   debug "including stones.inc\n"
#end

/*
              Persistence of Vision Raytracer Version 3.1

           Combines stones1.inc and stones2.inc.  Use only if
              needed, since it will take longer to parse.

*/

#include "stones1.inc"
#include "stones2.inc"

#version Stones_Inc_Temp;
#end
