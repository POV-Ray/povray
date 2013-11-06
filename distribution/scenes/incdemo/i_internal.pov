// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence of Vision Raytracer Scene Description File
// File: i_internal.pov
// Date: 11-04-2001
// Author: René Smellenbergh
// Demo: showing all internal functions of the "functions.inc" to be used in isosurfaces
// This scene uses the clock to switch between functions.
// Set "initial frame" to 1 and "final frame" to 78
// Using the "subset start" and "subset end" allows to choose the desired frames
//    example: all frames: start 1, end 78
//                   frame 14 (cushion): start 14, end 14
//
// -w320 -h240 +kfi1 +kff78
// -w800 -h600 +a0.3 +kfi1 +kff78

#version 3.7;

#if (clock_on = 0)
	#warning " This demo was designed to be used with the clock: \n please read the comments in the scene's header"#end
#include "functions.inc"
#declare AreaLightOn = off;	//Specify if you want soft shadows (slower!) or not
global_settings {
  assumed_gamma 1.8 
	noise_generator 2
}
camera{  
	location <0, 0, -40>
        right     x*image_width/image_height
	look_at <0, 0, 0>
	angle 20
}
light_source {
	<1000, 1000, -1200> rgb 1.3
	#if (AreaLightOn=yes)
		area_light <30,0,0>, <0,30,0>, 6, 6
		adaptive 1
		jitter
		orient circular
	#end
}
light_source { <0, 0, -40> rgb 0.5 shadowless }	//Fill in light
background { color red 0.184314 green 0.309804 blue 0.309804 }
#declare IsoTexture =
texture { pigment { rgb <1.0, 0.8, 0.5> } finish { phong 0.6 phong_size 250 } }
#declare ContTexture =
texture { pigment { rgb 0.65 } finish { ambient 0.35 phong 1.0  phong_size 40 } }
//***  FRAME 1  ***************************************************
#if ( frame_number = 1 )
	//** f_algbr_cyl1 : lathe form  ************************
	isosurface {
		function  { f_algbr_cyl1(x,y,z,1, 1.5, 1, 0, 0) }
			//P0= Field Strength
			//P1= Field limit
			//P2= SOR switch
			//P3= SOR offset
			//P4= SOR angle
		contained_by { sphere { 0,1.1} }
		max_gradient 1.5
		texture { IsoTexture }
		no_shadow
		scale 3  rotate x*-25 translate <3.5, -1, 0>
	}
	//** f_algbr_cyl1 : extruded form  ************************
	intersection {
		isosurface {
			function  { f_algbr_cyl1(x,y,z,1, 1.5, 0, 0, 0) }
				//P0= Field Strength
				//P1= Field limit
				//P2= SOR switch
				//P3= SOR offset
				//P4= SOR angle
			contained_by { box { <-1.05, -0.4, -0.01>, <1.05, 0.4, 3> } }
			max_gradient 1.2
			texture { IsoTexture }
			no_shadow
		}
		box { <-1.05, -0.4, -0.01>*0.9999, <1.05, 0.4, 3>*0.9999  texture { ContTexture } }
		scale 3  rotate x*-10 translate <-3, -1, 0> 
	}
#end
//***  FRAME 2  ***************************************************
#if ( frame_number = 2 )
	//** f_algbr_cyl2 : lathe form  ************************
	isosurface {
		function  { -(f_algbr_cyl2(x,y,z,0.3, 5, 1, 0, 0)) }
			//P0= Field Strength
			//P1= Field limit
			//P2= SOR switch
			//P3= SOR offset
			//P4= SOR angle
		contained_by { box { <-1.6, -0.01, -1.6>, <1.6, 2.4, 1.6> } }
		max_gradient 9
		texture { IsoTexture }
		no_shadow
		scale 1.9  rotate x*-15 translate <3.2, -4, 0>
	}
	//** f_algbr_cyl2 : extruded form  ************************
	intersection {
		isosurface {
			function  { f_algbr_cyl2(x,y,z,-0.3, 5, 0, 0, 0) }
				//P0= Field Strength
				//P1= Field limit
				//P2= SOR switch
				//P3= SOR offset
				//P4= SOR angle
			accuracy  0.0001
			contained_by { box { <-1.6, -0.01, -0.01>, <1.6, 2.4, 3> } }
			max_gradient 5.4
			texture { IsoTexture }
			no_shadow
		}
		box { <-1.6, -0.01, -0.01>*0.9999, <1.6, 2.4, 3>*0.9999  texture { ContTexture } }
		scale 1.9  rotate x*-15 translate <-3.2, -4, 0>
	}
#end
//***  FRAME 3  ***************************************************
#if ( frame_number = 3 )
	//** f_algbr_cyl3 : lathe form  ************************
	isosurface {
		function  { f_algbr_cyl3(x,y,z,-1, 3, 1, 0, 0) }
			//P0= Field Strength
			//P1= Field limit
			//P2= SOR switch
			//P3= SOR offset
			//P4= SOR angle
		contained_by { box { <-1.05, -0.01, -1.05>, <1.05, 1.3, 1.05> } }
		max_gradient 6.5
		texture { IsoTexture }
		no_shadow
		scale 3.2  rotate x*-20  translate <2.7, -3.5, 0>
	}
	//** f_algbr_cyl3 : extruded form  ************************
	#declare LCorner = <-1.05, -0.01, -0.01>;  #declare RCorner = <1.05, 1.3, 3> ;
	intersection {
		isosurface {
			function  { f_algbr_cyl3(x,y,z,-1, 3, 0, 0, 0)}
				//P0= Field Strength
				//P1= Field limit
				//P2= SOR switch
				//P3= SOR offset
				//P4= SOR angle
			contained_by { box { LCorner, RCorner  } }
			max_gradient 2
			texture { IsoTexture }
			no_shadow
		}
		box { LCorner*0.9999, RCorner*0.9999  texture { ContTexture } }
		scale 3.2  rotate y*-5  rotate x*-10  translate <-4.8, -3.5, 0>
	}
	light_source { <-10, 4, -0> rgb 0.5 shadowless }
#end
//***  FRAME 4  ***************************************************
#if ( frame_number = 4 )
	//** f_algbr_cyl4 : lathe form  ************************
	isosurface {
		function  { f_algbr_cyl4(x,y,z, -0.4, 5, 1, 0, 0) }
			//P0= Field Strength
			//P1= Field limit
			//P2= SOR switch
			//P3= SOR offset
			//P4= SOR angle
		contained_by { sphere { 0, 1.2 } }
		max_gradient 7.1
		texture { IsoTexture }
		no_shadow
		scale 3.3  rotate x*-10  translate <3.5, -0.8, 0>
	}
	//** f_algbr_cyl4 : extruded form  ************************
	#declare LCorner = <-0.9, -0.55, -0.01>;  #declare RCorner = <0.9, 1.0, 6>;
	intersection {
		isosurface {
			function  { f_algbr_cyl4(x,y,z, -0.4, 5, 0, 0, 0) }
				//P0= Field Strength
				//P1= Field limit
				//P2= SOR switch
				//P3= SOR offset
				//P4= SOR angle
			contained_by { box { LCorner, RCorner } }
			max_gradient 4
			texture { IsoTexture }
			no_shadow
		}
		box { LCorner*0.9999, RCorner*0.9999  texture { ContTexture } }
		scale 3.3  translate <-3.3, -0.8, 0>
	}
