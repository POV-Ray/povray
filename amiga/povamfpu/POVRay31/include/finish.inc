#local Finish_Inc_Temp = version;


#ifdef(View_POV_Include_Stack)
#   debug "including finish.inc\n"
#end

/*
              Persistence of Vision Raytracer Version 3.1

             Some basic finishes.  Others may be defined in
                  specific .inc files (see metals.inc).

*/

// Dull creates a large, soft highlight on the object's surface
#declare Dull = finish {specular 0.5 roughness 0.15}

// Shiny creates a small, tight highlight on the object's surface
#declare Shiny = finish {specular 1 roughness 0.001}

// Phong highlights are less "realistic" than specular, but useful
// for different effects.
// Dull creates a large, soft highlight on the object's surface
#declare Phong_Dull = finish {phong 0.5  phong_size 1}

// Shiny creates a small, tight highlight on the object's surface
#declare Phong_Shiny = finish {phong 1  phong_size 200}

// Very shiny with very tight highlights and a fair amount of reflection
#declare Glossy = finish {specular 1 roughness 0.0001 reflection 0.13}
#declare Phong_Glossy = finish {phong 1 phong_size 300 reflection 0.13}

// Luminous for shadowless skies and light_sources.
#declare Luminous = finish {ambient 1  diffuse 0}

// a perfectly mirrored finish with no highlights
#declare Mirror = finish {ambient 0  diffuse 0 reflection 1}

#version Finish_Inc_Temp;
