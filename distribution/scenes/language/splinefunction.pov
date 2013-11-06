// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision Ray Tracer Scene Description File
// File: splinefunction.pov
// Desc: A demonstration of using splines in functions.
// This scene uses a spline to make a customized gradient pattern.
// Date: 2001/08/18
// Auth: Ingo Janssen
//
// -w320 -h240
// -w800 -h600 +a0.3
//

#version 3.6;
global_settings {
  assumed_gamma 1.0
}

camera { location <0.0, 0.0, -2.5>
         right     x*image_width/image_height
         look_at 0 
         angle 30 
       }

#declare Spl_Pat = function {
   spline {
     cubic_spline
      -0.001, < 0   , 0  , 0>
       0.25,  < 0.25, 0.5, 0>
       0.5,   < 0.50, 0.1  , 0>
       0.75,  < 0.75, 0.9, 0>
       1.001, < 1   , 0  , 0>
   }
}

#declare P= function {
   pigment {
      function {Spl_Pat(x).y}
   }
}

box {
   0,1
   pigment {function{P(x,y,z).gray}}
   finish {ambient 1}
   translate <-0.5,-0.5,0>
}

#declare I=0;
#declare N=100;
#while(I<N)
   sphere{
      Spl_Pat(I/N),0.01
      translate <-0.5,-0.5,0>
      pigment {rgb <1,0,0>}
      finish {ambient 1}
   }
   #declare I=I+1;
#end
