#version 3.7;
global_settings{assumed_gamma 1.0}

#include "colors.inc"

background { White }
camera { 
    orthographic  
    right x
    up y*image_height/image_width
    direction z 
    location -100*z-0.07*x
}
#local HFactor=5; 

#declare Control_Point_Pigment=pigment{color Red};
#declare Control_Point_Radius=0.05*2;

#declare Spline_Pigment=pigment{color Black};
#declare Spline_Radius=0.04*2;
#declare Spline_Accuracy=0.1;

#declare Position = -1/2;

#macro Draw_Spline(Spline)
    union{
			  cylinder { <0,0,0>,<16,0,0>,Spline_Radius/2 pigment { color  Grey}}
        #local Counter = 0;
        #while (Counter < dimension_size(Spline))
            #local Center = Spline[Counter][1];
            sphere{ 
                <Center.x,Center.y,0> 
                Control_Point_Radius 
                pigment{ Control_Point_Pigment }
            }
            #if (Counter>0)
            cylinder{ 
                <Spline[Counter-1][1].x,Spline[Counter-1][1].y,0> 
                <Center.x,Center.y,0> 
                Spline_Radius/2
                pigment{ color Green }
            }
            #local Start= Spline[Counter-1][0];
            #local End= Spline[Counter][0];
            #local C = 0;
            #while (Start+C <= End)
                #local New_Center = Spline(Start + C);
                    sphere{ 
                        <New_Center.x,New_Center.y,0> 
                        Spline_Radius 
                        pigment{ Spline_Pigment }
                    }
                #local C=C+Spline_Accuracy;
            #end
            #end
            #local Counter = Counter + 1;
        #end
        scale 1/18
        translate -x/2-y/40
        #declare Position = Position + 1;
    }
#end


#default{finish{ambient 1 diffuse 0}}

Draw_Spline(spline{akima_spline
    -1 <0,0*HFactor> 
	1 <1/2,1*HFactor>
    1.4 <3,1*HFactor> 
    3 <2,-0.5*HFactor> 
    3.2 <3,-.5*HFactor> 
    4 <4,0.2*HFactor> 
    4.4 <6,0.2*HFactor> 
    4.8 <6,.7*HFactor> 
    5.2 <4,1*HFactor> 
    6 <7,1*HFactor> 
    10 <7,-0.5*HFactor> 
	10.2 <7.2,-0.5*HFactor>
	12 <7.2,1*HFactor>
	12.2 <7.4,1*HFactor>
	14 <7.4,0.5*HFactor>
	14.2 <7.6,0.5*HFactor>
	16 <7.6,-1*HFactor>
	16.2 <7.8,-1*HFactor>
	18 <7.8,0.5*HFactor>
	18.2 <8.0,0.5*HFactor>
	18.6 <8.0,1*HFactor>
	18.8 <8.2,1*HFactor>
	22 <8.2,0*HFactor>
	22.2 <8.4,0*HFactor>
	24 <8.4,1*HFactor>
	24.2 <8.6,1*HFactor>
	26 <8.6,0*HFactor>
	26.2 <8.8,0*HFactor>
	28 <8.8,1*HFactor>
	28.2 <9,1*HFactor>
	30 <9,0*HFactor>
    31 <10,-1*HFactor>
    32 <10,0*HFactor>
    32.3 <10.5,0*HFactor>
    32.6 <11.5,0*HFactor>
	35 <11,1*HFactor>
	35.4 <12,1*HFactor>
	37 <12,0*HFactor>
	39 <12.25,-1*HFactor>
	41 <12.5,0*HFactor>
	43 <12.75,1*HFactor>
	45 <13,0*HFactor>
	47 <13.25,-1*HFactor>
	49 <13.5,0*HFactor>
	49.5 <14,0*HFactor>
	50 <15,0*HFactor>
	53 <15,1*HFactor>
	53.2 <14,1*HFactor>
	58 <14,0*HFactor>
	58.4 <15,0*HFactor>
	58.6 <16,0*HFactor>
})