#end
//***  FRAME 5  ***************************************************
#if ( frame_number = 5 )
isosurface {
	function  { f_bicorn(x,y,z, -0.01, 3) }
		//P0= Field strength
		//P1= Y Scale (inverse). The surface is always the same shape. Setting the scale to 1 gives
		//			a surface with a radius of about 1 unit
	contained_by { sphere { 0, 3 } }
	max_gradient 5.0
	texture { IsoTexture }
	no_shadow
	scale 1.5  rotate x*-20
}
#end
//***  FRAME 6  ***************************************************
#if ( frame_number = 6 )
isosurface {
	function  { f_bifolia(x,y,z, -1, 3) }
		//P0= Field Strength
		//P1= Scale. Setting the scale to 3 gives a surface with a radius of about 1 unit
	contained_by { box { <-1.0, -0.01, -1.0>, <1.0, 0.8, 1.0> } }
	max_gradient 7.3
	texture { IsoTexture }
	no_shadow
	scale 4.5  rotate x*-20
}
#end
//***  FRAME 7  ***************************************************
#if ( frame_number = 7 )
	#declare LCorner = <-1.9, -1.3, -1.3>;  #declare RCorner = <1.8, 1.3, 1.3>;
	isosurface {
		function  {  f_blob(x,y,z, 1.4, 1, 0.8, 1.2, 1) }
			//P0= X distance between the 2 components
			//P1= Strength of component 1
			//P2= Radius component 1 (inv)
			//P3= Strength of component 2
			//P4= Radius component 2 (inv)
		threshold  -0.3
		contained_by { box { LCorner, RCorner } }
		max_gradient 1.8
		texture { IsoTexture }
		no_shadow
		scale 4
	}
#end
	
	
//***  FRAME 8  ***************************************************
#if ( frame_number = 8 )
	#declare LCorner = <-0.5, -0.5, -0.5>;  #declare RCorner = <1.4, 0.5, 0.5>;
	isosurface {
		function  { f_blob2(x,y,z, 1, 1/0.2, 2, 1) }
			//P0= Separation (One comp at origin)
			//P1= Size (inv)
			//P2= Strength
			//P3= Threshold
		contained_by { box { LCorner, RCorner } }
		max_gradient 3.9
		texture { IsoTexture }
		no_shadow
		scale 7  translate x*-3.5
	}
#end	//frame_number = 51
//***  FRAME 9  ***************************************************
#if ( frame_number = 9 )
isosurface {
	function  { f_boy_surface(x,y,z, -0.001, 0.01) }
		//P0= Field Strength. Set extremely low to avoid that the shape breaks up.
		//P1= Scale. The surface is always the same shape.
	contained_by { box { <-1.0, -1.45, -0.1>, <1.5, 1.2, 2.2> } }
	max_gradient 2.0
	texture { IsoTexture }
	no_shadow
	scale 3  rotate y*15  rotate x*-20
}
#end
//***  FRAME 10  ***************************************************
#if ( frame_number = 10 )
	#declare LCorner = <-1.0, -0.5, -0.55>;  #declare RCorner = <1.0, 0.6, 1.1>;
	isosurface {
		function  { f_comma(x,y,z, 1) }
			//P0= size
		contained_by { box { LCorner, RCorner } }
		max_gradient 1.4
		texture { IsoTexture }
		scale 5  rotate y*-30  rotate x*-35  translate y*-1.8
	}
#end
//***  FRAME 11  ***************************************************
#if ( frame_number = 11 )
	#declare Radius = 2.4;
	isosurface {
		function  { f_cross_ellipsoids(x,y,z, 0.05, 8, 8, 1) }
			//P0= Eccentricity
			//P1= Size (inv)
			//P2= Diameter
			//P3= Threshold
		contained_by { sphere { 0, Radius } }
		max_gradient 12
		texture { IsoTexture }
		no_shadow
		scale 2  rotate y*25  rotate x*-20
	}
#end
//***  FRAME 12  ***************************************************
#if ( frame_number = 12 )
	#declare LCorner = <-3.2, -0.1, -2.5>;  #declare RCorner = <3.2, 2, 2.5>;
	intersection {
		isosurface {
			function  { f_crossed_trough(x,y,z, -0.05) }
				//P0= Field Strength
			contained_by { box { LCorner, RCorner } }
			max_gradient 0.5
			texture { IsoTexture }
			no_shadow
		}
		box { LCorner*0.9999, RCorner*0.9999  texture { ContTexture } }
		scale 1.8  rotate x*-90
	}
#end
//***  FRAME 13  ***************************************************
#if ( frame_number = 13 )
	#declare Radius = 8;
	intersection {
		isosurface {
			function  { f_cubic_saddle(x,y,z, -0.5) }
				//P0= Field Strength
			contained_by { sphere { 0, Radius } }
			max_gradient 38.1
			texture { IsoTexture }
			no_shadow
		}
		sphere { 0, Radius-0.00001  texture { ContTexture } }
		scale 0.8  rotate z*135  rotate x*-20  translate y*1.2
	}
#end
//***  FRAME 14  ***************************************************
#if ( frame_number = 14 )
isosurface {
	function  { f_cushion(x,y,z, -0.25) }
		//P0= Field Strength
	contained_by { box { <-1.1, -1.3, -0.45>, <1.1, 1.3, 1.2> } }
	max_gradient 2.9
	texture { IsoTexture }
	no_shadow
	scale 3  rotate y*-145
}
#end
//***  FRAME 15  ***************************************************
#if ( frame_number = 15 )
isosurface {
	function  { f_devils_curve(x,y,z, -0.2) }
		//P0= Field Strength
	contained_by { box { <-1.2, -1.2, -1.2>, <1.2, 1.2, 1.2> } }
	max_gradient 2.5
	texture { IsoTexture }
	no_shadow
	scale 4  rotate x*5
}
#end
//***  FRAME 16  ***************************************************
#if ( frame_number = 16 )
	//**  f_devils_curve_2d  : lathe form  ***********************************
	isosurface {
		function  { f_devils_curve_2d(x,y,z, -1, 0.25, 0.3, 1, 0, 0) }
			//P0= Field Strength
			//P1= X factor. The X and Y factors control the size of the central feature
			//P2= Y factor
			//			If the X factor is slightly stronger than the Y factor, then the side pieces
			//			are linked to the central piece by a horizontal bridge at each corner.
			//			If the Y factor is slightly greater than the X factor, then there is a vertical
			//			gap between the side pieces and the central piece at each corner.
			//			If the X and Y factors are equal each of the four corners meets at a point.
			//P3= SOR switch
			//P4= SOR offset
			//P5= SOR angle
		contained_by { box { <-0.7, -0.8, -0.8>, <0.7, 0.7, 0.7> } }
		max_gradient 3.3
		texture { IsoTexture }
		no_shadow
		scale 4.2  rotate x*-2 translate <3.3, -0.5, 0>
	}
	//**  f_devils_curve_2d  : extruded form  **********************************
	#declare LCorner = <-0.7, -0.8, -0.01>;  #declare RCorner = <0.7, 0.7, 6>;
	intersection {
		isosurface {
			function  { f_devils_curve_2d(x,y,z, -1, 0.25, 0.3, 0, 0, 0) }
				//P0= Field Strength
				//P1= X factor. The X and Y factors control the size of the central feature
				//P2= Y factor
				//			If the X factor is slightly stronger than the Y factor, then the side pieces
				//			are linked to the central piece by a horizontal bridge at each corner.
				//			If the Y factor is slightly greater than the X factor, then there is a vertical
				//			gap between the side pieces and the central piece at each corner.
				//			If the X and Y factors are equal each of the four corners meets at a point.
				//P3= SOR switch
				//P4= SOR offset
				//P5= SOR angle
			contained_by { box { LCorner, RCorner } }
			max_gradient 1.5
			texture { IsoTexture }
			no_shadow
		}
		box { LCorner*0.9999, RCorner*0.9999  texture { ContTexture } }
		scale 4.2  rotate x*-2 translate <-3.3, -0.5, 0>
		no_shadow
	}
