// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence of Vision Ray Tracer version 3.6 / 3.7 Include File
// file:   shapes3.inc
// author: Friedrich A. Lohmueller, March-2013
// 
// Update: Dec-2013 - Corrected wrong rotation in 'Round_Pyramid_N_in'. 
//
// Description: This file contains macros for working with object shapes,
// as well as macros for creating shapes of special geometric objects.
//    
// Segments of shapes
// #macro Segment_of_Object ( Segment_Object, Segment_Angle)
// #macro Segment_of_CylinderRing ( R_out, R_in, Height, Segment_Angle)
// #macro Segment_of_Torus ( R_major, R_minor, Segment_Angle)
//
// Angular shapes  
// #macro Column_N    (N, Radius_in, Height ) 
// #macro Column_N_AB (N, A, B, R_in)
// #macro Pyramid_N   (N, Radius_in_1, Radius_in_2, Height )
// #macro Pyramid_N_AB(N, A, R_in_A, B, R_in_B)             
// 
// Facetted shapes  
// #declare Egg  (uses #macro Egg_Shape) 
// #macro Facetted_Sphere (Quarter_Segments, Radial_Segments) 
// #macro Facetted_Egg_Shape (Quarter_Segments, Radial_Segments, Lower_Scale, Upper_Scale)
// #macro Facetted_Egg(N_Quarter_Segments, N_Radial_Segments) 
// 
// Round shape 
// #macro Egg_Shape (Lower_Scale, Upper_Scale) 
// 
// Wireframe shape  
// #macro Ring_Sphere (Rmaj_H, Rmaj_V, Rmin_H, Rmin_V, Number_of_Rings_horizontal, Number_of_Rings_vertical)
//
// Rounded shapes
// #macro Round_Pyramid_N_out (N, A, CornerR_out_A, B, CornerR_out_B, R_Border, Filled, Merge ) 
// #macro Round_Pyramid_N_in  (N, A, FaceR_in_A, B, FaceR_in_B, R_Border, Filled, Merge_On ) 
//
// #macro Round_Cylinder_Tube( A, B, R_major, R_minor, Filled, Merge)
// #macro Rounded_Tube( R_out, R_in, R_Border,  Height,  Merge)
// #macro Rounded_Tube_AB( A, B, R_out, R_in, R_Border, Merge)
//
// #macro Round_Conic_Torus( Center_Distance, R_upper, R_lower, R_Border, Merge)
// #macro Round_Conic_Prism( Center_Distance, R_upper, R_lower, Length_Zminus, R_Border, Merge) 
// #macro Half_Hollowed_Rounded_Cylinder1( Length, R_out, R_border, BorderScale, Merge)
// #macro Half_Hollowed_Rounded_Cylinder2( Length, R_out, R_corner, R_border, BorderScale, Merge)
// 
// #macro Round_N_Tube_Polygon (N, Tube_R, R_incircle, Edge_R, Filled, Merge)  
//  
//
// 
// 
// 
// --------------------------------------------------------

#ifndef( Shapes3_Inc_Temp)
#declare Shapes3_Inc_Temp = version;
#version 3.6; 

#ifdef(View_POV_Include_Stack)
   #debug "including shapes3.inc\n"
#end   
 
#ifndef ( SHAPES_INC_TEMP) //Shapes_Inc_Temp)      
#include "shapes.inc"                                             
#end

#ifndef ( TRANSFORMS_INC_TEMP )
#include "transforms.inc"
#end 

#ifndef ( MATH_INC_TEMP )
#include "math.inc"
#end 

#ifndef ( STRINGS_INC_TEMP)
#include "strings.inc"
#end

// -----------------------------------------------------------------------------------------


// -----------------------------------------------------------------------------------------
//-------------------------------------------------------------<<< macro Segment_of_Object() 
#macro  Segment_of_Object ( SEgment_OBject, Segment_Angle_)
// cuts out a segment of an shape object radial around y axis  
// starting with the +x direction 
// ----------------------------------------------------------------------------------  
#local D = 0.000001; // just a little bit
#local Segment_Angle = Segment_Angle_;

#if (Segment_Angle = 0) #local Segment_Angle = D; #end

#if (abs(Segment_Angle) >= 360) #local Segment_Angle = mod (Segment_Angle, 360); #end

#local O_min = min_extent ( SEgment_OBject );
#local O_max = max_extent ( SEgment_OBject );

#local O_max_x = max (O_min.x, O_max.x); 
#local O_max_z = max (O_min.z, O_max.z); 

#local R_max = 1.5*max(O_max_x,O_max_z);
  
#if (Segment_Angle > 0) 
   #local Box_z = R_max+D;  
#else 
   #local Box_z = -R_max+D; 
#end
 
 intersection{ 
  object{ SEgment_OBject }

  #if (abs(Segment_Angle) >= 180) 
  merge{
  #end // then use merge!

   box { <-R_max+D,O_min.y-D,0>,< R_max+D, O_max.y+D,-Box_z> 
         rotate<0,0,0> 
       }// end of box
 
   box { <-R_max+D,O_min.y-D, Box_z>,< R_max+D, O_max.y+D,0> 
         rotate<0, Segment_Angle,0> 
        }// end of box

  #if (abs(Segment_Angle) >= 180) 
   } // end of merge
  #end // end of merge, if merge is used!
  
} // end of intersection  

#end // end of macro -----------------------------------<<< end of macro Segment_of_Object() 
// -----------------------------------------------------------------------------------------
 


// -----------------------------------------------------------------------------------------
// ------------------------------------------------------<<< macro Segment_of_CylinderRing()
#macro  Segment_of_CylinderRing ( R_out, R_in, Height, Segment_Angle_)
// ------------------------------
#local D = 0.000001; // just a little bit

#local R_o = R_out; 
#local R_i = R_in; 
#local H = Height; 
#local Segment_Angle = Segment_Angle_; 


 #if (H = 0 ) #local  H = D; #end
 #if (H < 0 ) #local  D = -D; #end

 #if (R_o < R_i) #local X=R_o; #local R_o=R_i; #local R_i=X; #end
 #if (R_i <= 0) #local R_i = D; #end
 
 #if (Segment_Angle < 0) 
      #local Negativ_Flag = 1; 
      #local Segment_Angle = -Segment_Angle; 
 #else 
      #local Negativ_Flag = 0;
 #end
 

 #if (Segment_Angle >= 360) #local Segment_Angle = mod (Segment_Angle, 360); #end

 intersection{ 
   cylinder { <0,0,0>,<0,H,0>, R_o 
            } // end of outer cylinder  ----------
   cylinder { <0,-D,0>,<0,H+D,0>, R_i 
              inverse
            } // end of inner cylinder  ----------


  #if (Segment_Angle > 0) // ------------------------------------------------------
  #if (Segment_Angle >= 180)
  merge{
  #end // then use merge!

   box { <-R_o+D,-D,0>,< R_o+D, H+D, R_o+D> 
         rotate<0,0,0> 
       }// end of box
 
   box { <-R_o+D,-D,-R_o+D>,< R_o+D, H+D,0> 
         rotate<0,  Segment_Angle,0>  
       }// end of box

  #if (Segment_Angle >= 180)
   } // end of merge
  #end // end of merge, if merge is used!
  
 #if (Negativ_Flag = 1)  rotate<0,-Segment_Angle,0>   #end   
 scale<-1,1,-1> 
 #end // of "#if (Segment_Angle > 0)" --------------------------------------------

} // end of intersection  

#end // end of macro -----------------------------<<< end of macro Segment_of_CylinderRing()  
// -----------------------------------------------------------------------------------------

                                               
 
