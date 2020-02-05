// Persistence of Vision Ray Tracer POV-Ray 3.1 Sample Scene
// by Chris Young
// LOCAL.POV demonstrates basic use of a temporary local identifier
// using the #local directive.  Also demonstrates new #undef directive.
// See debug messages after rendering.

#include "colors.inc"

light_source { <100,1000,-1000>, White}

background{rgb 0.4}

camera { location -9*z direction 2*z look_at <0,0,0>}

#declare Thing = sphere{0,1 pigment{Red}}

/* Thing on the left is global */
object{Thing translate -2.5*x}


// This include file declares a local version of "Thing"
// and leaves it at the origin.  
#include "local.inc"


/* Now put "Thing" on the right.  You get the original version.*/
object{Thing translate 2.5*x}


// The file "local.inc" also declares a local
// identifier called "Local_Item" which disapears on exit.

#ifdef (Local_Item)
  #debug "Local_Item found\n"
#else
  #debug "Local_Item not found\n"
#end

// Now let's get rid of the global Thing

#debug "Doing #undef Thing\n"

#undef Thing

#ifdef (Thing)
  #debug "Thing found\n"
#else
  #debug "Thing not found\n"
#end


