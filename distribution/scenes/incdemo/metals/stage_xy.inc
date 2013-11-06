// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision Raytracer sample file.
// Used in conjunction with BRASSES.POV, COPPERS.POV, CHROMES.POV, & SILVERS.POV
// to demonstrate the textures in metals.inc.

camera {
   location <0, 0, -60>
   direction <0, 0,  1.5>
   right x*1.33
   look_at 0*y
}

light_source {<-50, 50, -1000> color Gray50}
light_source {<150, 50, -200> color Gray25}

//background { color Gray50 }

sky_sphere {
    pigment {
        gradient y
        color_map {[0, 1  color Gray50 color Gray80]}
    }
}

plane { z, 2.5 pigment {Gray50} finish { ambient 0.45 } }

#declare Stack =
union {
   sphere{<0, 4, 0>, 1}
   object {Disk_Y translate 2*y}
   object {UnitBox}
}

#declare Dist = 0;

#declare Row1 =  14;
#declare Row2 =   7;
#declare Row3 =   0;
#declare Row4 =  -7;
#declare Row5 = -14;

#declare Col1 = -18;
#declare Col2 =  -9;
#declare Col3 =   0;
#declare Col4 =   9;
#declare Col5 =  18;


object { Stack texture{T01} translate <Col1 Row1 Dist> }
object { Stack texture{T02} translate <Col2 Row1 Dist> }
object { Stack texture{T03} translate <Col3 Row1 Dist> }
object { Stack texture{T04} translate <Col4 Row1 Dist> }
object { Stack texture{T05} translate <Col5 Row1 Dist> }

object { Stack texture{T06} translate <Col1 Row2 Dist> }
object { Stack texture{T07} translate <Col2 Row2 Dist> }
object { Stack texture{T08} translate <Col3 Row2 Dist> }
object { Stack texture{T09} translate <Col4 Row2 Dist> }
object { Stack texture{T10} translate <Col5 Row2 Dist> }

object { Stack texture{T11} translate <Col1 Row3 Dist> }
object { Stack texture{T12} translate <Col2 Row3 Dist> }
object { Stack texture{T13} translate <Col3 Row3 Dist> }
object { Stack texture{T14} translate <Col4 Row3 Dist> }
object { Stack texture{T15} translate <Col5 Row3 Dist> }

object { Stack texture{T16} translate <Col1 Row4 Dist> }
object { Stack texture{T17} translate <Col2 Row4 Dist> }
object { Stack texture{T18} translate <Col3 Row4 Dist> }
object { Stack texture{T19} translate <Col4 Row4 Dist> }
object { Stack texture{T20} translate <Col5 Row4 Dist> }

object { Stack texture{T21} translate <Col1 Row5 Dist> }
object { Stack texture{T22} translate <Col2 Row5 Dist> }
object { Stack texture{T23} translate <Col3 Row5 Dist> }
object { Stack texture{T24} translate <Col4 Row5 Dist> }
object { Stack texture{T25} translate <Col5 Row5 Dist> }