#end
//***  FRAME 17  ***************************************************
#if ( frame_number = 17 )
isosurface {
	function  { f_dupin_cyclid(x,y,z,-.05, 0.27, 0.04, 0.55, 0.0, 1.0) }
		//P0= Field Strength
		//P1= Major radius of torus
		//P2= Minor radius of torus
		//P3= X displacement of torus
		//P4= Y displacement of torus
		//P5= radius of inversion
	contained_by { box { <1.0, -0.6, -1.6>, <4.3, 0.6, 1.6> } }
	max_gradient 2.1
	texture { IsoTexture }
	no_shadow
	scale 1.5  rotate x*-90
}
#end
//***  FRAME 18  ***************************************************
#if ( frame_number = 18 )
	#declare Radius = 1.0;
	isosurface {
		function  { f_ellipsoid(x,y,z, 1/0.95, 1/0.3, 1/0.95) }
			//P0= X scale (inv)
			//P1= Y scale (inv)
			//P2= Z scale (inv)
		threshold  1
		contained_by { sphere { 0, Radius } }
		max_gradient 2.7
		texture { IsoTexture }
		no_shadow
		scale 5.5  rotate x*-25  translate y*-1
	}
#end
//***  FRAME 19  ***************************************************
#if ( frame_number = 19 )
isosurface {
	function  { f_enneper(x,y,z, -0.1) }
		//P0= Field Strength
	contained_by { box { <-3.5, -3.5, -3.8>, <3.5, 3.5, -0.25> } }
	max_gradient 85
	texture { IsoTexture }
	no_shadow
	scale 1.1  rotate y*15
}
#end
//***  FRAME 20  ***************************************************
#if ( frame_number = 20 )
	#declare Radius = 1.6;
	isosurface {
		function  { f_flange_cover(x,y,z, 0.01, 35, 1.5, 1.2) }
			//P0= Spikiness (1= sph; <= spikes)
			//P1= Size (inv)
			//P2= Flange (1= no; >=flanges)
			//P3= Threshold
		contained_by { sphere { 0, Radius } }
		max_gradient 11
		texture { IsoTexture }
		no_shadow
		scale 3.45  rotate y*15  rotate x*-30  translate y*-0.1
	}
#end
//***  FRAME 21  ***************************************************
#if ( frame_number = 21)
	#declare LCorner = <-0.0, -0.9, -0.9>;  #declare RCorner = <0.8, 0.9, 0.9>;
	intersection {
		isosurface {
			function  { f_folium_surface(x,y,z, -0.02, 15, 15) }
				//P0= Field Strength
				//P1= Neck width factor - the larger you set this, the narrower the neck where the paraboloid meets the plane
				//P2= Divergence - the higher the value, the wider the paraboloid gets
			contained_by { box { LCorner, RCorner  } }
			max_gradient 1.3
			texture { IsoTexture }
			no_shadow
		}
		box { LCorner*0.9999, RCorner*0.9999  texture { ContTexture } }
		scale 5.5  rotate z*90  rotate x*-25  translate y*-1.5
		no_shadow
	}
#end
//***  FRAME 22  ***************************************************
#if ( frame_number = 22)
	//**  f_folium_surface_2d  : lathe form  **************************************
	#declare LCorner = <-0.7, -0.8, -0.7>;  #declare RCorner = <0.7, 0.8, 0.7>;
	intersection {
		isosurface {
			function  { f_folium_surface_2d(x,y,z, -0.01, 12, 20, 1, 0, 0) }
				//P0= Field Strength
				//P1= Neck width factor - same as 3d surface if you're revolving it around the Y axis
				//P2= Divergence - same as 3d surface if you're revolving it around the Y axis
				//P3= SOR switch
				//P4= SOR offset
				//P5= SOR angle
			contained_by { box { LCorner, RCorner } }
			max_gradient 0.5
			texture { IsoTexture }
			no_shadow
		}
		box { LCorner*0.9999, RCorner*0.9999  texture { ContTexture } }
		scale 5  rotate x*3  translate <2.5, -0.5, 0>
		no_shadow
	}
	//**  f_folium_surface_2d  : extruded form  **************************************
	#declare LCorner = <-0.7, -0.8, -0.01>;  #declare RCorner = <0.7, 0.8, 3>;
	intersection {
		isosurface {
			function  { f_folium_surface_2d(x,y,z, -0.01, 12, 20, 0, 0, 0) }
				//P0= Field Strength
				//P1= Neck width factor - same as 3d surface if you're revolving it around the Y axis
				//P2= Divergence - same as 3d surface if you're revolving it around the Y axis
				//P3= SOR switch
				//P4= SOR offset
				//P5= SOR angle
			contained_by { box { LCorner, RCorner } }
			max_gradient 0.3
			texture { IsoTexture }
		}
		box { LCorner*0.9999, RCorner*0.9999  texture { ContTexture } }
		scale 5  rotate x*3  translate <-5.5, -0.5, 0>
		no_shadow
	}
#end
//***  FRAME 23  ***************************************************
#if ( frame_number = 23 )
isosurface {
	function  { f_glob(x,y,z, -1) }
		//P0= Field Strength
	contained_by { box { <-1.0, -1.5, -1.6>, <1.5, 1.5, 1.6> } }
	max_gradient 6.5
	texture { IsoTexture }
	no_shadow
	scale 3.7  rotate z*90  rotate x*10
}
#end
//***  FRAME 24  ***************************************************
#if ( frame_number = 24 )
	isosurface {
		function  { f_heart(x,y,z, -0.001) }
			//P0= Field Strength
		contained_by { box { <-0.8, -1.2, -1.1>, <0.8, 1.2, 1.3> } }
		max_gradient 0.1
		texture { IsoTexture }
		no_shadow
		scale 3  rotate y*-90  rotate z*-90
	}
#end
//***  FRAME 25  ***************************************************
#if ( frame_number = 25 )
	#declare LCorner = <-3.7, -1.7, -3.7>;  #declare RCorner = <3.7, 1.7, 3.7>;
	isosurface {
		function  { f_helical_torus(x,y,z, 2, 7, 1, 0.1, 1, 0.5, 1, 6, 2.0, 0) }
			//P0= Major radius
			//P1= Nr of winding loops
			//P2= Twistness winding
			//P3= Flatness of winding?
			//P4= Threshold
			//P5= Negative minor radius?
			//P6= Another fatness of winding control
			//P7= Groove period
			//P8= Groove amplitude
			//P9= Groove phase
		contained_by { box { LCorner, RCorner } }
		max_gradient 13
		texture { IsoTexture }
		no_shadow
		scale 1.7  rotate x*-45
	}
