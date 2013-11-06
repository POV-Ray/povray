// Insert menu illustration scene
// Author Friedrich A. Lohmueller, Feb-2013
#version 3.7;

#declare In_Path  = "10 - Ready made scenes/" 

 // #declare Typ = 17; // for testing

// Basic Scenes 
#switch (Typ)  //-----------------------------------------------------------------------------
#case(1)   #declare Txt_Path="10 - Basic scene 01 - Checkered plane.txt"  #break 
#case(2)   #declare Txt_Path="20 - Basic scene 02 - Desert with blue sky.txt"  #break
#case(3)   #declare Txt_Path="30 - Basic scene 03 - White sands with blue sky.txt"  #break
#case(4)   #declare Txt_Path="40 - Basic scene 04 - Grass with partly cloudy sky.txt"  #break
#case(5)   #declare Txt_Path="50 - Basic scene 05 - Grass with small clouds in sky.txt"  #break
#case(6)   #declare Txt_Path="60 - Basic scene 06 - Sea with partly cloudy sky.txt"  #break
#case(7)   #declare Txt_Path="70 - Basic scene 07 - Sea with small clouds in sky.txt"  #break
#case(8)   #declare Txt_Path="80 - Basic scene 08 - Sea with blue sky.txt"  #break
#case(9)   #declare Txt_Path="90 - Basic scene 09 - Night moon partly cloudy sky.txt"  #break
#case(10)  #declare Txt_Path="A0 - Basic scene 10 - Night moon partly cloudy sky.txt"  #break
#case(11)  #declare Txt_Path="B0 - Basic scene 11 - pure white background.txt"  #break

#case(12)  #declare Txt_Path="M0 - Basic scene M0 - DarkBlueSky + Axes.txt"  #break
#case(13)  #declare Txt_Path="M1 - Basic scene M1 - Squared plane XY 2D.txt"  #break
#case(14)  #declare Txt_Path="M2 - Basic scene M2 - Squared plane XZ 3D far.txt"  #break
#case(15)  #declare Txt_Path="M3 - Basic scene M3 - Squared plane XZ 3D dark.txt"  #break
#case(16)  #declare Txt_Path="M4 - Basic scene M4 - Squared plane XZ 3D righthanded y up.txt"  #break
#case(17)  #declare Txt_Path="M5 - Basic scene M5 - Squared plane XY 3D righthanded z up.txt"  #break

#case(18)  #declare Txt_Path="N0 - Orthographic isometric scene.txt"  #break
#case(19)  #declare Txt_Path="N1 - Orthographic scene.txt"  #break
#end
// -------------------------------------------------------------------------------------------
// --------
#if (Typ>=1 & Typ<=20) #include concat(In_Path,Txt_Path) #end
// --------
 //---------------------------------------------------------- that's it :-)