// -----------------------------------------------------------------------------------------
//--------------------------------------------------------------<<< macro Segment_of_Torus()
#macro Segment_of_Torus ( R_major_, R_minor_, Segment_Angle_)
//------------------------------------------------------------------------------------------
 #local D =  0.000001;
 #local R_major = R_major_;
 #local R_minor = R_minor_;
 #local Segment_Angle = Segment_Angle_;

 #if (Segment_Angle < 0) 
      #local Negativ_Flag = 1; 
      #local Segment_Angle = -Segment_Angle; 
 #else 
      #local Negativ_Flag = 0;
 #end

#if (Segment_Angle > 360) #local Segment_Angle = mod(Segment_Angle,360); #end
intersection{
 torus { R_major, R_minor sturm }

#if (Segment_Angle > 180)
 merge{
#end // use merge!

 box   { <-1,-1,0>,<1,1,1>
         scale < R_major+R_minor+D, R_minor+D, R_major+R_minor+D>
       }// end of box
 box   { <-1,-1,-1>,<1,1,0>
         scale < R_major+R_minor+D, R_minor+D, R_major+R_minor+D>
         rotate < 0,-Segment_Angle,0 >
       }// end of box

 #if (Segment_Angle > 180)
 }
 #end // end of merge, if merge is used!

 #if (Negativ_Flag = 0)  rotate<0,Segment_Angle,0>   #end 

 } // end of intersection

#end  // end of macro Torus_Segment( ... ) --------------<<< end of macro Segment_of_Torus()
// -----------------------------------------------------------------------------------------


//------------------------------------------------------------------------------ /////////
//----------------------------------------------------- Round_Tube_Polygon_N (...) macro
#macro Round_N_Tube_Polygon( // A round polygon tube ring with N corners, filled or not!
                             N_in, // number of corners
                             Tube_R_in, // tube radius
                             Base_Width_in, // R_incircle (center to edge middle)
                             Corner_R_in, //  corner torus segment major radius
                             Filled, // 1 = filled, 0 = ring, filling percentage
                             Merge_On // 0 = union, 1 = merge

                           ) //-----------------------------------------------
//---------------------------------------------------------------------------
#local D = 0.000001; // just a little bit
//---------------------------------------------------------------------------
// check inputs:
#local Corner_R = abs(Corner_R_in);
#local Base_Width = abs(Base_Width_in);
#if (Corner_R > Base_Width)
    #local Corner_R=Base_Width;
    #debug concat( "Attention: Corner radius > base width. Set corner radius = base width !","\n")
#end

#local N = N_in;
#if (int(N) != N ) #local N = int(N);
    #debug concat( "Attention: Number of corners should be an integer!","\n")
    #debug concat( "           Number of corners set to int(number of corners)","\n")
#end
#if (N < 3 ) #local N = 3;
    #debug concat( "Attention: Number of corners should be > 3. Set mumber of corners to 3 !","\n")
#end

#local Tube_R  = Tube_R_in;
#if (Tube_R <= 0 )
  #if (abs(Tube_R)<= 0.00001 )
  #local Tube_R    = 0.00001;
  #else
  #local Tube_R = abs(Tube_R);
  #end
  #debug concat( "Attention: Tube radius should > 0.00001. Tube radius set to max( abs(tube radius), 0.00001) !","\n")
  #debug concat( "           This could be unvisible in this scene !","\n")
#end
// --------------------------------------------------------------------------
#local Edge_Angle = 360/N ;
#local Linear_Half_Len = (Base_Width-Corner_R)*tan(radians(Edge_Angle/2));

