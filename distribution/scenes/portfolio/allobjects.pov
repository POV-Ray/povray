// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence of Vision Ray Tracer Scene Description File
// File: allobjects_.pov
// Vers: 3.5
// Desc: Render via allobjects.ini,
//       generates html files and images for all the
//       objects in allobjects_.pov
// Date: 2001/08/08
// Auth: Ingo Janssen
#version 3.6;
global_settings {
  assumed_gamma 1.0
  max_trace_level 10
}

#include "colors.inc"
#include "metals.inc"
#include "functions.inc"
#include "html_gen.inc"


#declare Generate_HTML=yes;
#declare Generate_Images=yes;

#declare str_objectArr=array[27] {
  "bicubic_patch", "blob", "box", "cone", "cubic",
  "cylinder", "disc", "julia_fractal", "height_field",
  "isosurface","lathe","mesh","parametric","plane",
  "polygon", "poly", "prism", "quadric", "quartic",
  "smooth_triangle", "sphere", "sphere_sweep","superellipsoid",
  "sor", "text", "torus", "triangle"
}

#if (Generate_HTML)
   #if (clock=0)                         // generate the html-files for showing the images in.
      #declare FromFileName="allobjects.pov"   // the name of the include file the data came from.
      #declare OutName="allobjects"          // the OutName should match with Output_File_Name in the ini-file!!!!
      #declare Keyword="objects"           // the stuff represented in the array: texture, pigment, material, color etc.
      #declare DataArray=str_objectArr    // the array containing the strings of identifiers
      #declare NumPicHorizonal=3;        // the amount of images per row in the table
      #declare NumPicVertical=2;         // the amount of images per collumn in the table
      #declare IW=image_width;           // the dimesions of the image, these are set in the ini-file!
      #declare IH=image_height;
      #declare Comment=""
      HTMLgen(FromFileName, OutName, Keyword, DataArray, NumPicHorizonal, NumPicVertical, IW, IH, Comment)
   #end
#end