#end
//***  FRAME 26  ***************************************************
#if ( frame_number = 26 )
	#declare LCorner = <-1.5, -2.9, -1.5>;  #declare RCorner = <1.5, 2.9, 1.5>;
	isosurface {
		function { f_helix1(x,y,z, 1, 8, 0.3, 0.9, 0.6, 0.2, 0) }
			//P0= number of helixes
			//P1= frequency
			//P2= minor radius
			//P3= major radius
			//P4= Y scale cross section
			//P5= cross section
			//P6= cross section rotation (°)
		contained_by { box { LCorner, RCorner } }
		max_gradient 1.5
		texture { IsoTexture }
		no_shadow
		 scale 1.9  rotate z*-54
	}
#end
//***  FRAME 27  ***************************************************
#if ( frame_number = 27 )
	#declare LCorner = <-1, -2.9, -1>;  #declare RCorner = <1, 2.9, 1>;
	isosurface {
		function { f_helix2(x,y,z, 0, 8, 0.35, 0.4, 0, 0.5, 0) }
			//P0= not used
			//P1= frequency
			//P2= minor radius
			//P3= major radius
			//P4= not used
			//P5= cross section
			//P6= cross section rotation (°)
		contained_by { box { LCorner, RCorner } }
		max_gradient 4.3
		texture { IsoTexture }
		no_shadow
		scale 2.2  rotate z*-54
	}
#end
//***  FRAME 28  ***************************************************
#if ( frame_number = 28 )
	#declare LCorner = <-4, -4, -4>;  #declare RCorner = <4, 4, 4>;
	isosurface {
		function { f_hex_x(x,y,z, 1) } 
			//P0= not used
		threshold 0.6
		contained_by { box { LCorner, RCorner } }
		max_gradient 1
		texture { IsoTexture }
		no_shadow
	}
#end
//***  FRAME 29  ***************************************************
#if ( frame_number = 29 )
	#declare LCorner = <-4, -4, -4>;  #declare RCorner = <4, 4, 4>;
	isosurface {
		function { f_hex_y(x,y,z, 1) } 
			//P0= not used
		threshold 0.1
		contained_by { box { LCorner, RCorner } }
		max_gradient 1
		texture { IsoTexture }
		no_shadow
	}
#end
//***  FRAME 30  ***************************************************
#if ( frame_number = 30 )
	#declare Radius = 1.5;
	intersection {
		isosurface {
			function { f_hetero_mf(x,y,z, 1.1, 2.05, 15, 0.26, 0.01, 1) }
				//P0= H
				//P1= lacunarity
				//P2= octaves
				//P3= offset
				//P4= T
				//P5= noise generator type
			contained_by { sphere { 0, Radius } }
			max_gradient 5.5
			texture { IsoTexture }
			no_shadow
		}
		sphere { 0, Radius-0.00001  texture { ContTexture } }
		scale 4.3  translate y*-1.7
	}
#end
//***  FRAME 31  ***************************************************
#if ( frame_number = 31 )
isosurface {
	function  { f_hunt_surface(x,y,z, -0.01) }
		//P0= Field Strength
	contained_by { sphere { 0, 3.8 } }
	max_gradient 84
	texture { IsoTexture }
	no_shadow
	scale 1.2  rotate y*-45
}
#end
//***  FRAME 32  ***************************************************
#if ( frame_number = 32 )
isosurface {
	function  { f_hyperbolic_torus(x,y,z, -0.005, 1, 0.6) }
		//P0= Field Strength
		//P1= Major radius: separation between the centres of the tubes at the closest point
		//P2= Minor radius: thickness of the tubes at the closest point
	contained_by { box { <-4, -0.9, -4>, <4, 0.9, 4> } }
	max_gradient 0.9
	texture { IsoTexture }
	no_shadow
	scale 1.5  rotate x*-90
}
#end
//***  FRAME 33  ***************************************************
#if ( frame_number = 33 )
	#declare Radius = 0.9;
	isosurface {
		function  { f_isect_ellipsoids(x,y,z, 5, 1, 18, 1) }
			//P0= Eccentricity
			//P1= Size (inv)
			//P2= Diameter
			//P3= Threshold
		contained_by { sphere { 0, Radius } }
		max_gradient 20
		texture { IsoTexture }
		scale 5  rotate y*25  rotate x*-20
		no_shadow
	}
#end
//***  FRAME 34  ***************************************************
#declare ContainerOn = off;
#declare SphereOn = 0;
#if ( frame_number = 34 )
	#declare LCorner = <-1.0, -1.5, -1.5>;  #declare RCorner = <1.0, 1.5, 1.5>;
	intersection {
		isosurface {
			function  { f_kampyle_of_eudoxus(x,y,z, -0.001, 0.01, 2) }
				//P0= Field Strength
				//P1= Dimple: When zero, the two dimples punch right through and meet at the centre. Non-zero values give less dimpling
				//P2= Closeness: Higher values make the two planes become closer
			contained_by { box { LCorner, RCorner } }
			max_gradient 0.04
			texture { IsoTexture }
			no_shadow
		}
		box { LCorner*0.9999, RCorner*0.9999  texture { ContTexture } }
		scale 3.8  rotate z*90
		no_shadow
	}
#end
//***  FRAME 35  ***************************************************
#if ( frame_number = 35 )
	//**  f_kampyle_of_eudoxus_2d  : lathe form  *****************************
	#declare LCorner = <-1.6, -1.1, -1.6>;  #declare RCorner = <1.6, 1.15, 1.6>;
	intersection {
		isosurface {
			function  { f_kampyle_of_eudoxus_2d(x,y,z, -0.001, 0, 2, 1, 0, 90) }
				//P0= Field Strength
				//P1= Dimple: When zero, the two dimples punch right through and meet at the centre. Non-zero values give less dimpling
				//P2= Closeness: Higher values make the two planes become closer
				//P3= SOR switch
				//P4= SOR offset
				//P5= SOR angle
			contained_by { box { LCorner, RCorner } }
			max_gradient 0.2
			texture { IsoTexture }
			
		}
		box { LCorner*0.9999, RCorner*0.9999  texture { ContTexture }  }
		scale 1.9  rotate x*-1  translate <3, -1, 0>
		no_shadow
	}
	//**  f_kampyle_of_eudoxus_2d  : extruded form  *****************************
	#declare LCorner = <-1.1, -1.6, -0.01>;  #declare RCorner = <1.15, 1.6, 8>;
	intersection {
		isosurface {
			function  { f_kampyle_of_eudoxus_2d(x,y,z, -0.001, 0, 2, 0, 0, 0) }
				//P0= Field Strength
				//P1= Dimple: When zero, the two dimples punch right through and meet at the centre. Non-zero values give less dimpling
				//P2= Closeness: Higher values make the two planes become closer
				//P3= SOR switch
				//P4= SOR offset
				//P5= SOR angle
			contained_by { box { LCorner, RCorner } }
			max_gradient 0.05
			texture { IsoTexture }
			no_shadow
		}
		box { LCorner*0.9999, RCorner*0.9999  texture { ContTexture } }
		scale 1.9  rotate z*90 rotate x*-1  translate <-3.6, -1, 0>
		no_shadow
	}
	light_source { <0, -1, -0.5> rgb 0.3 shadowless }