//---------------------------------------------------------------------------
#local Edge_Part =
#if( Filled > 0)
 #if( Merge_On = 1 )
 merge{
 #else
 union{
 #end // #if(Merge_On = 1 )
#end // #if(Filled > 0)

object{ Segment_of_Torus( Corner_R, Tube_R, -Edge_Angle)
        rotate<-90,0,0> translate<Base_Width-Corner_R,Linear_Half_Len,0>
      } // end of Torus_Segment(...)

#if( Filled > 0)
cylinder{ <0,0,-Tube_R*Filled>,<0,0,Tube_R*Filled>, Corner_R
          translate<Base_Width-Corner_R,Linear_Half_Len,0>
        }

}// end union or merge
#end // #if(Filled > 0)

//---------------------------------------------------------------------------
#if (Corner_R != Base_Width)


#local Linear_Part =
#if( Filled > 0)
 #if( Merge_On = 1 )
 merge{
 #else
 union{
 #end // #if(Merge_On = 1 )
#end // #if(Filled > 0)

cylinder { <0,-Linear_Half_Len-D,0>,<0,Linear_Half_Len+D,0>,Tube_R
           scale <1,1,1> rotate<0,0,0> translate<Base_Width,0,0>
         } // end of cylinder


#if( Filled > 0)
// linear prism in z-direction: from ,to ,number of points (first = last)
prism{ -Tube_R*Filled-D ,Tube_R*Filled+D , 6
       <-D, 0.00>,  // first point
       < Base_Width-Corner_R-D,-Linear_Half_Len-D>,
       < Base_Width           ,-Linear_Half_Len-D>,
       < Base_Width           , Linear_Half_Len+D>,
       < Base_Width-Corner_R-D, Linear_Half_Len+D>,
       <-D, 0.00>  // last point = first point!!!!
       rotate<-90,0,0> scale<1,1,-1> //turns prism in z direction! Don't change this line!
     } // end of prism --------------------------------------------------------

}// end union or merge
#end // #if(Filled_On = 1)


#end // #if (Corner_R != Base_Width)
//---------------------------------------------------------------------------

#if (Corner_R != Base_Width)
#local One_Segment =
 #if(Merge_On = 1 )
 merge{
 #else
 union{
 #end
        object {Linear_Part}
        object {Edge_Part}
        translate<0,0,0>
     } // end union or merge
#else
 #local One_Segment =
        object {Edge_Part}
#end
//---------------------------------------------------------------------------
// final union or merge
#if(Merge_On = 1 )
merge{
#else
union{
#end

  #local Nr = 0;     // start
  #local EndNr = N; // end
  #while (Nr< EndNr)
    object{One_Segment rotate<0,0,Nr * 360/EndNr>}

  #local Nr = Nr + 1;    // next Nr
  #end // ---------------  end of loop
} // end union or merge

// --------------------------------------------------------------------------------------
#end// of macro ------------------------------------------------------// end of macro
// -----------------------------------------------------------------------------------------

 
// -----------------------------------------------------------------------------------------
// --------------------------------------------------------------------<<< macro Pyramid_N()
#macro Pyramid_N (N, Radius1, Radius2, Height ) 
// ----------------------------------------------------------------------------- 
#local D= 0.000001; // a little bit to avoid coincident surfaces in intersection

intersection{
 #local Nr = 0;    // start
 #local EndNr = N; // end
 #while (Nr< EndNr) 
   
  // linear prism in z-direction: from ,to ,number of points (first = last)
  prism { -2.00 ,2.00 , 5
         <-2.00, 0.00-Nr*D>,
         < 1.00,0.00-Nr*D>,
         < 0.00+Radius2/Radius1,1.00+Nr*D>,
         <-2.00,1.00+Nr*D>,
         <-2.00,0.00-Nr*D>
         rotate<-90,0,0> scale<1,1,-1> //turns prism in z direction! 
         scale<Radius1+Nr*D,Height+Nr*D,Radius1+Nr*D>
         rotate<0,Nr * 360/EndNr,0>
     } // end of prism -------------------------------------------------------------

 #local Nr = Nr + 1;    // next Nr
 #end // ----------------  end of loop 
} // end of intersection

#end // ---------------------------<<< end of macro Pyramid_N (N, Radius1, Radius2, Height ) 
// -----------------------------------------------------------------------------------------


// -----------------------------------------------------------------------------------------
// -----------------------------------------------------------------<<< macro Pyramid_N_AB()
#macro Pyramid_N_AB (N, Point_A, Radius_A, Point_B, Radius_B ) 
// ----------------------------------------------------------------------------- 
#local A = Point_A; 
#local B = Point_B; 
#local AB = B-A;  
#local H  = vlength( AB); // pyramid height;

object{ Pyramid_N ( N, Radius_A, Radius_B, H ) 
        Reorient_Trans(< 0,1,0>, AB ) // needs "transforms.inc":
        translate A 
      }  //
#end // -------------<<< end of macro Pyramid_N_AB(N, Point_A, Radius_A, Point_B, Radius_B ) 
// -----------------------------------------------------------------------------------------


// --------------------------------------------------------------------<<< macro Column_N()
#macro Column_N  (N, Radius, Height ) 
//------------------------------------------------------------------
object{ Pyramid_N (N, Radius, Radius, Height )
      }
#end // -------------------------------------<<< end of  macro N_Column (N, Radius, Height )
// -----------------------------------------------------------------------------------------


// -----------------------------------------------------------------------------------------
// -----------------------------------------------------------------<<< macro Column_N_AB()
#macro Column_N_AB (N, Point_A, Point_B, Radius ) 
// ----------------------------------------------------------------------------- 
#local A = Point_A; 
#local B = Point_B; 
#local AB = B-A;  
#local H  = vlength( AB); // pyramid height;

object{ Pyramid_N ( N, Radius, Radius, H ) 
        Reorient_Trans(< 0,1,0>, AB ) // needs "transforms.inc":
        translate A 
      }  //
#end // --------------------------<<< end of macro Column_N_AB(N, Point_A, Point_B, Radius ) 
// -----------------------------------------------------------------------------------------


// -----------------------------------------------------------------------------------------
// --------------------------------------------------------------------<<< macro Egg_Shape() 
#macro Egg_Shape (Lower_Scale, Upper_Scale)                  // 
// ------------------------------------------------------------
#local Egg_Lower_Part =
         difference {
                      sphere{<0,0,0>,1 scale<1,Lower_Scale,1>}
                      box{<-1,0,-1>,<1,Lower_Scale,1>}
                    } //---------------------------------------
#local Egg_Upper_Part =
         difference {
                      sphere {<0,0,0>,1 scale<1,Upper_Scale,1>}
                      box {<-1,-Upper_Scale,-1>,<1,0,1>}
                     }//---------------------------------------
//-------------------------------------------------------------
  merge { 
          object {Egg_Upper_Part}
          object {Egg_Lower_Part}
          translate<0,Lower_Scale,0>
          scale 2/(Lower_Scale+Upper_Scale) 
           
        } // end of merge ------------------------------------
#end //---------------------------------------------------------<<< end of macro Egg_Shape()
// -----------------------------------------------------------------------------------------


// -------------------------------------------------------- shape of simple egg: object Egg
#declare Egg = object { Egg_Shape (1.15,1.55)} 
// ---------------------------------------------------------------<<< end of the object Egg


// -----------------------------------------------------------------------------------------
// --------------------------------------------------------------<<< macro Facetted_Sphere()
#macro Facetted_Sphere (Quarter_Meridian_Segments, Equatorial_Segments)

#local Facets_Silhouette =
 prism { 
   -2 ,2 , 
   2*Quarter_Meridian_Segments+4
   < -2,-1.00>, 

   #local Nr    = -Quarter_Meridian_Segments; 
   #local EndNr =  Quarter_Meridian_Segments;
   #while (Nr< EndNr+1)
     #local Angle_degrees = Nr* 90/EndNr;
     #local Angle_radians = radians(Angle_degrees);
   < cos(Angle_radians) , sin(Angle_radians)>,
     
   #local Nr = Nr + 1 ;     
   #end       
   < -2, 1>,
   < -2,-1> 
      
 rotate<-90,0,0> scale<1,1,-1> //turns prism in z direction!  
 } // end of prism object ---------------------------------- 

intersection{
 #local Nr = 0;                  // start
 #local EndNr = Equatorial_Segments; // end
 #while (Nr< EndNr) 
 
 object{ Facets_Silhouette rotate<0,Nr * 360/EndNr,0>} 

 #local Nr = Nr + 1;    // next Nr
 #end // ---------------  end of loop 

} // end of intersection

#end // --------------------------------------------------<<< end of macro Facetted_Sphere()
// -----------------------------------------------------------------------------------------


// -----------------------------------------------------------------------------------------
// -----------------------------------------------------------<<< macro Facetted_Egg_Shape()
#macro Facetted_Egg_Shape (Quarter_Segments, Radial_Segments, Lower_Scale, Upper_Scale)
     //Facettierte_Kugel (Viertelskreis_Teilung, Radial_Teilung)
#local Facets_Silhouette =
union{
 prism { 
   -2 ,2 ,  Quarter_Segments +4
   < -2,-1.00>, 
 #local Nr    =  -Quarter_Segments; 
 #local EndNr =  0;
 #while (Nr< EndNr+1)
   #local Angle_degrees = Nr* 90/Quarter_Segments;
   #local Angle_radians = radians(Angle_degrees);
   < cos (Angle_radians) , sin (Angle_radians)> ,
 #local Nr = Nr + 1 ;     
 #end       
   < -2, 0>, 
   < -2,-1.00> 
 rotate<-90,0,0> scale<1,1,-1> //turns prism in z direction! Don't change this line! 
 scale<1,Lower_Scale,1>
 } // end of prism object ----------------------------------------------------------

 prism { 
  -2 ,2 , Quarter_Segments+4
  < -2, 0>, 
  #local Nr    =  0; 
  #local EndNr =  Quarter_Segments;
  #while (Nr< EndNr+1)
   #local Angle_degrees = Nr* 90/Quarter_Segments;
   #local Angle_radians = radians(Angle_degrees);
  < cos (Angle_radians) , sin (Angle_radians)> ,
  #local Nr = Nr + 1 ;     
  #end       
  < -2, 1>,
  < -2, 0> 
 rotate<-90,0,0> scale<1,1,-1> //turns prism in z direction! 
 scale<1,Upper_Scale,1>
 } // end of prism object -------------------------------------------
}// end of union

intersection{
 #local Nr = 0;                  // start
 #local EndNr = Radial_Segments; // end
 #while (Nr< EndNr) 
 
 object{ Facets_Silhouette rotate<0,Nr * 360/EndNr,0>} 

 #local Nr = Nr + 1;    // next Nr
 #end // ---------------  end of loop 
} // end of intersection

#end // ----------------------------------------------------<<<< end of macro Facetted_Egg() 
// -----------------------------------------------------------------------------------------


// -----------------------------------------------------------------------------------------
// -----------------------------------------------------------------<<< macro Facetted_Egg()
#macro Facetted_Egg(N_Quarter_Segments, N_Radial_Segments) 
  object{ Facetted_Egg_Shape(N_Quarter_Segments, N_Radial_Segments, 1.15, 1.55)
          translate < 0, 1.15, 0>
          scale 2/(1.15 + 1.55)
         }
#end                                                                                   
//------------------------------------------------------------<<<end of macro Facetted_Egg()
// -----------------------------------------------------------------------------------------
  

// -----------------------------------------------------------------------------------------
// ------------------------------------------------------------------<<< macro Ring_Sphere()
//Sphere with latitudes by steps in degrees - globus 
#macro Ring_Sphere( Rmaj_H, Rmaj_V, Rmin_H, Rmin_V, 
                    Number_of_Rings_horizontal, Number_of_Rings_vertical)

#if( (Rmin_H > 0) & (Number_of_Rings_horizontal > 0))
#local RingsH1 = 
union{
#local AngleD = 180/ (Number_of_Rings_horizontal+1);
#local Nr = -90+AngleD; #local EndNr = 90; // --- start and end
// Nr = value of the angle 

#while (Nr< EndNr)
 #local RingR = Rmaj_H*cos(radians(Nr)); //sqrt( pow(R0,2) - pow((Nr*HDiff),2) );
 #local RingH = Rmaj_H*sin(radians(Nr));
 torus{RingR,Rmin_H scale <1,1,1>
       rotate<0,0,0>
       translate<0, RingH,0>} 
#local Nr = Nr + AngleD;
#end // --------------- end of loop 
} // -----------------
#end // of "#if ( (Rmin_H > 0) & (Number_of_Rings_horizontal > 0))"

#if ((Rmin_V > 0) &(Number_of_Rings_vertical > 0))
#local RingsV1 =          // longitudes 
union{
#local Nr = 0; #local EndNr = Number_of_Rings_vertical; // --- start and end
#while (Nr< EndNr)
 torus{Rmaj_V-Rmin_V,Rmin_V scale <1,1,1>
       rotate<90,0,0>
       rotate<0, Nr*360/EndNr,0>} 
#local Nr = Nr + 1;
#end // --------------- end of loop 
} // ---------------------------------
#end // of "#if ((Rmin_V > 0) &(Number_of_Rings_vertical > 0))"

union{ #if ((Rmin_H > 0) & (Number_of_Rings_horizontal > 0)) object{ RingsH1} #end
       #if ((Rmin_V > 0) & (Number_of_Rings_vertical   > 0)) object{ RingsV1} #end
       sphere{<0, Rmaj_H,0>,Rmin_H} 
       sphere{<0,-Rmaj_H,0>,Rmin_H} 
     }  
#end // ------------------------------------------------------<<< end of macro Ring_Sphere()
// -----------------------------------------------------------------------------------------
 

// -----------------------------------------------------------------------------------------
// ---------------------------------------------------------<<<   macro Round_Pyramid_N_in()
#macro Round_Pyramid_N_in ( // radius of the incircle radius
            Number_of_Sidefaces, // >=3 
            Point_A, Radius_A_in, Point_B, Radius_B_in,// radii of edge middles (R_in_A,R_in_B) 
            Wire_Radius, //  border radius (Fill_On = 1) or wire radius ( Fill_On = 0 )
            Fill_On,     //  1 = filled, 0 = wireframe,
            Merge_On     //  1 = use merge, 0 = use union
          ) // -------------------------------------------------   
// -------------------------------------------
// calculating the radius of the circumcircle: 
#local Half_Angle = 180/Number_of_Sidefaces;
#local R_out_A = Radius_A_in*sqrt( 1 + pow(tan(radians(Half_Angle)),2) ) ; 
#local R_out_B = Radius_B_in*sqrt( 1 + pow(tan(radians(Half_Angle)),2) ) ; 
// ------------------------------------------------------------- 
object{ Round_Pyramid_N_out ( // used radius of the circumcircle 
            Number_of_Sidefaces, // >=3 
            Point_A, R_out_A , Point_B, R_out_B,// radii of corner points (R_out_A,R_out_B) 
            Wire_Radius, //  border radius (Fill_On = 1) or wire radius ( Fill_On = 0 )
            Fill_On,     //  1 = filled, 0 = wireframe,
            Merge_On     //  1 = use merge, 0 = use union
          ) // -------------------------------------------------   
  //
translate -Point_A
Axis_Rotate_Trans(  Point_B-  Point_A ,Half_Angle) 
translate  Point_A
} // end of object
#end// -----------------------------------------------<<<  end of macro Round_Pyramid_N_in()
// -----------------------------------------------------------------------------------------
 
// -----------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------
// --------------------------------------------------------<<<   macro Round_Pyramid_N_out()
#macro Round_Pyramid_N_out ( // used radius of the circumcircle 
            Number_of_Sidefaces, // >=3 
            Point_A, Radius_A, Point_B, Radius_B,// radii of corner points (R_out_A,R_out_B) 
            Wire_Radius, //  border radius (Fill_On = 1) or wire radius ( Fill_On = 0 )
            Fill_On,     //  1 = filled, 0 = wireframe,
            Merge_On     //  1 = use merge, 0 = use union
          ) // -----------------------------------------------------------------------------  
#local D = 0.000001;
#local N  = Number_of_Sidefaces; 
#local Rw = Wire_Radius; 

#local A  = Point_A;  
#local B  = Point_B;  
#local AB = B-A;  

#local Rc1= Radius_A-Rw; // base corner radius 
#local Rc2= Radius_B-Rw; // top  corner radius
#local H  = vlength( AB)-2*Rw;  // Pyramid_Height;

#if ( H <= 0 ) // ---------------------------
#debug "Warning: H must be > 0 "
#local H = 0 ;
#end // ----------------------------------------
 
#local D = 0.000001;    
#if ( N < 3) #local N = 3; #end // 3 is minimum!!!
#if ( Rw = 0 ) #local Rw = D; #end  
#if ( Rw < 0 ) #local Rw = abs(Rw) ; #end
#if ( Rc1 < 0 ) #local Rc1 = abs(Rc1) ; #end
#if ( Rc2 < 0 ) #local Rc2 = abs(Rc2) ; #end
// ---------------------------------------------------------------------------------------------------
#local Flip = 0;       
#if ( Rc1 < Rc2 ) #local Delta_Xchange = Rc1; #local Rc1 = Rc2; #local Rc2 = Delta_Xchange; 
#local Flip = 1;       
#end // --------
// ---------------------------------------------------------------------------------------------------
// radii to middle of the horizontal edges
#local Re1 = Rc1*cos(radians(180/N));
#local Re2 = Rc2*cos(radians(180/N));
// length of the hor. edges: 
#local HLe1 = Rc1*sin(radians(180/N)); // half length of base edge
#local HLe2 = Rc2*sin(radians(180/N)); // half length of top edge
//
#local P_Angle = degrees(atan2( H, (Re1-Re2))); // pending sides angle against vertical !!
// -----------------------------------------------------------
// wire radius depending relevant values
#local Rw_sin = Rw*sin(radians(P_Angle)); // side difference
#local Rw_cos = Rw*cos(radians(P_Angle)); // height diffence
//----------------------------------------------------------------------

//  wireframe + filling ------------------------------------------------ global union
#if (Merge_On = 1)   
merge{
#else
union{ // I 
#end // of #if (Merge_On = 1) .............    

 //---------------------------- Wireframe
 #if (Rc1 = 0 )
  sphere{< 0, H, 0>,Rw } // only one base sphere 
 #end 
 #if (Rc2 = 0 )
  sphere{< 0, H, 0>,Rw } // only one top sphere 
 #end 
 
 #local Nr = 0; 
 #while (Nr< N) 
 
   #if (Merge_On = 1) // Ia
   merge{
   #else
   union{
   #end // of "#if (Merge_On = 1)"
       #if (Rc1 != 0 ) // else no base spheres  and  no base ring cylinders
       sphere{< Rc1, 0, 0>,Rw} // base corner
       cylinder {< 0, 0, -HLe1>, < 0, 0,  HLe1>,Rw translate<Re1,0,0> rotate<0,-180/N,0 > } // lower sides hor.ring
       #end
     #if (H > 0)
       #if (Rc2 != 0 ) // else no top sphere and no base ring cylinders
       sphere{< Rc2, H, 0>,Rw} // top  corner
       cylinder {< 0, 0, -HLe2>, < 0, 0,  HLe2>,Rw translate<Re2,H,0> rotate<0,-180/N,0 > } // upper sides hor.ring
       #end 
       cylinder {< Rc1, 0, 0>,< Rc2, H, 0>,Rw }    // side edge base to top 
     #end 
       rotate<0,Nr*360/N,0>
   } // end of union or merge  // Ia
 
 #local Nr = Nr + 1;
 #end  // -------------------------- end of wire frame

// ---------------------------------------------------
#if (Fill_On = 1)  // ------------------------ filling

#if (Merge_On = 1)
 merge{ //II
#else
 union{ //II
#end // of "#if (Merge_On = 1)"

 #local Nr = 0; 
 #while (Nr< N) 

   #if (Merge_On = 1)
   merge{ //III
   #else
   union{ //III
   #end // of "#if (Merge_On = 1)"

 intersection{    
   prism{ -Rw ,H+Rw  , 4   // prism y
           < Rw_sin+D    ,    0>,
           < Re1+Rw_sin+D, HLe1>,
           < Re1+Rw_sin+D,-HLe1>,
           < Rw_sin+D    ,    0>
        } // end of prism in y --------
   prism{ -Rc1-D , Rc1+D, 5 // prism z   
           < 0            ,   Rw_cos>,   
           < Re1+Rw_sin   ,   Rw_cos>,
           < Re2+Rw_sin   , H+Rw_cos>,
           < 0            , H+Rw_cos>, 
           < 0            ,   Rw_cos>  
          rotate<-90,0,0> scale<1,1,-1>  
        } // end of prism ---------------
   rotate<0,-180/N,0>
   rotate<0,-Nr*360/N,0>
 }// end of intersection
 
 // inner between fillers, center to the corners: 
 prism{ -Rw_sin-D , Rw_sin+D, 5 // prism z 
                   < -D           ,Rw_cos -D>,
                   <  Rc1         ,Rw_cos -D>,
                   <  Rc2         ,Rw_cos +H +D>,
                   <-D            ,Rw_cos +H +D>,
                   <-D            ,Rw_cos -D>
          rotate<-90,0,0> scale<1,1,-1> rotate<0,360/N/2,0>  
          rotate<0,-360/N/2 ,0>
  rotate<0,-(Nr)*360/N,0>        
          } // end of prism -------------------------------------

  }//  end union/merge III

 #local Nr = Nr + 1;
 #end  

 // fillers cover and top ---------------------------------------
 intersection{  // lower cover  
  #local Nr = 0; 
  #while (Nr< N) 
        box{  <-1.5*Rc1,-Rw-D*Nr,-Rc1-Rw_sin>, <Re1 ,Rw_cos+D+D*Nr, Rc1+Rw_sin> 
        rotate<0,(Nr+0.5)*360/N,0>}
 #local Nr = Nr + 1;
 #end  
 }// end of intersection

 intersection{   // upper cover 
  #local Nr = 0; 
  #while (Nr< N) 
      box{<-1.5*Rc1,H-Rw,-Rc1>,<Re2,H+Rw-D*Nr, Rc1> 
        rotate<0,(Nr+0.5)*360/N,0>}
 #local Nr = Nr + 1;
 #end  
 }// end of intersection
}// end of union or merge // II
#end // of (Fill_On = 1) ---------- end of filling

translate<0,Rw,0>
 
#if (Flip = 1)
scale<1,-1,1> translate<0,H+2*Rw,0> 
#end 
 

Reorient_Trans(< 0,1,0>, AB ) // needs "transforms.inc":
translate A 
                            
} // end of global union
#end// ----------------------------------------------<<<  end of macro Round_Pyramid_N_out()
// -----------------------------------------------------------------------------------------
 


// -----------------------------------------------------------------<<< macro Rounded_Tube() 
#macro Rounded_Tube ( Tube_R_out__, // Tube radius outside
                      Tube_R_in__,  // Tube inner radius 
                      Border_R__,   // border radius 
                      Tube_Y__,     // tube high
                      Merge_, // 0 = union, 1 = merge
                    ) //----------------------------------------------------  
// ------------------------------------------------------------------------- 
#local D = 0.000001;     /// *Border_R__                  

#local Tube_R_out_ =  Tube_R_out__; 
#local Tube_R_in_ = Tube_R_in__; 
#local Border_R_ = Border_R__; 
#local Tube_Y_ = Tube_Y__;

// inner radius bigger than outer radius - exchange them
#if (Tube_R_in_ > Tube_R_out_)
    #local Temporary_ = Tube_R_out_; 
    #local Tube_R_out_ = Tube_R_in_;
    #local Tube_R_in_ = Temporary_;
    #warning concat("\nTube inner radius > tube inner radius! \n Radii exchanged!")
#end
// too big border radii: reduce border radii.
#if ( Border_R_ >= min((Tube_Y_/2),(Tube_R_out_-Tube_R_in_) )) 
    #local Border_R_ = min((Tube_Y_/2),(Tube_R_out_-Tube_R_in_))-D; 
    #warning concat("\nTube height < 2*border radius! or 
 Difference of outer radius - inner radius <  2*border radius! 
 Border radius reduced!")
#end  

#if (Merge_ = 1 )  
merge{ 
#else
union { 
#end
   difference{ // outline
    cylinder{<0,+Border_R_,0>,<0,Tube_Y_-Border_R_,0>,Tube_R_out_ } 
    cylinder{<0,-D        ,0>,<0,Tube_Y_+D,        0>,Tube_R_in_  } 
   } // end of difference
   difference{ // tween
    cylinder{<0,0, 0>,<0,Tube_Y_  ,0>,Tube_R_out_-Border_R_} 
    cylinder{<0,-D,0>,<0,Tube_Y_+D,0>,Tube_R_in_ +Border_R_} 
   } // end of difference

   torus{ Tube_R_out_-Border_R_, Border_R_ translate<0,Tube_Y_-Border_R_-D,0>}  
   torus{ Tube_R_out_-Border_R_, Border_R_ translate<0,0+Border_R_+D,0>}  
   torus{ Tube_R_in_ +Border_R_, Border_R_ translate<0,Tube_Y_-Border_R_-D,0>}  
   torus{ Tube_R_in_ +Border_R_, Border_R_ translate<0,0+Border_R_+D,0>}  
} // end of merge or union 
#end// of macro --------------------------------------------<<<  end of macro Rounded_Tube()
// -----------------------------------------------------------------------------------------


// --------------------------------------------------------------<<< macro Rounded_Tube_AB()
#macro Rounded_Tube_AB ( Point_A, Point_B, Radius_out, Radius_in, Border_Radius, Merge) 
// --------------------------------------------------------------  
#local A = Point_A; 
#local B = Point_B; 
#local AB = B-A;  
#local H  = vlength( AB); // pyramid height;

object{ Rounded_Tube ( Radius_out, Radius_in, Border_Radius, H, Merge ) 
        Reorient_Trans(< 0,1,0>, AB ) // needs "transforms.inc":
        translate A 
      }  //
#end // ------------<<< end of macro Rounded_Tube_AB(N, A, B, R_out, R_in, R_Border, Merge ) 
// -----------------------------------------------------------------------------------------



// ----------------------------------------------------------<<< macro Round_Cylinder_Tube()
#macro Round_Cylinder_Tube ( A, // starting point
                             B, // end point
                             Radius,     // major radius
                             EdgeRadius, // minor radius
                             Filled, // if Filled = 1;  otherwise: open tube
                             UseMerge // use merge for transparent materials
                           ) //--------------------------------------------- 
//-------------------------------------------------------------------------- 
#local D = 0.00001;

#if( Filled = 0 )
difference{
#end

#if( UseMerge = 1 )
   merge {
#else
   union {
#end

  #if( Radius<EdgeRadius ) // degenerated case
     #warning "\nRound_Cylinder() macro called with Radius < EdgeRadius,\nresults may not be as expected\n"
     #local AA = A + vnormalize(B - A)*Radius;
     #local BB = B + vnormalize(A - B)*Radius;

    cylinder {AA, BB, Radius}
    sphere {0, Radius translate AA }
    sphere {0, Radius translate BB }

  #else // non-degenerated case

     #local AA = A + vnormalize(B - A)*EdgeRadius;
     #local BB = B + vnormalize(A - B)*EdgeRadius;

     #if( Filled = 1 )
     cylinder {A, B, Radius - EdgeRadius}
     #end

     cylinder {AA, BB, Radius}
     torus {Radius - EdgeRadius, EdgeRadius translate y*EdgeRadius
            Point_At_Trans(B - A)
            translate A
           }
     torus {Radius - EdgeRadius, EdgeRadius translate y*(vlength(A - B) - EdgeRadius)
            Point_At_Trans(B - A)
            translate A
           }
  #end // end of degenerated by radius or not
  } // end of union or merge

 #if( Filled = 0)  // A+D*(A-B), B+D*(B-A)
 cylinder {A, B, Radius - 2*EdgeRadius}
}// end of difference
#end // of "#if( Filled = 0 )"

#end// of macro --------------------------------------<<< end of macro Round_Cylinder_Tube()
// -----------------------------------------------------------------------------------------

//-------------------------------------------------------------<<< macro Round_Conic_Torus() 
#macro Round_Conic_Torus( C_distance_,// >0, vertical center distance of upper + lower torii
                          R_upper_,  // >0, upper radius up by <0,C_distance,0>
                          R_lower_,  // >0, lower radius on zero !!!
                          Border_R_, // max. = min(R_lower,R_upper)
                          Merge_On
                        ) //-------  looks in y+direction
//------------------------------------------------------------------
#local D = 0.000001; // just a little bit !!!
//------------------------------------------
#local C_distance = C_distance_; 
#local R_upper = R_upper_;
#local R_lower = R_lower_; 
#local Border_R = Border_R_; 
//------------------------------------------
#if (C_distance = 0) #local C_distance = D;
 #warning "\nRound_Conic_Torus() macro called with center distance = 0,\n center distance set to 0.000001 ! \n"
#end
#if (C_distance < 0) #local C_distance = abs(C_distance);
 #warning "\nRound_Conic_Torus() macro called with center distance < 0,\n center distance set to abs(center distance) ! \n"
#end

#if (Border_R < 0 ) #local Border_R = abs(  Border_R );
 #warning "\nRound_Conic_Torus() macro called with border radius < 0,\n border radius set to abs(border radius) ! \n"
#end
#if (Border_R = 0 ) #local Border_R = 0.01;
 #warning "\nRound_Conic_Torus() macro called with border radius = 0,\n border radius set to 0.001 ! \n"
#end
#if (Border_R >  min(R_lower,R_upper) ) #local Border_R = min(R_lower,R_upper)+D;
 #warning "\nRound_Conic_Torus() macro called with border radius > min(lower radius, upper radius),\n border radius set to  min(lower radius, upper radius) + 0.000001 ! \n"
#end


#if (R_upper = 0) #local R_upper =  0.002;
 #warning "\nRound_Conic_Torus() macro called with upper radius = 0,\n upper radius set to 0.002 ! \n"
#end
#if (R_upper < 0) #local R_upper = abs (R_upper);
 #warning "\nRound_Conic_Torus() macro called with upper radius < 0,\n upper radius set to  abs(upper radius) ! \n"
#end
#if (R_lower = 0) #local R_lower = 0.002;
 #warning "\nRound_Conic_Torus() macro called with lower radius = 0,\n lower radius set to 0.002 ! \n"
#end
#if (R_lower < 0) #local R_lower = abs (R_upper);
 #warning "\nRound_Conic_Torus() macro called with lower radius < 0,\n lower radius set to  abs(lower radius) ! \n"
#end
//---------------------------------------------------------------------------------------------------
// exchange upper and lower for construction if necessary (later they will changed back!)
#if ( (R_upper >= R_lower) & (C_distance>0)) #local Ro = R_upper; #local Ru = R_lower; #local Flag=0;
#else                                        #local Ro = R_lower; #local Ru = R_upper; #local Flag=1;
#end
//------------------------------------------------------
#local Side_Len   = sqrt(pow(C_distance,2) - pow( (Ro-Ru),2) );
#local Side_Angle = degrees( atan( (Ro-Ru)/ Side_Len) );


#if ( Merge_On = 1 ) union{
#else                merge{   #end
 // +z /-z border cylinder pending
 cylinder{< 0,0,-D>,<0,Side_Len,0>,Border_R translate<0,0,Ru> rotate< Side_Angle,0,0>}
 cylinder{< 0,0,-D>,<0,Side_Len,0>,Border_R translate<0,0,Ru> rotate< Side_Angle,0,0> scale<1,1,-1>}

intersection{ // +z box pending
 torus{ Ru, Border_R rotate<0,0,90> translate<0,0,0>}
 box{< -Border_R-D,0,-Ru-Border_R-D>,<Border_R+D,Ru+Border_R+D,Ru+Border_R+D> rotate< Side_Angle,0,0> inverse}
 box{< -Border_R-D,0,-Ru-Border_R-D>,<Border_R+D,Ru+Border_R+D,Ru+Border_R+D> rotate<-Side_Angle,0,0> inverse}
}// end inters

intersection{ // +z box pending
//union{
 torus{ Ro, Border_R rotate<0,0,90> translate<0,C_distance,0>}
 intersection{
 box{< -Border_R-D,-Ro-Border_R-D,-Ro-Border_R-D>,<Border_R+D,0,Ro+Border_R+D> rotate< Side_Angle,0,0> }
 box{< -Border_R-D,-Ro-Border_R-D,-Ro-Border_R-D>,<Border_R+D,0,Ro+Border_R+D> rotate<-Side_Angle,0,0> } 
 translate<0,C_distance,0> inverse}
}// end inters

//#end // of "#if ( Border_R > 0 )"
#if (Flag = 1) scale<1,-1,1> translate<0,C_distance,0> #end
rotate<0,90,0> // turn it in the xy-plane
 } //end of union

#end// of macro ---------------------------------------<<<  end of macro Round_Conic_Torus()
// -----------------------------------------------------------------------------------------

// ------------------------------------------------------------<<< macro Round_Conic_Prism()
#macro Round_Conic_Prism( C_distance_,  // >0, vertical center distance of the upper and lower torii
                          R_upper_,  // >0, upper radius up by <0,C_distance,0>
                          R_lower_,  // >0, lower radius on zero !!!
                          Len_,  // length in z-
                          Border_R_, //max. = min(R_lower,R_upper) 0 = without rounded borders
                          Merge_On
                        ) //-------  looks in y+direction
// -----------------------------------------------------------------------------------------
#local D = 0.000001; // just a little bit !!!
// ------------------------------------------
#local C_distance = C_distance_; 
#local R_upper = R_upper_;
#local R_lower = R_lower_; 
#local Len = Len_; 
#local Border_R = Border_R_; 
// ------------------------------------------

#if (C_distance = 0) #local C_distance =  0.001;
 #warning "\nRound_Conic_Prism() macro called with center distance = 0,\n center distance set to 0.000001 ! \n"
#end
#if (C_distance < 0) #local C_distance = abs(C_distance);
 #warning "\nRound_Conic_Prism() macro called with center distance < 0,\n center distance set to abs(center distance) ! \n"
#end

#if (Border_R < 0 ) #local Border_R = abs(  Border_R );
 #warning "\nRound_Conic_Prism() macro called with border radius < 0,\n border radius set to abs(border radius) ! \n"
#end
#if (Border_R = 0 ) // #local Border_R = 0.01;
// #warning "\nRound_Conic_Torus() macro called with border radius = 0,\n border radius set to 0.001 ! \n"
#end
#if (Border_R >  min(R_lower,R_upper) ) #local Border_R = min(R_lower,R_upper)+D;
 #warning "\nRound_Conic_Prism() macro called with border radius > min(lower radius, upper radius),\n border radius set to  min(lower radius, upper radius) + 0.000001 ! \n"
#end


#if (R_upper = 0) #local R_upper =  0.0005;
 #warning "\nRound_Conic_Prism() macro called with upper radius = 0,\n upper radius set to 0.0005 ! \n"
#end
#if (R_upper < 0) #local R_upper = abs (R_upper);
 #warning "\nRound_Conic_Prism() macro called with upper radius < 0,\n upper radius set to  abs(upper radius) ! \n"
#end
#if (R_lower = 0) #local R_lower = 0.0001;
 #warning "\nRound_Conic_Prism() macro called with lower radius = 0,\n lower radius set to 0.0001 ! \n"
#end
#if (R_lower < 0) #local R_lower = abs (R_upper);
 #warning "\nRound_Conic_Prism() macro called with lower radius < 0,\n lower radius set to  abs(lower radius) ! \n"
#end


#if (Len < 0)   #local Len = abs(Len);
 #warning "\nRound_Conic_Prism() macro called with length in z+ = 0,\n length set to abs(length in z-) ! \n"
#end

#if (Len < 2*Border_R+D)   #local Len = 2*Border_R+D;
 #warning "\nRound_Conic_Prism() macro called with length <= 2*border radius,\n length set to 2*border radius+0.000001 ! \n"
#end

#if (Len = 0)   #local Len = 2*Border_R+D;
 #warning "\nRound_Conic_Prism() macro called with length in z- = 0,\n length set to 2*border radius+0.000001 ! \n"
#end

// ---------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------
// exchange upper and lower for construction if necessary (later they will changed back!)
#if ( (R_upper >= R_lower) & (C_distance>0)) #local Ro = R_upper; #local Ru = R_lower; #local Flag=0;
#else                                        #local Ro = R_lower; #local Ru = R_upper; #local Flag=1;
#end
// ----------------------------------------------------------------------------------------
#local Side_Len   = sqrt(abs( pow(C_distance,2) - pow( ( abs(Ro-Ru)),2) ) );
#local Side_Angle = degrees( atan( abs((Ro-Ru))/ Side_Len) );

//----------------------------------------------------------------------------------------
#if ( Merge_On = 1 ) union{
#else                merge{   #end
// around x-axis - turned later in z- direction
intersection{ // +z box pending
 box{< Border_R,0,-1.1*Ro-D>,<Len-Border_R,Side_Len,-D> translate<0,0,Ru> rotate< Side_Angle,0,0>}
 box{<-D,-Ro,-D>,<Len+D,C_distance+Ro,Ro+D>}
            } //end of intersection
intersection{ // +z box pending
 box{< Border_R,0,-1.1*Ro-D>,<Len-Border_R,Side_Len,-2*D> translate<0,0,Ru> rotate< Side_Angle,0,0>}
 box{<-D,-Ro,-D>,<Len+D,C_distance+Ro,Ro+D>}
 scale<1,1,-1>
            } //end of intersection
cylinder{ <Border_R,0,0>,<Len-Border_R,0,0>,Ro translate<0,C_distance,0>}
cylinder{ <Border_R,0,0>,<Len-Border_R,0,0>,Ru translate<0,0,0>}

#if ( Border_R > 0 )
// inner boxes full lenght
intersection{ // +z box pending
 box{< 0,0,-1.1*Ro-D>,<Len,Side_Len,D> translate<0,0,Ru-Border_R> rotate< Side_Angle,0,0>}
 box{<-D,-Ro,0>,<Len+D,C_distance+Ro,Ro+D>}
 } //end of intersection
intersection{ // +z box pending
 box{< 0,0,-1.1*Ro-D>,<Len,Side_Len,0> translate<0,0,Ru-Border_R> rotate< Side_Angle,0,0>}
 box{<-D,-Ro,-D>,<Len+D,C_distance+Ro,Ro+D>}
 scale<1,1,-1>
 } //end of intersection

// +z /-z border cylinder pending
 cylinder{< 0,0,-D>,<0,Side_Len,0>,Border_R translate<Border_R,0,Ru-Border_R> rotate< Side_Angle,0,0>}
 cylinder{< 0,0,-D>,<0,Side_Len,0>,Border_R translate<Border_R,0,Ru-Border_R> rotate< Side_Angle,0,0> scale<1,1,-1>}
 cylinder{< 0,0,-D>,<0,Side_Len,0>,Border_R translate<Len-Border_R,0,Ru-Border_R> rotate< Side_Angle,0,0>}
 cylinder{< 0,0,-D>,<0,Side_Len,0>,Border_R translate<Len-Border_R,0,Ru-Border_R> rotate< Side_Angle,0,0> scale<1,1,-1>}


 torus{ Ro-Border_R, Border_R rotate<0,0,90> translate<Border_R,C_distance,0>}
 torus{ Ru-Border_R, Border_R rotate<0,0,90> translate<Border_R,0,0>}
 torus{ Ro-Border_R, Border_R rotate<0,0,90> translate<Len-Border_R,C_distance,0>}
 torus{ Ru-Border_R, Border_R rotate<0,0,90> translate<Len-Border_R,0,0>}
 cylinder{ <0,0,0>,<Len,0,0>,Ro-Border_R translate<0,C_distance,0>}
 cylinder{ <0,0,0>,<Len,0,0>,Ru-Border_R translate<0,0,0>}
#end // of "#if ( Border_R > 0 )"

#if (Flag = 1) scale<1,-1,1> translate<0,C_distance,0> #end
rotate<0,90,0>
 } //end of union

#end// of macro ---------------------------------------<<<  end of macro Round_Conic_Prism()
// -----------------------------------------------------------------------------------------

// ----------------------------------------------<<< macro Half_Hollowed_Rounded_Cylinder1()
#macro Half_Hollowed_Rounded_Cylinder1( 
                               Len_total_, // total_Lenght from end to end
                               R_out_,     // outer radius 
                               R_Border_,  // border Radius < outer radius !!!
                               Border_Scale_y_, // ( >=0 ) 0 = no rounded borders!
                               Merge_On , // 0 = union, 1 = merge !
                             ) //-----------------------------------------------