#if(Generate_Images)
   #declare Stream=seed(7);
   #switch(frame_number-1)
      #case(0)//bicubic_patch
         #declare B11=<0,0,3>; #declare B12=<1,0,3>; #declare B13=<2,0,3>; #declare B14=<3,0,3>;   
         #declare B21=<0,0,2>; #declare B22=<1,0,2>; #declare B23=<2,0,2>; #declare B24=<3,2,2>;  
         #declare B31=<0,0,1>; #declare B32=<1,0,1>; #declare B33=<2,0,1>; #declare B34=<3.5,1,1>;    
         #declare B41=<0,-1,0>; #declare B42=<1,0,0>; #declare B43=<2,0,0>; #declare B44=<3,-1,0>;    
         #declare R11=B14; #declare R12=<4,0,3>; #declare R13=<5,0,3>; #declare R14=<6,0,3>;
         #declare R21=B24; #declare R22=<4,0,2>; #declare R23=<5,0,2>; #declare R24=<6,0,2>;
         #declare R31=B34; #declare R32=<4,0,1>; #declare R33=<5,0,1>; #declare R34=<6,0,1>;
         #declare R41=B44; #declare R42=<4,0,0>; #declare R43=<5,0,0>; #declare R44=<6,0,0>;
         #declare R12=B14+(B14-B13); 
         #declare R22=B24+(B24-B23); 
         #declare R32=B34+(B34-B33); 
         #declare R42=B44+(B44-B43); 
         union {
            bicubic_patch {
               type 1 flatness 0.001
               u_steps 4 v_steps 4
               uv_vectors
               <0,0> <1,0> <1,1> <0,1>
               B11, B12, B13, B14
               B21, B22, B23, B24
               B31, B32, B33, B34
               B41, B42, B43, B44
               uv_mapping
               texture {
                  pigment {
                     checker 
                     color rgbf <1,1,1,0.5> 
                     color rgbf <0,0,1,0.7> 
                     scale 1/3
                  }
                  finish {phong 0.6 phong_size 20}
               }
               no_shadow
            }
            bicubic_patch {
               type 1 flatness 0.001
               u_steps 4 v_steps 4
               uv_vectors
               <0,0> <1,0> <1,1> <0,1>
               R11, R12, R13, R14
               R21, R22, R23, R24
               R31, R32, R33, R34
               R41, R42, R43, R44
               uv_mapping
               texture {
                  pigment {
                     checker 
                     color rgbf <1,1,1,0.5> 
                     color rgbf <1,0,0,0.7> 
                     scale 1/3
                  }
                  finish {phong 0.6 phong_size 20}
               }
               no_shadow
            }
            rotate <-45,10,0>
            translate<-3.5,-0.5,0.5>
         }
      #break
      #case(1)//blob
         #declare N=14;
         #declare I=0;
         blob {
            threshold .65
            #while (I<N)
               sphere { <2,0,0>, .75, 1 rotate<0,0,(360/N)*I> pigment {rgb <rand(Stream),rand(Stream),rand(Stream)>} }
               #declare I=I+1;
            #end 
            cylinder { <-2.5,-2.5,0>, <2.5,2.5,0>, .5, 1 pigment{rgb<1,0,0>}}
            cylinder { <-2.5,2.5,0>, <2.5,-2.5,0>, .5, 1 pigment{rgb<0,0,1>}}
            finish { phong 1 phong_size 30 }
         }
      #break
      #case(2)//box
         #declare I=0;
         #declare N=14;
         union {
            #while (I<N)
               box {
                  -1,1
                  scale 0.2
                  rotate <rand(Stream)*360,rand(Stream)*360,rand(Stream)*360>
                  translate <1,2.5,0>
                  rotate <(360/N)*I,0,0>
                  pigment {rgb <rand(Stream),rand(Stream),rand(Stream)>}
               }
               #declare I=I+1;
            #end
            box{-1,1 pigment{rgb 1}}
            rotate <45,45,45>
         }
      #break
      #case(3)//cone
         #declare I=0;
         #declare N=12;
         union {   
            #while (I<N)
               cone {
                  y, rand(Stream)*0.4, 0, 0.5
                  translate <2,0,0>
                  rotate <0,I*360/N,0>
                  pigment {rgb 1}
                  finish {phong 1 phong_size 5}
               }
               #declare I=I+1;
               #end
               cone {2.5*y,0, 0, 1.5 pigment {rgb 1} finish{reflection{0.2,0.4}}}
               rotate <-45,0,0>
            }
      #break
      #case(4)//cubic
         cubic {
           <
         // x^3,      x^2y,     x^2z,     x^2,
            2,        4,        0,        0,
         // xy^2,     xyz,      xy,       xz^2,
            0,        1,        0,        0,
         // xz,       x,        y^3,      y^2z,
            1.1,        1,        2,        3,
         // y^2,      yz^2,     yz,       y,
            0,        1,        0,        0,
         // z^3,      z^2,      z,        C
            0,        -0.7,        0.5,       13.
           >
           sturm
           scale 0.2
           rotate <0,90,0>
           rotate <-20,0,0>
           pigment {rgb 1}
           finish {phong 0.8 phong_size 20}
         }
      #break
      #case(5)//cylinder
         #declare A=0.75;
         #declare Step=0.3; 
         #declare Theta=0;
         #declare Revs1=3*pi;
         #declare Revs2=3*pi;
         union {
            #while (Theta<Revs2)
               #declare R=sqrt(A*A*Theta);
               #declare P=vrotate(x*R,z*degrees(Theta));
               #declare TR=(Theta/Revs2);
               #if(Theta<Revs1)
                  cylinder{
                     -P,(-P+<0,0,-1.2+TR>),0.15
                     pigment {rgb 1-<0,TR,TR>}
                  }
               #end
               #if (Theta !=0)
                  cylinder{
                     P,(P+<0,0,-1.2+TR>),0.15
                     pigment {rgb 1-<TR,TR,0>}
                  }
               #end
               #declare Theta=Theta+Step;
            #end
            cylinder {
               <0,0,0>, <0,0,10>, 2.6
               pigment {rgb 1}
               finish {reflection{0.81,0.8}}
            } 
            rotate <20,0,0>
            rotate <0,-25,0>
            translate <0.5,0.5,0>
         } 
      #break
      #case(6)
         #declare I=0;
         #declare N=8;
         union {
            #while (I<N)
               disc {
                  <0, 1, 0>  z, 1+(I/N), 0.5+(I/N)
                  translate <(-2+(I*4/N)), (-2+(I*4/N)), (-2+(I*4/N))>
                  texture {
                     pigment {rgb <rand(Stream),rand(Stream),rand(Stream)>}
                     normal {bozo scale 0.5}
                     finish {phong 1 phong_size 10}
                  }
               }
               #declare I=I+1;
            #end
            disc {   
               <0,0,0>, y, 3 
               translate <1.25,-3,3.5>
               pigment {checker color rgb 1 color rgb <0,0,1>}
            }
         }
         
      #break
      #case(7)
         julia_fractal {
            <-0.033,-0.6,-0.93,-0.025>
            quaternion 
            cube
            max_iteration 4
            precision 50
            translate <0,0.2,0> 
            scale 2.5
            rotate <0,-5,0>
            pigment {rgb 1}
         }
      #break
      #case(8)
         #declare Landscape=pigment {
            gradient x
            sine_wave
            turbulence 0.7
            octaves 7
            lambda 0.5
            omega 0.5
            scale 0.2
         }
         height_field {
            function 100,100 {
               pigment{Landscape}
            }
            smooth
            translate -0.5
            scale <50,5,50>
            rotate <-30,0,0>
            translate <0,-3,55>
            pigment {rgb 1}
            finish {specular 0.1 roughness 2}
         }
      #break
      #case(9)
         #declare Landscape=function {
            pigment {
               wood
               scale 0.5
               turbulence 0.3
               octaves 7
               lambda 0.5
               omega 0.5
               color_map {
                  [0, rgb 0]
                  [1, rgb 1]
               }
            }
         }
         isosurface {
            function{f_sphere(x,y,z,1)-(Landscape(x,y,z).gray)*0.5}
            contained_by {sphere {0,1.5}}
            max_gradient 8.607
            pigment {rgb 1}
            finish {specular 0.1 roughness 2}
            scale 1.5
         }
      #break
      #case(10)
         lathe{
         	cubic_spline
         	12,
         	<0.542658,0.731408>,
         	<0.401095,0.943752>,
         	<0.306719,0.990940>,
         	<0.212344,0.990940>,
         	<0.117969,0.896565>,
         	<0.141563,0.731408>,
         	<0.519064,0.448282>,
         	<0.637033,0.306719>,
         	<0.353907,0.235938>,
         	<0.117969,0.000000>,
         	<0.047188,0.353907>,
         	<0.047188,0.401095>
         	translate <0,-0.5,0>
         	scale 3
         	rotate <25,-45,0>
         	uv_mapping
            pigment {
         	   checker 
         	   color rgbf <1,1,1,0.5> 
               color rgbf <1,0,0,0.7> 
               scale <0.1,0.25,0.25>
            }
            finish {specular 0.1 roughness 2}
         }
         lathe{
         	cubic_spline
         	20,
         	<3.794117,0.470588>,
         	<3.823529,0.529412>,
         	<3.911765,0.558823>,
         	<4.029412,0.558823>,
         	<4.058823,0.735294>,
         	<4.088235,1.000000>,
         	<4.117647,0.735294>,
         	<4.176470,0.558823>,
         	<4.294117,0.558823>,
         	<4.382353,0.470588>,
         	<4.411764,0.352941>,
         	<4.235294,0.382353>,
         	<4.176470,0.294118>,
         	<4.088235,0.000000>,
         	<4.000000,0.294118>,
         	<3.911765,0.382353>,
         	<3.764706,0.352941>,
         	<3.794117,0.470588>,
         	<3.823529,0.529412>,
         	<3.911765,0.558823>
         	rotate <-65,45,0>
         	translate <-0.5,1,0>
         	uv_mapping
         	pigment {checker color rgb 1 color rgb <0,0,1>scale <0.01,0.05,0.05>}
            finish {specular 0.1 roughness 2}
         }
      #break
      #case(11)
         #declare Surface = mesh {
           smooth_triangle {
             <0.561619,0,-1> <0.732723,-0.240315,0.636684> 
             <0.158893,0.489021,-1> <-0.0845312,0.817068,0.570311> 
             <0.229865,0.707453,-0.6> <0.616422,0.787124,0.0214246> 
             uv_vectors
             <0,-1> <1.25664,-1> <1.25664,-0.6> 
           }
           smooth_triangle {
             <0.561619,0,-1> <0.732723,-0.240315,0.636684> 
             <0.229865,0.707453,-0.6> <0.616422,0.787124,0.0214246> 
             <0.65852,0,-0.6> <0.865618,0.0758127,-0.494932> 
             uv_vectors
             <0,-1> <1.25664,-0.6> <0,-0.6> 
           }
           smooth_triangle {
             <0.158893,0.489021,-1> <-0.0845312,0.817068,0.570311> 
             <-0.397883,0.289079,-1> <-0.285623,0.95593,-0.0679491> 
             <-0.583435,0.42389,-0.6> <-0.956835,-0.245365,-0.155768> 
             uv_vectors
             <1.25664,-1> <2.51327,-1> <2.51327,-0.6> 
           }
           smooth_triangle {
             <0.158893,0.489021,-1> <-0.0845312,0.817068,0.570311> 
             <-0.583435,0.42389,-0.6> <-0.956835,-0.245365,-0.155768> 
             <0.229865,0.707453,-0.6> <0.616422,0.787124,0.0214246> 
             uv_vectors
             <1.25664,-1> <2.51327,-0.6> <1.25664,-0.6> 
           }
           smooth_triangle {
             <-0.397883,0.289079,-1> <-0.285623,0.95593,-0.0679491> 
             <-0.445518,-0.323688,-1> <-0.0347631,-0.964436,-0.262019> 
             <-0.531893,-0.386443,-0.6> <-0.132985,-0.990974,-0.0168757> 
             uv_vectors
             <2.51327,-1> <3.76991,-1> <3.76991,-0.6> 
           }
           smooth_triangle {
             <-0.397883,0.289079,-1> <-0.285623,0.95593,-0.0679491> 
             <-0.531893,-0.386443,-0.6> <-0.132985,-0.990974,-0.0168757> 
             <-0.583435,0.42389,-0.6> <-0.956835,-0.245365,-0.155768> 
             uv_vectors
             <2.51327,-1> <3.76991,-0.6> <2.51327,-0.6> 
           }
           smooth_triangle {
             <-0.445518,-0.323688,-1> <-0.0347631,-0.964436,-0.262019> 
             <0.148955,-0.458435,-1> <0.0320451,-0.924809,-0.37908> 
             <0.147272,-0.453256,-0.6> <-0.44191,-0.873686,-0.203443> 
             uv_vectors
             <3.76991,-1> <5.02655,-1> <5.02655,-0.6> 
           }
           smooth_triangle {
             <-0.445518,-0.323688,-1> <-0.0347631,-0.964436,-0.262019> 
             <0.147272,-0.453256,-0.6> <-0.44191,-0.873686,-0.203443> 
             <-0.531893,-0.386443,-0.6> <-0.132985,-0.990974,-0.0168757> 
             uv_vectors
             <3.76991,-1> <5.02655,-0.6> <3.76991,-0.6> 
           }
           smooth_triangle {
             <0.148955,-0.458435,-1> <0.0320451,-0.924809,-0.37908> 
             <0.584502,-1.43157e-016,-1> <0.773102,-0.628832,0.0829675> 
             <0.530369,-1.29899e-016,-0.6> <0.781801,-0.612988,0.114162> 
             uv_vectors
             <5.02655,-1> <6.28319,-1> <6.28319,-0.6> 
           }
           smooth_triangle {
             <0.148955,-0.458435,-1> <0.0320451,-0.924809,-0.37908> 
             <0.530369,-1.29899e-016,-0.6> <0.781801,-0.612988,0.114162> 
             <0.147272,-0.453256,-0.6> <-0.44191,-0.873686,-0.203443> 
             uv_vectors
             <5.02655,-1> <6.28319,-0.6> <5.02655,-0.6> 
           }
           smooth_triangle {
             <0.65852,0,-0.6> <0.865618,0.0758127,-0.494932> 
             <0.229865,0.707453,-0.6> <0.616422,0.787124,0.0214246> 
             <0.197502,0.607848,-0.2> <0.475101,0.638272,0.605712> 
             uv_vectors
             <0,-0.6> <1.25664,-0.6> <1.25664,-0.2> 
           }
           smooth_triangle {
             <0.65852,0,-0.6> <0.865618,0.0758127,-0.494932> 
             <0.197502,0.607848,-0.2> <0.475101,0.638272,0.605712> 
             <0.720629,0,-0.2> <0.893232,0.401734,0.201858> 
             uv_vectors
             <0,-0.6> <1.25664,-0.2> <0,-0.2> 
           }
           smooth_triangle {
             <0.229865,0.707453,-0.6> <0.616422,0.787124,0.0214246> 
             <-0.583435,0.42389,-0.6> <-0.956835,-0.245365,-0.155768> 
             <-0.561656,0.408067,-0.2> <-0.97664,-0.14052,0.162567> 
             uv_vectors
             <1.25664,-0.6> <2.51327,-0.6> <2.51327,-0.2> 
           }
           smooth_triangle {
             <0.229865,0.707453,-0.6> <0.616422,0.787124,0.0214246> 
             <-0.561656,0.408067,-0.2> <-0.97664,-0.14052,0.162567> 
             <0.197502,0.607848,-0.2> <0.475101,0.638272,0.605712> 
             uv_vectors
             <1.25664,-0.6> <2.51327,-0.2> <1.25664,-0.2> 
           }
           smooth_triangle {
             <-0.583435,0.42389,-0.6> <-0.956835,-0.245365,-0.155768> 
             <-0.531893,-0.386443,-0.6> <-0.132985,-0.990974,-0.0168757> 
             <-0.408835,-0.297036,-0.2> <-0.749137,-0.20608,-0.629544> 
             uv_vectors
             <2.51327,-0.6> <3.76991,-0.6> <3.76991,-0.2> 
           }
           smooth_triangle {
             <-0.583435,0.42389,-0.6> <-0.956835,-0.245365,-0.155768> 
             <-0.408835,-0.297036,-0.2> <-0.749137,-0.20608,-0.629544> 
             <-0.561656,0.408067,-0.2> <-0.97664,-0.14052,0.162567> 
             uv_vectors
             <2.51327,-0.6> <3.76991,-0.2> <2.51327,-0.2> 
           }
           smooth_triangle {
             <-0.531893,-0.386443,-0.6> <-0.132985,-0.990974,-0.0168757> 
             <0.147272,-0.453256,-0.6> <-0.44191,-0.873686,-0.203443> 
             <0.178376,-0.548985,-0.2> <0.176189,-0.936466,0.303297> 
             uv_vectors
             <3.76991,-0.6> <5.02655,-0.6> <5.02655,-0.2> 
           }
           smooth_triangle {
             <-0.531893,-0.386443,-0.6> <-0.132985,-0.990974,-0.0168757> 
             <0.178376,-0.548985,-0.2> <0.176189,-0.936466,0.303297> 
             <-0.408835,-0.297036,-0.2> <-0.749137,-0.20608,-0.629544> 
             uv_vectors
             <3.76991,-0.6> <5.02655,-0.2> <3.76991,-0.2> 
           }
           smooth_triangle {
             <0.147272,-0.453256,-0.6> <-0.44191,-0.873686,-0.203443> 
             <0.530369,-1.29899e-016,-0.6> <0.781801,-0.612988,0.114162> 
             <0.495055,-1.21249e-016,-0.2> <0.891028,-0.356262,-0.281329> 
             uv_vectors
             <5.02655,-0.6> <6.28319,-0.6> <6.28319,-0.2> 
           }
           smooth_triangle {
             <0.147272,-0.453256,-0.6> <-0.44191,-0.873686,-0.203443> 
             <0.495055,-1.21249e-016,-0.2> <0.891028,-0.356262,-0.281329> 
             <0.178376,-0.548985,-0.2> <0.176189,-0.936466,0.303297> 
             uv_vectors
             <5.02655,-0.6> <6.28319,-0.2> <5.02655,-0.2> 
           }
           smooth_triangle {
             <0.720629,0,-0.2> <0.893232,0.401734,0.201858> 
             <0.197502,0.607848,-0.2> <0.475101,0.638272,0.605712> 
             <0.192586,0.592719,0.2> <0.390336,0.91867,0.060692> 
             uv_vectors
             <0,-0.2> <1.25664,-0.2> <1.25664,0.2> 
           }
           smooth_triangle {
             <0.720629,0,-0.2> <0.893232,0.401734,0.201858> 
             <0.192586,0.592719,0.2> <0.390336,0.91867,0.060692> 
             <0.550128,0,0.2> <0.920694,0.10818,0.374993> 
             uv_vectors
             <0,-0.2> <1.25664,0.2> <0,0.2> 
           }
           smooth_triangle {
             <0.197502,0.607848,-0.2> <0.475101,0.638272,0.605712> 
             <-0.561656,0.408067,-0.2> <-0.97664,-0.14052,0.162567> 
             <-0.493653,0.35866,0.2> <-0.837728,0.153265,0.524139> 
             uv_vectors
             <1.25664,-0.2> <2.51327,-0.2> <2.51327,0.2> 
           }
           smooth_triangle {
             <0.197502,0.607848,-0.2> <0.475101,0.638272,0.605712> 
             <-0.493653,0.35866,0.2> <-0.837728,0.153265,0.524139> 
             <0.192586,0.592719,0.2> <0.390336,0.91867,0.060692> 
             uv_vectors
             <1.25664,-0.2> <2.51327,0.2> <1.25664,0.2> 
           }
           smooth_triangle {
             <-0.561656,0.408067,-0.2> <-0.97664,-0.14052,0.162567> 
             <-0.408835,-0.297036,-0.2> <-0.749137,-0.20608,-0.629544> 
             <-0.623399,-0.452926,0.2> <-0.289266,-0.939784,0.182017> 
             uv_vectors
             <2.51327,-0.2> <3.76991,-0.2> <3.76991,0.2> 
           }
           smooth_triangle {
             <-0.561656,0.408067,-0.2> <-0.97664,-0.14052,0.162567> 
             <-0.623399,-0.452926,0.2> <-0.289266,-0.939784,0.182017> 
             <-0.493653,0.35866,0.2> <-0.837728,0.153265,0.524139> 
             uv_vectors
             <2.51327,-0.2> <3.76991,0.2> <2.51327,0.2> 
           }
           smooth_triangle {
             <-0.408835,-0.297036,-0.2> <-0.749137,-0.20608,-0.629544> 
             <0.178376,-0.548985,-0.2> <0.176189,-0.936466,0.303297> 
             <0.16627,-0.511725,0.2> <0.754408,-0.649295,-0.0963537> 
             uv_vectors
             <3.76991,-0.2> <5.02655,-0.2> <5.02655,0.2> 
           }
           smooth_triangle {
             <-0.408835,-0.297036,-0.2> <-0.749137,-0.20608,-0.629544> 
             <0.16627,-0.511725,0.2> <0.754408,-0.649295,-0.0963537> 
             <-0.623399,-0.452926,0.2> <-0.289266,-0.939784,0.182017> 
             uv_vectors
             <3.76991,-0.2> <5.02655,0.2> <3.76991,0.2> 
           }
           smooth_triangle {
             <0.178376,-0.548985,-0.2> <0.176189,-0.936466,0.303297> 
             <0.495055,-1.21249e-016,-0.2> <0.891028,-0.356262,-0.281329> 
             <0.504919,-1.23665e-016,0.2> <0.846239,0.440164,0.300225> 
             uv_vectors
             <5.02655,-0.2> <6.28319,-0.2> <6.28319,0.2> 
           }
           smooth_triangle {
             <0.178376,-0.548985,-0.2> <0.176189,-0.936466,0.303297> 
             <0.504919,-1.23665e-016,0.2> <0.846239,0.440164,0.300225> 
             <0.16627,-0.511725,0.2> <0.754408,-0.649295,-0.0963537> 
             uv_vectors
             <5.02655,-0.2> <6.28319,0.2> <5.02655,0.2> 
           }
           smooth_triangle {
             <0.550128,0,0.2> <0.920694,0.10818,0.374993> 
             <0.192586,0.592719,0.2> <0.390336,0.91867,0.060692> 
             <0.169239,0.520863,0.6> <0.860605,0.4749,0.183924> 
             uv_vectors
             <0,0.2> <1.25664,0.2> <1.25664,0.6> 
           }
           smooth_triangle {
             <0.550128,0,0.2> <0.920694,0.10818,0.374993> 
             <0.169239,0.520863,0.6> <0.860605,0.4749,0.183924> 
             <0.485191,0,0.6> <0.674063,-0.719438,0.167473> 
             uv_vectors
             <0,0.2> <1.25664,0.6> <0,0.6> 
           }
           smooth_triangle {
             <0.192586,0.592719,0.2> <0.390336,0.91867,0.060692> 
             <-0.493653,0.35866,0.2> <-0.837728,0.153265,0.524139> 
             <-0.464813,0.337706,0.6> <-0.943194,-0.00259578,0.332232> 
             uv_vectors
             <1.25664,0.2> <2.51327,0.2> <2.51327,0.6> 
           }
           smooth_triangle {
             <0.192586,0.592719,0.2> <0.390336,0.91867,0.060692> 
             <-0.464813,0.337706,0.6> <-0.943194,-0.00259578,0.332232> 
             <0.169239,0.520863,0.6> <0.860605,0.4749,0.183924> 
             uv_vectors
             <1.25664,0.2> <2.51327,0.6> <1.25664,0.6> 
           }
           smooth_triangle {
             <-0.493653,0.35866,0.2> <-0.837728,0.153265,0.524139> 
             <-0.623399,-0.452926,0.2> <-0.289266,-0.939784,0.182017> 
             <-0.384165,-0.279112,0.6> <-0.757635,-0.07908,-0.64787> 
             uv_vectors
             <2.51327,0.2> <3.76991,0.2> <3.76991,0.6> 
           }
           smooth_triangle {
             <-0.493653,0.35866,0.2> <-0.837728,0.153265,0.524139> 
             <-0.384165,-0.279112,0.6> <-0.757635,-0.07908,-0.64787> 
             <-0.464813,0.337706,0.6> <-0.943194,-0.00259578,0.332232> 
             uv_vectors
             <2.51327,0.2> <3.76991,0.6> <2.51327,0.6> 
           }
           smooth_triangle {
             <-0.623399,-0.452926,0.2> <-0.289266,-0.939784,0.182017> 
             <0.16627,-0.511725,0.2> <0.754408,-0.649295,-0.0963537> 
             <0.158111,-0.486615,0.6> <0.697163,-0.647575,-0.307587> 
             uv_vectors
             <3.76991,0.2> <5.02655,0.2> <5.02655,0.6> 
           }
           smooth_triangle {
             <-0.623399,-0.452926,0.2> <-0.289266,-0.939784,0.182017> 
             <0.158111,-0.486615,0.6> <0.697163,-0.647575,-0.307587> 
             <-0.384165,-0.279112,0.6> <-0.757635,-0.07908,-0.64787> 
             uv_vectors
             <3.76991,0.2> <5.02655,0.6> <3.76991,0.6> 
           }
           smooth_triangle {
             <0.16627,-0.511725,0.2> <0.754408,-0.649295,-0.0963537> 
             <0.504919,-1.23665e-016,0.2> <0.846239,0.440164,0.300225> 
             <0.519352,-1.272e-016,0.6> <0.973941,-0.223504,0.0385486> 
             uv_vectors
             <5.02655,0.2> <6.28319,0.2> <6.28319,0.6> 
           }
           smooth_triangle {
             <0.16627,-0.511725,0.2> <0.754408,-0.649295,-0.0963537> 
             <0.519352,-1.272e-016,0.6> <0.973941,-0.223504,0.0385486> 
             <0.158111,-0.486615,0.6> <0.697163,-0.647575,-0.307587> 
             uv_vectors
             <5.02655,0.2> <6.28319,0.6> <5.02655,0.6> 
           }
           smooth_triangle {
             <0.485191,0,0.6> <0.674063,-0.719438,0.167473> 
             <0.169239,0.520863,0.6> <0.860605,0.4749,0.183924> 
             <0.151125,0.465115,1> <0.334519,0.905031,-0.262709> 
             uv_vectors
             <0,0.6> <1.25664,0.6> <1.25664,1> 
           }
           smooth_triangle {
             <0.485191,0,0.6> <0.674063,-0.719438,0.167473> 
             <0.151125,0.465115,1> <0.334519,0.905031,-0.262709> 
             <0.524585,0,1> <0.827168,0.394773,-0.399934> 
             uv_vectors
             <0,0.6> <1.25664,1> <0,1> 
           }
           smooth_triangle {
             <0.169239,0.520863,0.6> <0.860605,0.4749,0.183924> 
             <-0.464813,0.337706,0.6> <-0.943194,-0.00259578,0.332232> 
             <-0.429329,0.311926,1> <-0.970612,0.194321,-0.141954> 
             uv_vectors
             <1.25664,0.6> <2.51327,0.6> <2.51327,1> 
           }
           smooth_triangle {
             <0.169239,0.520863,0.6> <0.860605,0.4749,0.183924> 
             <-0.429329,0.311926,1> <-0.970612,0.194321,-0.141954> 
             <0.151125,0.465115,1> <0.334519,0.905031,-0.262709> 
             uv_vectors
             <1.25664,0.6> <2.51327,1> <1.25664,1> 
           }
           smooth_triangle {
             <-0.464813,0.337706,0.6> <-0.943194,-0.00259578,0.332232> 
             <-0.384165,-0.279112,0.6> <-0.757635,-0.07908,-0.64787> 
             <-0.447154,-0.324877,1> <-0.285333,-0.900234,0.328883> 
             uv_vectors
             <2.51327,0.6> <3.76991,0.6> <3.76991,1> 
           }
           smooth_triangle {
             <-0.464813,0.337706,0.6> <-0.943194,-0.00259578,0.332232> 
             <-0.447154,-0.324877,1> <-0.285333,-0.900234,0.328883> 
             <-0.429329,0.311926,1> <-0.970612,0.194321,-0.141954> 
             uv_vectors
             <2.51327,0.6> <3.76991,1> <2.51327,1> 
           }
           smooth_triangle {
             <-0.384165,-0.279112,0.6> <-0.757635,-0.07908,-0.64787> 
             <0.158111,-0.486615,0.6> <0.697163,-0.647575,-0.307587> 
             <0.17901,-0.550936,1> <0.980628,-0.195812,0.00511105> 
             uv_vectors
             <3.76991,0.6> <5.02655,0.6> <5.02655,1> 
           }
           smooth_triangle {
             <-0.384165,-0.279112,0.6> <-0.757635,-0.07908,-0.64787> 
             <0.17901,-0.550936,1> <0.980628,-0.195812,0.00511105> 
             <-0.447154,-0.324877,1> <-0.285333,-0.900234,0.328883> 
             uv_vectors
             <3.76991,0.6> <5.02655,1> <3.76991,1> 
           }
           smooth_triangle {
             <0.158111,-0.486615,0.6> <0.697163,-0.647575,-0.307587> 
             <0.519352,-1.272e-016,0.6> <0.973941,-0.223504,0.0385486> 
             <0.494957,-1.21226e-016,1> <0.691586,-0.720333,0.053189> 
             uv_vectors
             <5.02655,0.6> <6.28319,0.6> <6.28319,1> 
           }
           smooth_triangle {
             <0.158111,-0.486615,0.6> <0.697163,-0.647575,-0.307587> 
             <0.494957,-1.21226e-016,1> <0.691586,-0.720333,0.053189> 
             <0.17901,-0.550936,1> <0.980628,-0.195812,0.00511105> 
             uv_vectors
             <5.02655,0.6> <6.28319,1> <5.02655,1> 
           }
         }
         object{ 
            Surface
            scale 2.2
            uv_mapping
            pigment {
         	   checker 
         	   color rgbf <1,1,1,0.5> 
               color rgbf <0,0,1,0.7> 
               scale <0.2,0.1,0.1>
            }
            finish {specular 0.2 roughness 20}
            rotate <90,0,0>
            rotate<0,35,0>
         }
      #break
      #case(12)
         parametric {
            function{cos(u)+v*cos(u/2)*cos(u)}
            function{sin(u)+v*cos(u/2)*sin(u)}
            function{v*sin(u/2)}              
            <0,-0.3>,<2*pi,0.3>
            contained_by{box{<-2,-2,-2>,<2,2,2>}}
            accuracy 0.001
            precompute 20,x,y,z
            rotate <0,33,0>
            translate <0,0,-3>
            pigment {rgb 1}
            finish {specular 0.4}
         }
      #break
      #case(13)
         plane {
            y,-3
            no_shadow
            pigment {checker color rgb 1 color rgb <0,0,1> scale 3}
         }
         plane {
            x, -3
            no_shadow
            pigment {checker color rgb 1 color rgb <0,0,1> scale 3}
         }
         plane {
            -x, -50
            no_shadow
            pigment {checker color rgb 1 color rgb <0,0,1> scale 3 }
            rotate <0,-45,0>
         }
      #break
      #case(14)
         polygon {
             30,
             <-0.8, 0.0>, <-0.8, 1.0>,    // Letter "P"
             <-0.3, 1.0>, <-0.3, 0.5>,    // outer shape
             <-0.7, 0.5>, <-0.7, 0.0>,
             <-0.8, 0.0>,
             <-0.7, 0.6>, <-0.7, 0.9>,    // hole
             <-0.4, 0.9>, <-0.4, 0.6>,
             <-0.7, 0.6>
             <-0.25, 0.0>, <-0.25, 1.0>,  // Letter "O"
             < 0.25, 1.0>, < 0.25, 0.0>,  // outer shape
             <-0.25, 0.0>,
             <-0.15, 0.1>, <-0.15, 0.9>,  // hole
             < 0.15, 0.9>, < 0.15, 0.1>,
             <-0.15, 0.1>,
             <0.45, 0.0>, <0.30, 1.0>,    // Letter "V"
             <0.40, 1.0>, <0.55, 0.1>,
             <0.70, 1.0>, <0.80, 1.0>,
             <0.65, 0.0>,
             <0.45, 0.0>
             translate <0.2,-0.5,-0.5>
             scale <4,3,1>
             rotate <60,0,0>
             rotate <0,-45,0>
             pigment { color rgb <1, 0, 0> }
           }
      #break
      #case(15)
         // create an Nth order infinite polynomial surface
         // poly { N <a,b,c...> [sturm] }
         // N = order of poly, M terms where M = (N+1)*(N+2)*(N+3)/6
         poly {
           5, // order of polynomial (2...7)
           <
         // x^5,        x^4y,       x^4z,       x^4,
            1,          0,          0,          0,
         // x^3y^2,     x^3yz,      x^3y,       x^3z^2,
            0,          1,          0,          0,
         // x^3z,       x^3,        x^2y^3,     x^2y^2z,
            0,          3,          0,          0,
         // x^2y^2,     x^2yz^2,    x^2yz,      x^2y,
            0,          0,          0,          0,
         // x^2z^3,     x^2z^2,     x^2z,       x^2,
            0.2,          0,          3,          0.7,
         // xy^4,       xy^3z,      xy^3,       xy^2z^2,
            0,          -1,          -0.3,          0,
         // xy^2z,      xy^2,       xyz^3,      xyz^2,
            0,          3,          0.7,          0,
         // xyz,        xy,         xz^4,       xz^3,
            0,          0,          0,          0,
         // xz^2,       xz,         x,          y^5,
            0,          0,          0,          0,
         // y^4z,       y^4,        y^3z^2,     y^3z,
            0,          0,          -0.9,          0,
         // y^3,        y^2z^3,     y^2z^2,     y^2z,
            0,          0,          0,          0,
         // y^2,        yz^4,       yz^3,       yz^2,
            0,          0,          0,          0,
         // yz,         y,          z^5,        z^4,
            0,          0.4,          0,          0,
         // z^3,        z^2,        z,          C           
            0,          0,          0,          0-0.33
           >
           sturm
           pigment {rgb 1}
           finish {specular 0.1 roughness 2}
         }
      #break
      #case(16)
         prism {
            linear_sweep
            bezier_spline 0,0.25,24
            <0,-1><0,-1><0.5,-0.3><0.7,-0.1>
            <0.7,-0.1><0.9,0.1><0.9,0.6><0.5,0.6>
            <0.5,0.6><0.1,0.6><0.1,0.2><0,0.2>
            <0,0.2><-0.1,0.2><-0.1,0.6><-0.5,0.6>
            <-0.5,0.6><-0.9,0.6><-0.9,0.1><-0.7,-0.1>
            <-0.7,-0.1><-0.5,-0.3><0,-1><0,-1>
            sturm
            pigment {color rgb <1,0,0>}
            rotate<90,0,0>
            translate<0,-1,0>
            rotate<35,0,1>
            scale 1.5
         }
         prism {
            linear_sweep
            bezier_spline 0,0.25,48
            <0,-1><0,-1><0.5,-0.3><0.7,-0.1>
            <0.7,-0.1><0.9,0.1><0.9,0.6><0.5,0.6>
            <0.5,0.6><0.1,0.6><0.1,0.2><0,0.2>
            <0,0.2><-0.1,0.2><-0.1,0.6><-0.5,0.6>
            <-0.5,0.6><-0.9,0.6><-0.9,0.1><-0.7,-0.1>
            <-0.7,-0.1><-0.5,-0.3><0,-1><0,-1>
            
            <0,-0.6><0,-0.6><0.25,-0.25><0.35,-0.15>
            <0.35,-0.15><0.45,-0.05><0.45,0.2><0.25,0.2>
            <0.25,0.2><0.05,0.2><0.05,0><0,0>
            <0,0><-0.05,0><-0.05,0.2><-0.25,0.2>
            <-0.25,0.2><-0.45,0.2><-0.45,-0.05><-0.35,-0.15>
            <-0.35,-0.15><-0.25,-0.25><0,-0.6><0,-0.6>
            sturm
            pigment {color rgb <1,0,0>}
            rotate<-90,0,0>
            translate <0,1,0>
            rotate <35,0,1>
            scale 2
         }
      #break
      #case(17)
         quadric {
            <0.3, 1, 1>
            <1, 3, -1>
            <0, 3, -0.4>
            1
            pigment {
               gradient z
               sine_wave
               turbulence 0.1
               colour_map {
                  [0, rgb 0] 
                  [1, rgbf <1,1,1,0.7>]
               }
            }
            finish {
               irid {
                  0.5
                  thickness 0.4
                  turbulence 0.05
               }      
            }
         }
      #break
      #case(18)
         // create a 4th order infinite polynomial surface
         quartic {
            <
            // x^4,        x^3y,       x^3z,       x^3,        x^2y^2,
               3,          3,          3,          3,          3,
            // x^2yz,      x^2y,       x^2z^2,     x^2z,       x^2,
               0,          0,          0,          0,          pi,
            // xy^3,       xy^2z,      xy^2,       xyz^2,      xyz,
               0,          pi,          0,          0,          0,
            // xy,         xz^3,       xz^2,       xz,         x,
               -1,         -1,         -1,         -1,         -1,
            // y^4,        y^3z,       y^3,        y^2z^2,     y^2z,
               0,          0,          pi,          pi,          0,
            // y^2,        yz^3,       yz^2,       yz,         y,
               0,          0,          0,          0,          0,
            // z^4,        z^3,        z^2,        z,          C           
               0,          0,          1,          0,          0.33
            >
            sturm 
            pigment {
               gradient z
               sine_wave
               turbulence 0.1
               colour_map {
                  [0, rgb 0] 
                  [1, rgb <1,1,1>]
               }
            }
            finish {
               irid {
                  0.5
                  thickness 0.4
                  turbulence 0.05
               }      
            }
         }
      #break
      #case(19)
         smooth_triangle {
           < 0.1,0.1,0> <-1, 1, -1>
           < 2.5,0.1,0> < 1, 1, -1>   
           < 0.1,2.5,0> <-1, 1, -1>
           pigment {rgb 1}
         }
         smooth_triangle {
           <-0.1,0.1,0> <-1, 1, -1>
           <-2.5,0.1,0> <-1, 1, -1>   
           <-0.1,2.5,0> < 1, 1, -1>
           pigment {rgb 1}
         }
         smooth_triangle {
           < 0.1,-0.1,0> <-1, 1, -1>
           < 2.5,-0.1,0> <-1, 1, -1>   
           < 0.1,-2.5,0> < 1, 1, -1>
           pigment {rgb 1}
         }
         smooth_triangle {
           <-0.1,-0.1,0> <-1, 1, -1>
           <-2.5,-0.1,0> < 1, 1, -1>   
           <-0.1,-2.5,0> <-1, 1, -1>
           pigment {rgb 1}
         }
      #break
      #case(20) //sphere
      
         // This macro makes a "sphere" out of spheres.
         // Sphere_Of_Spheres(n,r,s)
         //    n: amount of spheres on the equator of the sphere.
         //    r: radius of the spheres.
         //    s: scalefactor for the radius of the spheres.
         //
         // The center of the sphere is at the origin, the spheres are centerd on the
         // raduis ru1. ru1 is determined by the number and size of the spheres.
         // The scalefactor gives the possibility to vary the size of the spheres, and 
         // keep the same radius ru1. Normaly s=1.
         //
         // In the scene, use r_ConnectSpheres inside an object, union or merge statement:
         // object{Sphere_Of_Cylinders(50,0.1,3) pigment{White}}

         #macro Sphere_Of_Spheres(n,r,s)
           #local Tz=0;   
           #local ru= (r)/sin(radians(180/n));
           #debug concat("\nru = ",str(ru,2,2),"\n")
           #local Rot2=0;  
           #local n1=n;
           #declare ru1=ru;
           #while (n >0)
             #local Tz=(sin(radians(Rot2))*ru1);
             #local Count=0;
             #local Rot1=0;
             #while (Count<=n+1)
               sphere{
                 0,r*s
                 translate <(cos(radians(Rot1)))*ru,Tz,(sin(radians(Rot1))*ru)>
               }
               #if (Tz>0)
                 sphere{
                   0,r*s
                   translate <(cos(radians(Rot1)))*ru,-Tz,(sin(radians(Rot1))*ru)>
                 }
               #end //if
               #local Rot1=Rot1+(360/n);
               #local Count=Count+1;
             #end //while
             #local Rot2=Rot2+(360/n1);
             #local ru= (cos(radians(Rot2)))*ru1;
             #local n=int(180/degrees(asin(r/ru)));
             //#local ru= r/sin(radians(180/n));
           #end //while
         #end //macro
         
         union {
            difference{
              sphere{0,3.16}
              sphere{0,3.06}
              Sphere_Of_Spheres(33,0.3,1)
              pigment{White}
              rotate<-70,30,0>
            } 
            union{
              Sphere_Of_Spheres(33,0.3,0.5)
              rotate<-70,30,0>
              pigment{Red}
            }
            scale 0.75
         }
      #break
      #case(21) //sphere_sweep
         #declare F1=function {pigment{cells}}
         #declare F2=function(x,y,z){sin(x)*sin(y)}
         #declare Yuck=pigment {
            function {
               F2(x*pi,y*pi,z)*F1(x,y,z).gray
            }
         }

         sphere_sweep {
            linear_spline       
            4,                  
            <-5, -5, 0>, .1     
            <-5, 5, 0>, 1       
            < 5, -5, 0>, 1
            < 5, 5, 0>, .1
            tolerance 0.001
            pigment {
               Yuck
               colour_map {
                  [0.1, rgb <0.6,0.5,0>]
                  [0.15, rgb <0.3,0.7,0>]
                  [0.25, rgb <0.5,0.7,0>]
                  [0.5, rgb <0.2,0.6,1>]
                  [0.8, rgb <.1,0,.1>]
               }
            }
            scale 0.3
            translate <1.1,1.2,1>
         } 
         sphere_sweep {
            cubic_spline
            6,                  
            <-5, -5, 0>, .1     
            <-5, -5, 0>, .1     
            <-5, 5, 0>, 1       
            < 5, -5, 0>, 1
            < 5, 5, 0>, .1
            < 5, 5, 0>, .1
            tolerance 0.001
            pigment {
               Yuck
               colour_map {
                  [0.1, rgb <1,1,0>]
                  [0.15, rgb <0.3,0.7,0>]
                  [0.25, rgb <0.5,0.7,0>]
                  [0.5, rgb <0.2,0.6,1>]
                  [0.8, rgb <1,0,.1>]
               }
            }
            scale 0.3
         }
         sphere_sweep {
            b_spline
            6,                  
            <-5, -5, 0>, .1     
            <-5, -5, 0>, .1     
            <-5, 5, 0>, 1       
            < 5, -5, 0>, 1
            < 5, 5, 0>, .1
            < 5, 5, 0>, .1
            tolerance 0.001
            pigment {
               Yuck
               colour_map {
                  [0.1, rgb <1,0.5,0.5>]
                  [0.15, rgb <0.3,0.7,0>]
                  [0.25, rgb <0.2,0.7,0.5>]
                  [0.5, rgb <0.2,0.6,1>]
                  [0.8, rgb <0,0,1>]
               }
            }
            scale 0.3
            translate <-0.8,-1.1,-1>
         }
      #break
      #case(22)
         #declare I=0;
         #declare N=15;
         #while (I<N)
            superellipsoid {<rand(Stream), rand(Stream)> rotate<rand(Stream)*180,rand(Stream)*180,rand(Stream)*180> scale 0.5 translate 3.5 rotate<0,0,I*360/N> pigment {rgb 1}}
            superellipsoid {<rand(Stream), rand(Stream)> rotate<rand(Stream)*180,rand(Stream)*180,rand(Stream)*180> scale 0.35 translate 2 rotate<0,0,I*360/N> pigment {rgb 1}}
            superellipsoid {<rand(Stream), rand(Stream)> rotate<rand(Stream)*180,rand(Stream)*180,rand(Stream)*180> scale 0.2 translate 1 rotate<0,0,I*360/N> pigment {rgb 1}}
            superellipsoid {<rand(Stream), rand(Stream)> rotate<rand(Stream)*180,rand(Stream)*180,rand(Stream)*180> scale 0.1 translate 0.5 rotate<0,0,I*360/N> pigment {rgb 1}}
            #declare I=I+1;
         #end
      #break
      #case(23)
         sor {
            7,
            <0.000000, 0.000000>
            <0.118143, 0.000000>
            <0.620253, 0.540084>
            <0.210970, 0.827004>
            <0.194093, 0.962025>
            <0.286920, 1.000000>
            <0.468354, 1.033755>
            translate <0,-0.5,0>
            rotate <-25,0,0>
            scale 3.5
            uv_mapping
            pigment {
         	   checker 
         	   color rgbf <1,1,1,0.5> 
               color rgbf <0,0,0,0> 
               scale <0.2,0.1,0.1>
            }
            finish {
               irid {
                  0.2
                  thickness 1
                  turbulence 0.2
               }      
            }
         }
      #break
      #case(24)
         #declare T= text {
            ttf           
            "crystal.ttf",
            "POV-Ray",    
            0.5,            
            0
            pigment {
               planar
               scale 2.5
               rotate <0,0,90>
               color_map {
                  [0, rgb <1,1,0.2>]
                  [1, rgb <1,0,0>]
                }
            }
            finish {phong 1 phong_size 5}             
            translate <1,0,1> 
            rotate <0,0,90>
         }
         #declare I=0;
         #declare N=14;
         #while (I<N)
            object {T rotate<0,0,-I*360/N>}
            #declare I=I+1;
         #end
      #break
      #case(25)
         #declare I=0;
         #declare N=10;
         union {
            #while (I<N)
               torus {2,0.1 rotate <90,0,0> rotate<0, I*360/N,0>}
               #declare I=I+1;
            #end
            torus {2,0.5 scale <1,0.3,1>}
            rotate <-15,0,0>
            texture {
               bozo
               scale 0.3
               texture_map {
                  [0, pigment {rgb <0,0.2,0.05>}]
                  [1, T_Copper_3C]
               }
            }
         }
      #break
      #case(26)
         triangle {
           < 0.1,0.1,0>
           < 2.5,0.1,0>   
           < 0.1,2.5,0>
           pigment {rgb 1}
         }
         triangle {
           <-0.1,0.1,0>
           <-2.5,0.1,0>   
           <-0.1,2.5,0>
           pigment {rgb 1}
         }
         triangle {
           < 0.1,-0.1,0>
           < 2.5,-0.1,0>  
           < 0.1,-2.5,0>
           pigment {rgb 1}
         }
         triangle {
           <-0.1,-0.1,0>
           <-2.5,-0.1,0>  
           <-0.1,-2.5,0>
           pigment {rgb 1}
         }
      #break
   #end
   camera {
     right x*image_width/image_height
     location  <0,0,-6>
     look_at   <0,0,0>
   }
   light_source {<500,500,-500> rgb 1}
   light_source {<500,-100,-500> rgb <0.4,0.3,0.3> shadowless}
   light_source {<-500,0,-500> rgb <0.3,0.3,0.4> shadowless}
   sky_sphere {
      pigment {
         planar
         color_map { [0.0 color blue 1] [1.0 color rgb <0.8,0.8,1.0>] }
      }
   }
#end