#end
//***  FRAME 36  ***************************************************
#if ( frame_number = 36 )
	isosurface {
		function  { f_klein_bottle(x,y,z, -0.0005) }
			//P0= Field strength
		contained_by { sphere { 0, 4.2 } }
		max_gradient 2.7
		texture { IsoTexture }
		no_shadow
		scale 1.3  rotate y*110  rotate x*5
	}
#end
//***  FRAME 37  ***************************************************
#if ( frame_number = 37 )
	#declare LCorner = <-7.4, -7.4, -7.4>;  #declare RCorner = <7.4, 7.4, 7.4>;
	intersection {
		isosurface {
			function  { f_kummer_surface_v1(x,y,z, -0.01) }
				//P0= Field Strength
			contained_by { box { LCorner, RCorner } }
			max_gradient 8.3
			texture { IsoTexture }
			no_shadow
		}
		box { LCorner*0.9999, RCorner*0.9999  texture { ContTexture } }
		scale 0.55  rotate y*-25
	}
#end
//***  FRAME 38  ***************************************************
#if ( frame_number = 38 )
	isosurface {
		function  { f_kummer_surface_v2(x,y,z, -0.0005, -0.3, -0.97, 0) }
			//P0= Field Strength
			//P1= Rod width (negative): Setting this parameter to larger negative values increases the diameter of the rods
			//P2= Divergence (negative): Setting this number to -1 causes the rods to become approximately cylindrical. 
			//			Larger negative values cause the rods to become fatter further from the origin
			//			Smaller negative values cause the rods to become narrower away from the origin, and have a finite length
			//P3= Influences the length of half of the rods. Changing the sign affects the other half of the rods. 0 has no effect 
		contained_by { box { -3.8, 3.8 } }
		max_gradient 0.2
		texture { IsoTexture }
		no_shadow
		scale 1.1  rotate y*25  rotate x*-5
	}
#end
//***  FRAME 39  ***************************************************
#if ( frame_number = 39 )
	isosurface {
		function  { f_lemniscate_of_gerono(x,y,z, -0.1) }
			//P0= Field Strength
		contained_by { box { <-1.05, -0.55, -0.55>, <1.05, 0.55, 0.55> } }
		max_gradient 0.3
		texture { IsoTexture }
		no_shadow
		scale 6  rotate y*15
	}
#end
//***  FRAME 40  ***************************************************
#if ( frame_number = 40 )
	//** f_ lemniscate_of_gerono_2d  : lathe form  **********************************
	#declare LCorner = <-3, -1, -3>;  #declare RCorner = <3, 1, 3>;
	isosurface {
		function  { f_lemniscate_of_gerono_2d(x,y,z, -0.1, 1, 1, 1, 2, -45) }
			//P0= Field Strength
			//P1= Size: increasing this makes the 2d curve larger and less rounded
			//P2= Width: increasing this makes the 2d curve flatter
			//P3= SOR switch
			//P4= SOR offset
			//P5= SOR angle
		contained_by { box { LCorner, RCorner } }
		max_gradient 2.8
		texture { IsoTexture }
		scale 1.4  rotate x*-15  translate <2.0, -2.4, 0>
		no_shadow
	}
	//**  f_lemniscate_of_gerono_2d  : extruded form  ************************************
	#declare LCorner = <-3, -1, -0.01>;  #declare RCorner = <3, 1, 8>;
	intersection {
		isosurface {
			function  { f_lemniscate_of_gerono_2d(x,y,z, -0.1, 1, 1, 0, 2, -45) }
				//P0= Field Strength
				//P1= Size: increasing this makes the 2d curve larger and less rounded
				//P2= Width: increasing this makes the 2d curve flatter
				//P3= SOR switch
				//P4= SOR offset
				//P5= SOR angle
			contained_by { box { LCorner, RCorner } }
			max_gradient 5
			texture { IsoTexture }
			no_shadow
		}
		box { LCorner*0.9999, RCorner*0.9999  texture { ContTexture } }
		scale 1.4  rotate z*45  rotate x*-3  translate <-4.8, -2.4, 0>
		no_shadow
	}
#end
//***  FRAME 41  ***************************************************
#if ( frame_number = 41 )
	#declare LCorner = <-1, -0.05, -1>;  #declare RCorner = <1, 0.05, 1>;
	intersection {
		isosurface {
			function { f_mesh1(x,y,z, 1/8, 1/8, 1/10, 0.01, 1/10) }
				//P0= X frequency
				//P1= Z frequency
				//P2= scale (X-Z)
				//P3= amplitude
				//P4= scale Y
			threshold  0.001
			contained_by { box { LCorner, RCorner } }
			max_gradient 0.2
			texture { IsoTexture }
			no_shadow
		}
		box { LCorner*0.9999, RCorner*0.9999  texture { ContTexture } }
		scale 6  rotate x*-30
	}
#end
//***  FRAME 42  ***************************************************
#if ( frame_number = 42 )
isosurface {
	function  { f_mitre(x,y,z, -0.5) }	
		//P0= Field Strength
	contained_by {box { <-0.4, -1, -1>, <0.4, 1, 1> }}
	max_gradient 3.1
	texture { IsoTexture }
	no_shadow
	scale 4.5  rotate y*-45
}
#end
//***  FRAME 43  ***************************************************
#if ( frame_number = 43 )
	#declare LCorner = <-10, -4, -4>;  #declare RCorner = <6, 4, 4>;
	intersection {
		isosurface {
			function  { f_nodal_cubic(x,y,z, -0.005) }
				//P0= Field Strength
			contained_by { box { LCorner, RCorner } }
			max_trace 3
			max_gradient 0.4
			texture { IsoTexture }
			no_shadow
		}
		box { LCorner*0.9999, RCorner*0.9999  texture { ContTexture } }
		scale 0.9  rotate <0, 110, 0>
		no_shadow
	}