//------------------------------------------------------------------------------
#local D = 0.000001; // just a little bit !!!
//------------------------------------------
#local Len_total = Len_total_; 
#local R_out = R_out_; 
#local R_Border = R_Border_; 
#local Border_Scale_y = Border_Scale_y_; 

#if ( R_out < R_Border )
 #warning "\nHalf_Hollowed_Rounded_Cylinder1() macro called with outer radius < border radius,\n radii exchanged ! \n"
 #local Safe = R_Border;
 #local R_Border = R_out; 
 #local R_out = Safe; 
#end 
#if ( R_out - R_Border <= 0 + D)
 #warning "\nHalf_Hollowed_Rounded_Cylinder1() macro called with outer radius ~ border radius,\n border radius set to 0.00002 ! \n"
 #local R_Border = 2*D
#end 
#local R_in = R_out - R_Border ;

#if ( Len_total < 2*R_out )
 #warning "\nHalf_Hollowed_Rounded_Cylinder1() macro called with total length < 2*outer radius,\n length increased to 2*outer radius. Results may not be as expected !\n"
 #local Len_total = 2*R_out +D; 
#end 

#local Len = Len_total-2*R_out; // length of linear kernel  
#local R_Border = (R_out-R_in)/2 ; // Radius of the upper borders 
 
