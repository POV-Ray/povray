// Persistence Of Vision Raytracer version 3.1 sample file.
// The TEXTUREn.POV files demonstrate all textures in TEXTURES.INC

camera {
   location <0, 5, -30>
   direction <0, 0,  1.35>
   look_at 5*y
}

light_source {<0, 0, -1000> rgb 0.66}

light_source {<150, 50, -200> rgb 0.66}

#local Stack =
union {
   box{-1,1}
   cylinder {y,-y,1 translate 2*y}
   sphere{<0, 4, 0>, 1}
}

#local Dist     =  0;
#local Top      = 10.5;
#local Left     =-12;
#local RowDelta = -7;
#local ColDelta =  3;

#local Row    = 0;
#local RowPos = Top;
#while (Row < 3)
  #local Col    = 0;
  #local ColPos = Left;
  #while (Col<9)
    object {
      Stack
      texture{Textures[Row][Col]}
      translate <ColPos, RowPos, Dist>
    }
    #local Col    = Col+1;
    #local ColPos = ColPos+ColDelta;
  #end
  #local Row      = Row+1;
  #local RowPos   = RowPos+RowDelta;
#end

plane { -z, -1.1 pigment {rgb 1 } }
