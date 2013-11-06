// Insert menu illustration scene "Colors.pov"
// Author Friedrich A. Lohmueller, May 2010
// ----------- submenu 'Colors' -------------- 

// -w96 -h64 +a0.1 +am2 -j +r3

#version 3.7;
global_settings{ assumed_gamma 1.0 } 

#default{ finish{ ambient 0.1 diffuse 0.9 }} 

#include "colors.inc"
#include "textures.inc"
// --------------------------- 
#declare In_Path  = "D0 - Colors/" 


 //  #declare Typ = 35; 

//#declare Typ=Switch-6000;
#switch (Typ)  //----------------------------------------------------------
// - Colors in textures/
#case(10)  #declare Txt_Path="10 - color White.txt" #break
#case(11)  #declare Txt_Path="11 - color Gray85.txt" #break
#case(12)  #declare Txt_Path="12 - color Gray75.txt" #break

#case(14)  #declare Txt_Path="14 - color Gray50.txt" #break
#case(16)  #declare Txt_Path="16 - color Gray25.txt" #break
#case(17)  #declare Txt_Path="17 - color Gray10.txt" #break
#case(18)  #declare Txt_Path="18 - color Black.txt" #break
#case(19)  #declare Txt_Path="19 - very light brown.txt" #break
#case(20)  #declare Txt_Path="20 - light brown.txt" #break
#case(21)  #declare Txt_Path="21 - very dark brown.txt" #break
#case(22)  #declare Txt_Path="22 - dark brown.txt" #break

#case(34)  #declare Txt_Path="34 - very pale red violet.txt" #break
#case(35)  #declare Txt_Path="35 - pale red violet.txt" #break
#case(36)  #declare Txt_Path="36 - red violet.txt" #break
#case(37)  #declare Txt_Path="37 - red wine.txt" #break
#case(38)  #declare Txt_Path="38 - very dark red.txt" #break
#case(39)  #declare Txt_Path="39 - dark red.txt" #break

#case(40)  #declare Txt_Path="40 - color Red.txt" #break
#case(41)  #declare Txt_Path="41 - very light red.txt" #break
#case(42)  #declare Txt_Path="42 - red orange.txt" #break

#case(44)  #declare Txt_Path="44 - light orange.txt" #break
#case(45)  #declare Txt_Path="45 - color Orange.txt" #break

#case(50)  #declare Txt_Path="50 - deep yellow.txt" #break
#case(51)  #declare Txt_Path="51 - color Yellow.txt" #break
#case(52)  #declare Txt_Path="52 - color Green.txt" #break
#case(53)  #declare Txt_Path="52 - yellow lemon.txt" #break
#case(54)  #declare Txt_Path="53 - color YellowGreen.txt" #break
#case(55)  #declare Txt_Path="54 - yellowish green.txt" #break
#case(56)  #declare Txt_Path="55 - dark yellow green.txt" #break

#case(60)  #declare Txt_Path="60 - dark green.txt" #break
#case(61)  #declare Txt_Path="61 - dark olive green.txt" #break
#case(62)  #declare Txt_Path="62 - dark blue green.txt" #break
#case(63)  #declare Txt_Path="63 - blueish green.txt" #break
#case(64)  #declare Txt_Path="64 - blue green.txt" #break

#case(70)  #declare Txt_Path="70 - light cyan.txt" #break
#case(71)  #declare Txt_Path="71 - cyan.txt" #break
#case(72)  #declare Txt_Path="72 - dark cyan.txt" #break
#case(73)  #declare Txt_Path="73 - greenish blue.txt" #break
#case(74)  #declare Txt_Path="74 - light greenish blue.txt" #break

#case(80)  #declare Txt_Path="80 - dark blue.txt" #break
#case(81)  #declare Txt_Path="81 - color Blue.txt" #break
#case(82)  #declare Txt_Path="82 - bright blue.txt" #break
#case(83)  #declare Txt_Path="83 - light blue.txt" #break
#case(84)  #declare Txt_Path="84 - very light blue.txt" #break
#case(85)  #declare Txt_Path="85 - pale blue.txt" #break
#case(86)  #declare Txt_Path="86 - light violet.txt" #break
#case(87)  #declare Txt_Path="87 - very light_violet.txt" #break
#case(88)  #declare Txt_Path="88 - pale violet.txt" #break
#case(89)  #declare Txt_Path="89 - violet.txt" #break

#end // end of '#switch (Typ)'//------------------------------------------
// ----------------------------------------------------------------------- 


//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
background { color rgb<1,1,1>*0.5} 
//-------------- 
camera{ angle 45 location <0,0,-4.25> look_at<0,0,0> right x*image_width/image_height}
//-------------- 
light_source{< 2500, 2500,-2500> color rgb<1,1,1>*1}     // sun light
//-------------- 
union{
sphere{ <-0.7,0,0>,1}
cylinder{<-0.7,0,0>,<0.7,0,0>,1}
sphere{ <0.7,0,0>,1}
scale <0.9,1,1> 
        #include concat(In_Path,Txt_Path) // 

} //------------------

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