// ----------------------------------------------------------------- 
#if (Merge_On = 0)
 union{ 
#else 
 merge{ 
#end   
       // hollow half rounded cylinder   
       difference{
                   #if (Merge_On = 0)
                   union{ 
                   #else 
                   merge{ 
                   #end   
                     cylinder{ <-Len/2,0,0>,<Len/2,0,0>,R_out}
                     sphere{   <-Len/2,0,0>,R_out}
                     sphere{   < Len/2,0,0>,R_out}
                   }// end of union or merge
                  
                   cylinder {<-Len/2,0,0>,<Len/2,0,0>,R_in}
                   sphere {  <-Len/2,0,0>,R_in}
                   sphere {  < Len/2,0,0>,R_in}
                  
                // cut off the upper part
                   box{ <-Len-R_out-D,       D,-R_out-D>,
                        < Len+R_out+D, R_out+D, R_out+D>} 
                 }
       
   #if( Border_Scale_y > 0 )
     // rounded borders
      #if (Merge_On = 0)
      union{ 
      #else 
      merge{ 
      #end 
       // side cylinders  
       difference { 
       cylinder{ <-Len/2-D,0, R_in + R_Border >,
                 < Len/2+D,0, R_in + R_Border >,
                 R_Border }
       box{ <-Len-R_out-D,-R_Border-D,-R_out-D> 
            < Len+R_out+D,         -D, R_out+D>} 
       }
       difference { 
       cylinder{ <-Len/2-D,0,-R_in - R_Border >,
                 < Len/2+D,0,-R_in - R_Border >,
                 R_Border }
       box{ <-Len-R_out-D,-R_Border-D,-R_out-D> 
            < Len+R_out+D,         -D, R_out+D>} 
       } 
       // ending with half torii
       difference {  
           union { // 
             torus{ R_in+R_Border,R_Border sturm
                    translate <-Len/2,0,0>
                  }
             torus{ R_in+R_Border,R_Border sturm
                    translate < Len/2,0,0>
                  }
           } // end of inner union  
        
           cylinder {<-Len/2+D,0,0>,<Len/2-D,0,0>,R_out+D}
                  
           box{ <-Len-R_out-D,-R_Border-D,-R_out-D> 
                < Len+R_out+D,         -D, R_out+D>} 

        } // end of rounded borders base shape 
      scale <1,Border_Scale_y,1> 
     }// end borders
   #end// of "#if( Border_Scale_y > 0 )" 
 } // end of union or merge
#end // of macro  -----------------------<<<  end of macro Half_Hollowed_Rounded_Cylinder1()
// -----------------------------------------------------------------------------------------


// ----------------------------------------------<<< macro Half_Hollowed_Rounded_Cylinder2()
#macro Half_Hollowed_Rounded_Cylinder2( 
                               Len_total_, // total_Lenght from end to end
                               R_out_, // Radius_out, outer radius 
                               R_End_, // < R_out !  > 2*R_Border
                               R_Border_,  // border radius
                               Border_Scale_y_ // ( >0 ), 0 = no rounded borders
                               Merge_On, // 0 = union, 1 = merge !
                             ) //----------------------------------------------- 
// -----------------------------------------------------------------------------------------
#local D = 0.000001; // just a little bit !!!
// ------------------------------------------
#local Len_total = Len_total_; 
#local R_out = R_out_; 
#local R_End = R_End_;
#local R_Border = R_Border_; 
#local Border_Scale_y = Border_Scale_y_; 

#if ( R_End > Len_total/2 )
 #warning "\nHalf_Rounded_Hollowed_Cylinder2() macro called with end radius < total lenght/2,\nresults may not be as expected\n"
 #local R_End = Len_total/2-2*D; 
#end 
#if ( R_out < R_Border )
 #warning "\nHalf_Hollowed_Rounded_Cylinder1() macro called with outer radius < border radius,\n radii exchanged ! \n"
 #local Safe = R_Border;
 #local R_Border = R_out; 
 #local R_out = Safe; 
#end 

#local Len = Len_total-2*R_End; // length of linear kernel  
#local R_in = (R_out-2*R_Border) ; // Radius of the inner round cylindere 
#local D_Corner = R_out - R_End; 
// ------------------------------------------------------------------------------
#local Corner_Border = 
intersection{  
               torus{ R_End-R_Border, R_Border  sturm
                    }
               box  {<0,-D,0>,<R_End+D,R_Border+D,R_End+D> // +x +z corner 
                    }  
            } // end intersection
// ------------------------------------------------------------------------------
#if (Merge_On = 0)
 union{ 
#else 
 merge{ 
#end   
       //  hollow rounded cylinder   
       difference{

           object{ //Round_Cylinder(point A, point B, Radius, EdgeRadius, UseMerge)
                   Round_Cylinder(<-Len/2,0,0>, <Len/2,0,0>, R_out, R_End, Merge_On)  
                 } // --------------------------------------------------------------
           object{ //Round_Cylinder(point A, point B, Radius, EdgeRadius, UseMerge)
                   Round_Cylinder(<-Len/2+2*R_Border,0,0>, <Len/2-2*R_Border,0,0>, 
                                                  R_in,  R_End-2*R_Border, Merge_On)  
                   translate<0,D,0>
                 } // --------------------------------------------------------------

                   // cut off the upper part
              box{ <-Len-R_out-D,       D,-R_out-D>,
                   < Len+R_out+D, R_out+D, R_out+D>} 
                 }
       
   #if( Border_Scale_y > 0 )
   // rounded borders
   difference { // with 1/4 torii in the corners 
      #if (Merge_On = 0)
      union{                                              

      #else 
      merge{ 
      #end   
        
       cylinder{ <-Len/2-D +R_End,0, R_in + R_Border >,
                 < Len/2+D -R_End,0, R_in + R_Border >,
                 R_Border }
       cylinder{ <-Len/2-D +R_End,0,-R_in - R_Border >,
                 < Len/2+D -R_End,0,-R_in - R_Border >,
                 R_Border }

       object{  Corner_Border translate<Len/2-R_End,0,D_Corner>  } 
       object{  Corner_Border translate<Len/2-R_End,0,D_Corner> scale<-1,1, 1> } 
       object{  Corner_Border translate<Len/2-R_End,0,D_Corner> scale<-1,1,-1> } 
       object{  Corner_Border translate<Len/2-R_End,0,D_Corner> scale< 1,1,-1> } 
    
       cylinder{ <0,0,  R_out-R_End +D >,
                 <0,0,-(R_out-R_End)-D >,
                 R_Border 
                 translate <-Len/2+R_Border,0,0> }
       cylinder{ <0,0,  R_out-R_End +D >,
                 <0,0,-(R_out-R_End)-D >,
                 R_Border 
                 translate < Len/2-R_Border,0,0> }
     } // end inner union
                  
     box{ <-Len-R_out-D,-R_Border-D,-R_out-D> 
          < Len+R_out+D,         -D, R_out+D>} 
     
      
     scale <1,Border_Scale_y,1> 
   }// end difference borders
   #end// of "#if( Border_Scale_y > 0 )"

 } // end of union or merge
#end // of macro  ----------------------<<<  end of macro Half_Hollowed_Rounded_Cylinder2()
// -----------------------------------------------------------------------------------------

 
                                      
// --------------------------------------------------------
// --------------------------------------------------------

 #version Shapes3_Inc_Temp;
 #end
// --------------------------------------------------------
//--------------------------------------------------------- end of include file shapes3.inc
 