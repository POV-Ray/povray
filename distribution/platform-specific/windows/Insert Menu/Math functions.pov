// Insert menu illustration scene
// Author Friedrich A. Lohmueller, Feb-2013
#version 3.7;

#declare In_Path  = "80 - Math functions/" 

  //#declare Typ = 1; // for testing

// Basic Scenes 
#switch (Typ)  //-----------------------------------------------------------------------------
#case(1)   #declare Txt_Path="10 - parabola.txt"  #break 
#case(2)   #declare Txt_Path="20 - hyperbola.txt"  #break
#case(3)   #declare Txt_Path="50 - function 0 derived.txt"  #break
#case(4)   #declare Txt_Path="51 - function 1 derived.txt"  #break
#case(5)   #declare Txt_Path="52 - function 2 derived.txt"  #break
#case(6)   #declare Txt_Path="53 - function 3 derived.txt"  #break
#case(7)   #declare Txt_Path="80 - sine function.txt"  #break
#case(8)   #declare Txt_Path="81 - sine function superpos.txt"  #break
#case(9)   #declare Txt_Path="90 - exp function ln function.txt"  #break
#case(10)  #declare Txt_Path="91 - exp function 2.txt"  #break
#case(11)  #declare Txt_Path="92 - exp function 3.txt"  #break
#end
// -------------------------------------------------------------------------------------------
// --------
#if (Typ>=1 & Typ<=11) #include concat(In_Path,Txt_Path) #end
// --------
 //---------------------------------------------------------- that's it :-)