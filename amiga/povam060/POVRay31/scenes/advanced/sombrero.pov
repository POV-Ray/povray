// Persistence Of Vision raytracer version 3.1 sample file.
// File: Sombrero.POV
// Vers: 3
// Desc: Create the famous Sinusoidal rippled surface,
//       using a 3-D sheet of spheres.
//       Shows off the use of the while loop for creating a surface,
//       and for smoothly changing colors.
// Date: 10/1/95
// Auth: Eduard Schwan



global_settings { assumed_gamma 1.0 }

// ------------------------------------------------------------------
// Look down at an angle at our creation
camera
{ 
  location  <0,1.5,-2>
  direction 1*z
  look_at   <0,-0.2,0>
} 


// ------------------------------------------------------------------
// Simple background for a simple scene
background { color rgb <0.9, 0.8, 0.7> }


// ------------------------------------------------------------------
// A light source
light_source { <20, 20, -10> colour 1 }


// ------------------------------------------------------------------
// create a simple shape to use as a dot
#declare BasicShape = sphere {  0, 1 }


// ------------------------------------------------------------------
// Set up the loop variables:
// the Xc & Zc variables will go from -1.0 to +1.0
// in NumIterations loops.
#declare NumIterations = 8; // try 6 to 16
#declare Increment     = 1.0/(NumIterations*2);


// ------------------------------------------------------------------
// Create a surface built from our basic shape
// Zc goes from -1 to +1
#declare Zc = -1.0;
#while (Zc<=1.0)
  union
  {

  // Xc goes from -1 to +1
  #declare Xc = -1.0;
  #while (Xc<=1.0)
    // precalculate height, since it is used several places below
    #declare YHeight = sin(sqrt(Xc*Xc+Zc*Zc)*6.28*2);
    object
    {
      BasicShape scale Increment
      translate <Xc, YHeight/4, Zc>
      texture
      {
        // colors change across the object, and also go from black
        // in the valleys to full saturation at the peaks
        pigment { color rgb <Xc/2+1, 1-Zc/2, YHeight/2+1>*YHeight }
        finish { ambient 0.2 specular 0.5 roughness 0.05 reflection 0.2 }
      }
    }
    // manually increment our counter inside the loop
    #declare Xc=Xc+Increment;
  #end
  }

  // manually increment our counters inside the loop
  #declare Zc=Zc+Increment;

#end


