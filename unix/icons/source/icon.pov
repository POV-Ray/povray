// For 16 icon:
// +w16 +h16 +a0.0 -j +r8
// For 32 icon:
// +w32 +h32 +a0.0 -j +r6
// For 48 icon:
// +w48 +h48 +a0.0 -j +r4

// For 128 icon:
// +w128 +h128 +a0.0 -j

#ifndef (Image)
	#declare Image = 2;
#end

// 1: Main icon
// 2: POV file icon
// 3: INC file icon
// 4: Render window icon
// 5: POV file icon (shadow)
// 6: INC file icon (shadow)

/*
NOTE:

When used as real icons (16, 32 and 48 pixels), the output from POV-Ray is
meant to be post-processed with the masks included.
Also, the 16x16 icon should be given a sharpening filter (before the mask is applied).
*/

#if (Image=1)
   #declare LogoColor = <0.0,0.5,1.0>;
   #declare Object = 1;
   #declare Disc = on;
   #declare Bkg = off;  
   #declare Shadow = off;
#end
#if (Image=2)
   #declare LogoColor = <0.0,0.5,1.0>;
   #declare Object = 1;
   #declare Disc = off;
   #declare Bkg = off; 
   #declare Shadow = off; 
#end
#if (Image=3)
   #declare LogoColor = <0.0,0.8,0.0>;
   #declare Object = 1;
   #declare Disc = off;
   #declare Bkg = off; 
   #declare Shadow = off;
#end
#if (Image=4)
   #declare LogoColor = <0.0,0.5,1.0>;
   #declare Object = 2;
   #declare Disc = on;
   #declare Bkg = off; 
   #declare Shadow = off;
#end
#if (Image=5)
   #declare LogoColor = <0.0,0.5,1.0>;
   #declare Object = 1;
   #declare Disc = off;
   #declare Bkg = off; 
   #declare Shadow = on; 
#end
#if (Image=6)
   #declare LogoColor = <0.0,0.8,0.0>;
   #declare Object = 1;
   #declare Disc = off;
   #declare Bkg = off; 
   #declare Shadow = on;
#end

camera {
   location -15*z
   right x
   up y
   angle 32 //+ 1*(image_width=16)
   look_at 0
}

background {color rgb 0}
#default {finish {ambient 0 diffuse 1 phong 1 phong_size 20}}

light_source {
	<-1,2,-3>*100, color 0.3
	area_light 150*x, 150*y, 6, 6 jitter
}

light_source {
	<-1,2,-3>*100, 
	#if (Shadow)
		color 0.55
	#else
		color 0.7
	#end
	shadowless
}

#if (Disc)
   union {
      torus {0.7, 0.3}
      cylinder {-0.3*y, 0.3*y, 0.7}
      rotate 90*x
      pigment {color rgb 1}
      scale 5 translate 3*z
   }
#else
  #if (Bkg)
    box {
      <0, 0, 0>, <1, 1, 0>
      texture {
        pigment {
          image_map {
            png "file16x16.png"           
            //png "file32x32.png"
            //png "file48x48.png"
            //png "file64x64.png"            
          }
        }
        finish {
          ambient 0.0
          diffuse 1.3
        }
      }
      scale 9
      translate -4.5*<1, 1, 0>
      translate 0.8*z
    }  
  #else
		#if (Shadow)
			plane {
				-z, -0.85
				pigment {
					color rgb 1.5
					/*
					gradient y translate -1.5*y
					scale 14 rotate 45*z
					color_map {[0.0, rgb 0.95][0.9, rgb 1.20]}
	*/
				}
			}
		#end
  #end
#end

#declare PovLogo =
union {
   sphere {2*y, 1}
   difference {
      cone {2*y, 1, -4*y, 0}
      sphere {2*y, 1.4 scale <1,1,2>}
   }
   difference {
      sphere {0, 1 scale <2.6, 2.2, 1>}
      sphere {0, 1 scale <2.3, 1.8, 2> translate <-.35, 0, 0>}
      rotate z*30 translate 2*y
   }
   rotate <0, 0, -25>
}

#declare RenderLogo =
union {
   #declare BarA =
   cone {-2.1*x, 0.1, 2.0*x, 0.7 translate <-0.9,1.5,0>}
   object {BarA} object {BarA rotate 180*z}
   
   #declare BarB =
   cone {-1.6*y, 0.1, 1.5*y, 0.7 translate <2.0,-0.9,0>}
   object {BarB} object {BarB rotate 180*z}
}

#if (Object=1)
   object {
      PovLogo
      #if (Disc)
         translate <-0.5,-0.35,0>
      #else
        #if ((Image=2) | (Image=3) | (Image=5) | (Image=6))
          scale 0.9
          translate <-0.9,-0.92,0>
        #else
          translate <-0.7,-0.5,0>
        #end
      #end
      pigment {color LogoColor}
		 #if (Shadow)
			 no_image
		 #end
   }
#else
   object {
      RenderLogo
      translate <-0.05,0.05,0>
      pigment {color LogoColor}
   }
#end

#if (Bkg)

  box {
    <-1, -1, 0>, <1, 1, 0>
    texture {
      pigment {
        image_map {
          png "file64x64.png"
        }
      }
      finish {
        ambient 1
        diffuse 0
      }
    }
  }
  
#end