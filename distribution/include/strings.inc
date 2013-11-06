// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

//    Persistence of Vision Ray Tracer version 3.5 Include File
//    File: strings.inc
//    Last updated: 2001.8.9
//    Description: This file contains various macros for manipulating strings.

#ifndef(STRINGS_INC_TEMP)
#declare STRINGS_INC_TEMP = version;
#version 3.5;

#ifdef(View_POV_Include_Stack)
   #debug "including strings.inc\n"
#end

//converts a color to a string, with filter and transmit
#macro CRGBFTStr(C, MinLen, Padding)
    concat( "color rgbft < ", str(C.red, MinLen, Padding), 
                        ", ", str(C.green, MinLen, Padding),
                        ", ", str(C.blue, MinLen, Padding),
                        ", ", str(C.filter, MinLen, Padding),
                        ", ", str(C.transmit, MinLen, Padding), ">")
#end

//converts a color to a string, without filter and transmit
#macro CRGBStr(C, MinLen, Padding)
    concat( "color rgb <   ", str(C.red, MinLen, Padding), 
                        ", ", str(C.green, MinLen, Padding),
                        ", ", str(C.blue, MinLen, Padding), ">")
#end

#macro Parse_String(String)
    #fopen FOut "parse_string.tmp" write
    #write(FOut,String)
    #fclose FOut
    #include "parse_string.tmp"
#end

//just a shortcut for using str() with system-defined precision and length.
#macro Str(A)
    str(A, 0,-1)
#end

//just a shortcut for using vstr()
//"Str" is with capitalized "S" to match the Str() macro
//above which also has system-defined precision and length.
#macro VStr2D(V)
    concat("<", vstr(2, V, ",", 0,-1), ">")
#end
#macro VStr(V)
    concat("<", vstr(3, V, ",", 0,-1), ">")
#end

//Another shortcut for using vstr(),
//this one with user specified precision and length.
//"str" is with non-capitalized "s" to match the str() function
//in POV-Ray which also has user-specified precision and length.
#macro Vstr2D(V,L,P)
    concat("<", vstr(2, V, ",", L,P), ">")
#end
#macro Vstr(V,L,P)
    concat("<", vstr(3, V, ",", L,P), ">")
#end

//Macros used to generate strings with mesh syntax.
//Can be used with the IO features to write a mesh to an external file.
#macro Triangle_Str(A, B, C)
    concat("triangle {<",   vstr(3, A, ",", 0,-1), ">,<",
                            vstr(3, B, ",", 0,-1), ">,<",
                            vstr(3, C, ",", 0,-1), ">}")
#end
#macro Smooth_Triangle_Str(A, NA, B, NB, C, NC)
    concat("smooth_triangle {<",    vstr(3, A, ",", 0,-1), ">,<",
                                    vstr(3, NA, ",", 0,-1), ">,<",
                                    vstr(3, B, ",", 0,-1), ">,<",
                                    vstr(3, NB, ",", 0,-1), ">,<",
                                    vstr(3, C, ",", 0,-1), ">,<",
                                    vstr(3, NC, ",", 0,-1), ">}")
#end


#version STRINGS_INC_TEMP;
#end//strings.inc

