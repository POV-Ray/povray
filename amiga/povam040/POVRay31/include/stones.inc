#ifdef(Stones_Inc_Temp)
// do nothing
#else
#declare Stones_Inc_Temp = version;

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