#end
//***  FRAME 44  ***************************************************
#if ( frame_number = 44 )
isosurface {
	function  { f_odd(x,y,z, -0.1) }
		//P0= Field Strength
	contained_by { box { <-1.05, -1.3, -0.45>, <1.05, 1.3, 1.1> } }
	max_gradient 1.0
	texture { IsoTexture }
	no_shadow
	scale 3.5  rotate y*-45  translate x*0.8
}
#end
//***  FRAME 45  ***************************************************
#if ( frame_number = 45 )
isosurface {
	function  { f_ovals_of_cassini(x,y,z, -0.1, 0.4, 0.18, 6) }	
		//P0= Field strength
		//P1= Major radius - like the major radius of a torus
		//P2= Filling. Zero for a torus. With higher values the hole in the middle starts to fill up. Even higher values give an ellipsoid with a dimple
		//P3= Thickness. Higher values give plumper results
	contained_by { box { <-0.82, -0.35, -0.82>, <0.82, 0.35, 0.82> }}
	max_gradient 0.4
	texture { IsoTexture }
	no_shadow
	scale 6  rotate x*-45
}
#end
//***  FRAME 46  ***************************************************
#if ( frame_number = 46 )
isosurface {
	function  { f_paraboloid(x,y,z, -1) }
		//P0= Field Strength
	contained_by {box { <-1.4, -0.1, -1.4>, <1.4, 1.9, 1.4> }}
	max_gradient 3.2
	texture { IsoTexture }
	no_shadow
	scale 3.5  rotate x*20  translate y*-3.5
}
#end
//***  FRAME 47  ***************************************************
#if ( frame_number = 47 )
isosurface {
	function  { f_parabolic_torus(x,y,z, -0.1, 0.4, 0.5) }
		//P0= Field Strength
		//P1= Major radius
		//P2= Minor radius
	contained_by {box { <-1.4, -0.4, -0.4>, <1.4, 0.4, 1.2> } }
	max_gradient 0.7
	texture { IsoTexture }
	no_shadow
	scale 4.5  rotate x*-92  rotate y*-8  translate y*-1.6
}
#end
//***  FRAME 48  ***************************************************
#if ( frame_number = 48 )
	#declare Radius = 4;
	intersection {
		isosurface {
			function  { f_ph(x,y,z) }
			threshold 0.5
			contained_by { sphere { 0, Radius } }
			max_gradient 2
			texture { IsoTexture }
		}
		sphere { 0, Radius - 0.0001  texture { ContTexture } }
	}
#end
//***  FRAME 49  ***************************************************
#if ( frame_number = 49 )
isosurface {
	function  { f_pillow(x,y,z, 1) }
		//P0= Field Strength
	contained_by {box { <-1.2, -1.2, -1.2>, <1.2, 1.2, 1.2> }}
	max_gradient 5.9
	texture { IsoTexture }
	no_shadow
	scale 2.7  rotate y*45  rotate x*-20
}
#end
//***  FRAME 50  ***************************************************
#if ( frame_number = 50 )
isosurface {
	function  { f_piriform(x,y,z, 0.7) }
		//P0= Field Strength
	contained_by { box { <-0, -0.4, -0.4>, <1, 0.4, 0.4> } }
	max_gradient 0.9
	texture { IsoTexture }
	no_shadow
	scale 10  rotate y*-20  translate x*-4
}
#end
//***  FRAME 51  ***************************************************
#if ( frame_number = 51 )
	//**  f_piriform_2d  : lathe form  ****************************************
	#declare LCorner = <-0.4, -0.1, -0.4>;  #declare RCorner = <0.4, 1.1, 0.4>;
	isosurface {
		function  { f_piriform_2d(x,y,z, -1, 1, -1, 0.9, 1, 0, -90) }
			//P0= Field Strength
			//P1= Size factor1: increasing this makes the curve larger
			//P2= Size factor 2: making this less negative makes the curve larger but also thinner
			//P3= Flatness: increasing this makes the curve fatter
			//P4= SOR switch
			//P5= SOR offset
			//P6= SOR angle
		contained_by { box { LCorner, RCorner } }
		max_gradient 1.4
		texture { IsoTexture }
		scale 7.5  translate <3, -4.6, 0>
		no_shadow
	}
	//**  f_piriform_2d  : extruded form   ************************************
	#declare LCorner = <-0.4, -0.4, -0.01>;  #declare RCorner = <1.1, 0.4, 7>;
	intersection {
		isosurface {
			function  { f_piriform_2d(x,y,z, -1, 1, -1, 0.9, 0, 0, 0) }
				//P0= Field Strength
				//P1= Size factor1: increasing this makes the curve larger
				//P2= Size factor 2: making this less negative makes the curve larger but also thinner
				//P3= Flatness: increasing this makes the curve fatter
				//P4= SOR switch
				//P5= SOR offset
				//P6= SOR angle
			contained_by { box { LCorner, RCorner } }
			max_gradient 0.8
			texture { IsoTexture }
		}
		box { LCorner*0.9999, RCorner*0.9999  texture { ContTexture } }
		scale 7.5  rotate z*90 rotate y*1  translate <-3.3, -4.6, 0>
	}
#end
//***  FRAME 52  ***************************************************
#if ( frame_number = 52 )
	#declare LCorner = <-0.3, 0, -0.3>;  #declare RCorner = <0.3, 1, 0.3>;
	isosurface {
		function  { f_poly4(x,y,z, 0, 1, -1, 0, 0) }
			//P0= Constant
			//P1= Y coefficient
			//p2= Y2 coefficient
			//p3= Y3 coefficient
			//p4= Y4 coefficient
		contained_by { box { LCorner, RCorner } }
		max_gradient 1.4
		texture { IsoTexture }
		scale 8  translate y*-4
		no_shadow
	}
#end
//***  FRAME 53  ***************************************************
#if ( frame_number = 53 )
	#declare Radius = 4;
	intersection {
		isosurface {
			function  { f_polytubes(x,y,z, 8, -1, -1/100, -0.02, 0.1, -0.25) }
				//P0= nr of wires
				//P1= radial onset (around Y) inverse
				//P2= radial scale (around Y)
				//P3= profile shape
							//positive values = parabolish becoming sharper ith higher values
				//P4= amplitude pos/neg part of profile
				//P5= amplitude profile
			threshold 0.4
			contained_by { sphere { 0, Radius } }
			max_gradient 28
			texture { IsoTexture }
			no_shadow
		}
		sphere { 0, Radius-0.00001  texture { ContTexture } }
		scale 1.6  rotate y*2 rotate x*-18
	}
#end
//***  FRAME 54  ***************************************************
#if ( frame_number = 54 )
	#declare LCorner = <-5.5, -7.3, -5.5>;  #declare RCorner = <5.5, 7.3, 5.5>;
	isosurface {
		function  { f_quantum(x,y,z, 0) }
			//P0= Not used
		contained_by { box { LCorner, RCorner } }
		max_gradient 6
		texture { IsoTexture }
		scale 0.7  rotate x*-20
		no_shadow
	}
#end
//***  FRAME 55  ***************************************************
#if ( frame_number = 55 )
isosurface {
	function  { f_quartic_paraboloid(x,y,z, -0.01) }
		//P0= Field Strength
	contained_by {box { <-1.45, -0.1, -1.45>, <1.45, 2.5, 1.45> }}
	max_gradient 0.2
	texture { IsoTexture }
	no_shadow
	scale 3  rotate y*45  rotate x*20  translate y*-4
}
#end
//***  FRAME 56  ***************************************************
#if ( frame_number = 56 )
	#declare LCorner = <-3.5, -3.0, -3.5>;  #declare RCorner = <3.5, 3.0, 3.5>;
	intersection {
		isosurface {
			function  { f_quartic_saddle(x,y,z, 0.05) }
				//P0= Field Strength
			contained_by { box { LCorner, RCorner } }
			max_gradient 11
			texture { IsoTexture }
		}
		box { LCorner*0.9999, RCorner*0.9999  texture { ContTexture } }
		scale 1.2  rotate y*5  rotate x*-15
		no_shadow
	}
