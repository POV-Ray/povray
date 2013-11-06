// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision Ray Tracer POV-Ray sample Scene
// by Chris Young
// LOCAL.INC demonstrates basic use of a temporary local identifier
// using the #local directive.

// Updated: 10Aug2008 (jh) for v3.7 distribution

#debug "Doing #include \"local.inc\"\n"

// The global version of "Thing" is a red sphere.
#local Thing = box{-1,1 pigment{Blue}}

// Center object in scene is the local blue box.
object{Thing}

// This object will disapear when we exit "local.inc"
#local Local_Item = cylinder{-1,1,1 pigment{Green}}
