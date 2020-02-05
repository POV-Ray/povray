// Persistence of Vision Ray Tracer POV-Ray 3.1 Sample Scene
// by Chris Young
// LOCAL.INC demonstrates basic use of a temporary local identifier
// using the #local directive.
#debug "Doing #include \"local.inc\"\n"

// The global version of "Thing" is a red sphere.
#local Thing = box{-1,1 pigment{Blue}}

// Center object in scene is the local blue box.
object{Thing}

// This object will disapear when we exit "local.inc"
#local Local_Item = cylinder{-1,1,1 pigment{Green}}