#end
//***  FRAME 57  ***************************************************
#if ( frame_number = 57 )
isosurface {
	function  { f_quartic_cylinder(x,y,z, -1, 0.8, 0.2) }
		//P0= Field Strength
		//P1= Diameter of the "bubble"
		//P2= Controls the width of the tube and the vertical scale of the "bubble"
	contained_by { box { <-0.85, -2.5, -0.85>, <0.85, 2.5, 0.85> } }
	max_gradient 8.8
	texture { IsoTexture }
	scale 2.0
	no_shadow
}
#end
//***  FRAME 58  ***************************************************
#if ( frame_number = 58 )
isosurface {
	function  { f_r(x,y,z) }
	threshold 0.7
	contained_by { sphere { 0, 0.75 } }
	max_gradient 1
	texture { IsoTexture }
	scale 5
}
#end
//***  FRAME 59  ***************************************************
#if ( frame_number = 59 )
	#declare Radius = 4;
	isosurface {
		function  { f_sphere(x,y,z, (Radius-0.5)) + (-f_ridge( x*3, y*3, z*3, 1, 3, 1, 0.2, 0, 0 )*0.9) }
			//P0= Lambda
			//P1= Octaves
			//P2= Omega
			//P3= Offset
			//P4= Ridge
			//P5= noise generator type [0,1,2,3]
		contained_by { sphere { 0, Radius*1.1 } }
		max_gradient 5
		texture { IsoTexture }
	}
#end
//***  FRAME 60  ***************************************************
#if ( frame_number = 60 )
	#declare Radius = 4;
	isosurface {
		function  { f_sphere(x,y,z, Radius-0.5) + (-f_ridged_mf( x*2, y*2, z*3, 2, 3, 1, 0.1, 1, 0 )*2) }
			//P0= H
			//P1= Lacunarity
			//P2= Octaves
			//P3= Offset
			//P4= Gain
			//P5= noise generator type [0,1,2,3]
		contained_by { sphere { 0, Radius*1.1 } }
		max_gradient 10
		texture { IsoTexture }
	}
#end
//***  FRAME 61  ***************************************************
#if ( frame_number = 61 )
	#declare LCorner = <-1.1, -1.1, -1.1>;  #declare RCorner = <1.1, 1.1, 1.1>;
	isosurface {
		function { f_rounded_box(x,y,z, 0.3, 1, 1, 1) }
			//P0= radius rounded corner
			//P1= scale x
			//P2= scale y
			//P3= scale z
		contained_by { box { LCorner, RCorner } }
		max_gradient 1
		texture { IsoTexture }
		scale 3  rotate y*-35  rotate x*-20  translate <0, 0, 0>
		no_shadow
	}
#end
//***  FRAME 62  ***************************************************
#if ( frame_number = 62 )
	#declare Radius = 1.01;
	isosurface {
		function { f_sphere(x,y,z, 1) }
			//P0= Radius
		contained_by { sphere { 0, Radius } }
		max_gradient 1.0
		texture { IsoTexture }
		scale 4
		no_shadow
	}
#end
//***  FRAME 63  ***************************************************
#if ( frame_number = 63 )
	#declare Radius = 2.5;
	isosurface {
		function  { f_spikes(x,y,z, 0.04, 5.6, -4, 0.1, 1) }
			//P0= Spikiness
			//P1= Hollowness
			//P2= Size (<0 with thresh 0)
			//P3= Roundness
			//P4= Fatness
		contained_by { sphere { 0, Radius } }
		max_gradient 11
		texture { IsoTexture }
		scale 2  rotate y*30  rotate x*-18
		no_shadow
	}
#end
//***  FRAME 64  ***************************************************
#if ( frame_number = 64 )
	#declare LCorner = <-1.2,  -0.2, -1.2>;  #declare RCorner = <1.2, 0.7, 1.2>;
	intersection {
		isosurface {
			function  { f_spikes_2d(x,y,z, 0.7, 15, 15, 2.2) }
				//P0= Height central spike
				//P1= X frequency of spikes
				//P2= Z frequency of spikes
				//P3= Height dimming from center
			accuracy  0.0001
			contained_by { box { LCorner, RCorner } }
			max_gradient 10
			texture { IsoTexture }
			no_shadow
		}
		box { LCorner*0.9999, RCorner*0.9999  texture { ContTexture } }
		scale 4.9  rotate x*-25  translate y*-0.7
	}
#end
//***  FRAME 65  ***************************************************
#if ( frame_number = 65 )
	#declare LCorner = <-1, -0.05, -1>;  #declare RCorner = <1, 0.05, 1>;
	isosurface {
		function { f_spiral(x,y,z, 0.2, 0.04, 1, 0, 0, 1) }
			//P0= distance between windings
			//P1= Y thickness
			//P2= outer diameter
			//P3= not used
			//P4= not used
			//P5= cross section shape
		contained_by { box { LCorner, RCorner } }
		max_gradient 1.3
		texture { IsoTexture }
		scale 6.3  rotate x*-30
		no_shadow
	}
#end
//***  FRAME 66  ***************************************************
#if ( frame_number = 66 )
	isosurface {
		function  { f_steiners_roman(x,y,z, -1) }
			//P0= Field Strength
		contained_by { sphere { 0, 0.6 } }
		max_gradient 0.3
		texture { IsoTexture }
		scale 8.5  rotate y*25  rotate x*-25 
		no_shadow
	}
#end
//***  FRAME 67  ***************************************************
#if ( frame_number = 67 )
	#declare Radius = 2.4;
	intersection {
		isosurface {
			function  { f_strophoid(x,y,z, -0.2, 2, 0.5, 0.9) }
				//P0= Field Strength
				//P1= Size of bulb. Larger values give larger bulbs. Negative values give a bulb on the other side of the plane
				//P2= Sharpness.
				//			When 0, the bulb is like a sphere that just touches the plane.
				//			When positive, there is a crossover point
				//			When negative the bulb simply bulges out of the plane like a pimple
				//P3= Flatness. Higher values make the top end of the bulb fatter
			contained_by { sphere { 0, 2.4 } }
			max_gradient 1.4
			texture { IsoTexture }
		}
		sphere { 0, Radius-0.00001  texture { ContTexture } }
		scale 2  rotate y*-28  rotate x*10  translate x*0.6
		no_shadow
	}
#end
//***  FRAME 68  ***************************************************
#if ( frame_number = 68 )
	//**  f_strophoid_2d  : lathe form  ***************************************
	#declare Radius = 8.5;
	isosurface {
		function  { f_strophoid_2d(x,y,z, 0.02, -8, 0.8, 0.3, 1, 0, 0) }
			//P0= Field Strength
			//P1= Size of bulb
			//			Larger values give larger bulbs
			//			Negative values give a bumb on the other side of the plane
			//P2= Sharpness
			//			When 0, the bulb is like a sphere that just touches the plane.
			//			When positive, there is a crossover point
			//			When negative the bulb simply bulges out of the plane like a pimple
			//P3= Flatness. Higher values make the top end of the bulb fatter
			//P4= SOR switch
			//P5= SOR offset
			//P6= SOR angle
		contained_by { sphere { 0, Radius } }
		max_gradient 1.4
		texture { IsoTexture }
		scale 0.4  rotate x*-10  translate <2.6, -1, 0>
	}
	//**  f_strophoid_2d  : extruded form  ***************************************
	#declare LCorner = <-1.0, -8.0, -0.01>;  #declare RCorner = <8.5, 8.0, 14>;
	intersection {
		isosurface {
			function  { f_strophoid_2d(x,y,z, 0.02, -8, 0.8, 0.3, 0, 0, 0) }
				//P0= Field Strength
				//P1= Size of bulb
				//			Larger values give larger bulbs
				//			Negative values give a bumb on the other side of the plane
				//P2= Sharpness
				//			When 0, the bulb is like a sphere that just touches the plane.
				//			When positive, there is a crossover point
				//			When negative the bulb simply bulges out of the plane like a pimple
				//P3= Flatness. Higher values make the top end of the bulb fatter
				//P4= SOR switch
				//P5= SOR offset
				//P6= SOR angle
			contained_by { box { LCorner, RCorner } }
			max_gradient 1.6
			texture { IsoTexture }
		}
		box { LCorner*0.9999, RCorner*0.9999  texture { ContTexture } }
		scale 0.4  rotate x*-10  translate <-5, -1, 0>
	}
