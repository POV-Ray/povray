// Persistence of Vision Ray Tracer POV-Ray 3.1 Sample Scene
// by Chris Young
// MACRO4.POV demonstrates basic use of a macro as a type
// of procedure that "returns" a value via a parameter but
// only when a lone identifier is passed.  If a constant
// or expression is passed, the value cannot be returned.
#include "colors.inc"

light_source { <100,1000,-1000>, White}

camera { location <0,-1,-16> direction 2*z look_at <0,-1,0>}

plane{-z,-10  pigment{checker Cyan,Yellow}}

// Define the macro.  Parameters are:
//   V:  The value to be incremented.  New value
//       is returned via this parameter so it must be
//       a lone identifier.  It cannot be a constant or 
//       an expression.
#macro Inc(V)
  #local V=V+1;
#end

#declare Value=5;

union{
 text{ttf "timrom.ttf" "#macro Inc(V)",0.1,0 translate 2*y}
 text{ttf "timrom.ttf" "  #local V=V+1;",0.1,0 translate y}
 text{ttf "timrom.ttf" "#end",0.1,0 }

 text{ttf "timrom.ttf" concat("#declare Value=",str(Value,0,0),";"),0.1,0 translate -y}

 Inc(Value+0)  // Expression won't work

 text{ttf "timrom.ttf" concat("Inc(Value+0)=",str(Value,0,0)),0.1,0 translate -2*y}

 Inc(+Value)  // This too is an expression so it won't work

 text{ttf "timrom.ttf" concat("Inc(+Value)=",str(Value,0,0)),0.1,0 translate -3*y}

 Inc(Value)   // Lone identifier works.  It accepts return value.

 text{ttf "timrom.ttf" concat("Inc(Value)=",str(Value,0,0)),0.1,0 translate -4*y}

 pigment{Red}
 translate -3.75*x
}