#end
//***  FRAME 69  ***************************************************
#if ( frame_number = 69 )
	#declare Radius = 1.4;
	isosurface {
		function { -f_superellipsoid(x,y,z, 0.3, 0.5) }
			//P0= rounding NS
			//P1= rounding EW
		contained_by { sphere { 0, Radius } }
		max_gradient 0.9
		texture { IsoTexture }
		scale 3.8  rotate y*-40  rotate x*-20
		no_shadow
	}
#end
//***  FRAME 70  ***************************************************
#if ( frame_number = 70 )
	isosurface {
		function { f_sphere(x,y,z, 1) + sin(f_th(x,y,z)*20)*0.05*(1-y*y) }
		contained_by { sphere { 0, 1.1 } }
		max_gradient 1.5
		texture { IsoTexture }
		scale 4 rotate x*-35
	}
#end
//***  FRAME 71  ***************************************************
#if ( frame_number = 71 )
	#declare LCorner = <-1, -0.25, -1>;  #declare RCorner = <1, 0.25, 1>;
	isosurface {
		function { f_torus(x,y,z, 0.8, 0.2) }
			//P0= major radius
			//P1= minor radius
		contained_by { box { LCorner, RCorner } }
		max_gradient 1
		texture { IsoTexture }
		scale 5.5  rotate x*-30
		no_shadow
	}
#end
//***  FRAME 72  ***************************************************
#if ( frame_number = 72 )
isosurface {
	function  { f_torus2(x,y,z, -1, 0.4, 0.07) }
		//P0= Field Strength
		//P1= Major radius
		//P2= Minor radius
	contained_by { box {<-0.5, -0.1, -0.5>, <0.5, 0.1, 0.5> } }
	max_gradient 0.6
	texture { IsoTexture }
	scale 12  rotate x*-35
	no_shadow
}
#end
//***  FRAME 73  ***************************************************
#if ( frame_number = 73 )
	isosurface {
		function  { f_torus_gumdrop(x,y,z, -0.01) }
			//P0= Field Strength
		contained_by { sphere { 0, 2.1 } }
		max_gradient 1.1
		texture { IsoTexture }
		no_shadow
		scale 2  rotate y*18
	}
#end
//***  FRAME 74  ***************************************************
#if ( frame_number = 74 )
	#declare LCorner = <-3.3, -0.2, -2.5>;  #declare RCorner = <3.3, 3.5, 3.0>;
	intersection {
		isosurface {
			function  { f_umbrella(x,y,z, -0.2) }
				//P0= Field Strength
			contained_by { box { LCorner, RCorner } }
			max_gradient 2.6
			texture { IsoTexture }
			no_shadow
		}
		box { LCorner*0.9999, RCorner*0.9999  texture { ContTexture } }
		scale 1.5  rotate x*60  translate y*-0.7
	}
#end
//***  FRAME 75  ***************************************************
#if ( frame_number = 75 )
	#declare LCorner = <-1.8, -0.15, -1.8>;  #declare RCorner = <1.8, 1.1, 1.8>;
	intersection {
		isosurface {
			function  { f_witch_of_agnesi(x,y,z, -0.09, 0.02) }
				//P0= Field Strength
				//P1= Controls width of the spike. The height of the spike is always about 1 unit
			contained_by { box { LCorner, RCorner } }
			max_gradient 0.6
			texture { IsoTexture }
			no_shadow
		}
		box { LCorner*0.9999, RCorner*0.9999  texture { ContTexture } }
		scale 3  rotate x*-20  translate y*-1.9
	}
#end
//***  FRAME 76  ***************************************************
#if ( frame_number = 76 )
	//**  f_witch_of_agnesi_2d  : lathe form  ***************************************
	#declare LCorner = <-1.6, -0.2, -2.2>;  #declare RCorner = <1.6, 2.2, 2.2>;
	intersection {
		isosurface {
			function  { f_witch_of_agnesi_2d(x,y,z, -0.2, 0.2, 0.08, 1, 0, 0) }
				//P0= Field Strength
				//P1= Controls width of the spike.
				//P2= Controls the height of the spike
				//P3= SOR switch
				//P4= SOR offset
				//P5= SOR angle
			contained_by { box { LCorner, RCorner } }
			max_gradient 1.9
			texture { IsoTexture }
		}
		box { LCorner*0.9999, RCorner*0.9999  texture { ContTexture } }
		scale 1.8  rotate x*-15  translate <3.1, -1.5, 0>
	}
	//** f_witch_of_agnesi_2d  : extruded form  ***************************************
	#declare LCorner = <-1.6, -0.2, -2.2>;  #declare RCorner = <1.6, 2.2, 2.2>;
	intersection {
		isosurface {
			function  { f_witch_of_agnesi_2d(x,y,z, -0.2, 0.2, 0.08, 0, 0, 0) }
				//P0= Field Strength
				//P1= Controls width of the spike.
				//P2= Controls the height of the spike
				//P3= SOR switch
				//P4= SOR offset
				//P5= SOR angle
			contained_by { box { LCorner, RCorner } }
			max_gradient 0.8
			texture { IsoTexture }
		}
		box { LCorner*0.9999, RCorner*0.9999  texture { ContTexture } }
		scale 1.8  rotate x*-15  translate <-3.1, -1.5, 0>
	}
#end
//***  FRAME 77  ***************************************************
#if ( frame_number = 77 )
	#declare Radius = 4;
	isosurface {
		function  { f_sphere(x,y,z, 4) + f_noise3d(x*1/0.5,y*1/0.3,z*1/0.4)*0.8 }
		contained_by { sphere { 0, Radius } }
		evaluate 1, 10, 0.99
		max_gradient 1.4
		texture { IsoTexture }
	}
#end
//***  FRAME 78  ***************************************************
#if ( frame_number = 78 )
	#declare Radius = 4;
	isosurface {
//		function  { f_sphere(x,y,z, 4) + f_noise_generator (x*1/0.5,y*1/0.3,z*1/0.4, 1)*0.8 }
		function  { f_sphere(x,y,z, 4) + f_noise_generator (x,y,z, 1)*0.8 }
				//P0= type of noise_generator used (0, 1, 2 or 3)
		contained_by { sphere { 0, Radius } }
		evaluate 1, 10, 0.99
		max_gradient 1.4
		texture { IsoTexture }
	}
#end
//***  END OF FILE  *************************************************/
      
