// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision Raytracer sample file.
// Used in conjunction with BRASSES.POV, COPPERS.POV, CHROMES.POV, & SILVERS.POV
// to demonstrate the textures in metals.inc.

camera {
   location <0, 20, -30>
   direction <0, 0,  1.75>
   right x*1.33
   look_at 0*y
}

light_source {<-50, 50, -1000> color Gray50}
light_source {< 50, 30, -20> color Gray75}

//background { color Gray50 }

sky_sphere {
    pigment {
        gradient y
        color_map {[0, 1  color Gray50 color Gray80]}
    }
}

plane { y, 0
    pigment {checker color Gray40 color Gray80
        scale <1, 1, 10000>
        rotate y*45
    }
    finish { ambient 0.45 }
}

#declare Stack =
union {
   sphere{<0, 4, 0>, 1}
   cone { -y,1, y, 0.5 translate 2*y }
   object {UnitBox}
}

#declare Height = 1;

#declare Dist1 =  10;
#declare Dist2 =   5;
#declare Dist3 =   0;
#declare Dist4 =  -5;
#declare Dist5 = -10;

#declare Col1 =  -8;
#declare Col2 =  -4;
#declare Col3 =   0;
#declare Col4 =   4;
#declare Col5 =   8;


object { Stack texture{T01} translate <Col1 Height Dist1 >}
object { Stack texture{T02} translate <Col2 Height Dist1 >}
object { Stack texture{T03} translate <Col3 Height Dist1 >}
object { Stack texture{T04} translate <Col4 Height Dist1 >}
object { Stack texture{T05} translate <Col5 Height Dist1 >}

object { Stack texture{T06} translate <Col1 Height Dist2 >}
object { Stack texture{T07} translate <Col2 Height Dist2 >}
object { Stack texture{T08} translate <Col3 Height Dist2 >}
object { Stack texture{T09} translate <Col4 Height Dist2 >}
object { Stack texture{T10} translate <Col5 Height Dist2 >}

object { Stack texture{T11} translate <Col1 Height Dist3 >}
object { Stack texture{T12} translate <Col2 Height Dist3 >}
object { Stack texture{T13} translate <Col3 Height Dist3 >}
object { Stack texture{T14} translate <Col4 Height Dist3 >}
object { Stack texture{T15} translate <Col5 Height Dist3 >}

object { Stack texture{T16} translate <Col1 Height Dist4 >}
object { Stack texture{T17} translate <Col2 Height Dist4 >}
object { Stack texture{T18} translate <Col3 Height Dist4 >}
object { Stack texture{T19} translate <Col4 Height Dist4 >}
object { Stack texture{T20} translate <Col5 Height Dist4 >}

object { Stack texture{T21} translate <Col1 Height Dist5 >}
object { Stack texture{T22} translate <Col2 Height Dist5 >}
object { Stack texture{T23} translate <Col3 Height Dist5 >}
object { Stack texture{T24} translate <Col4 Height Dist5 >}
object { Stack texture{T25} translate <Col5 Height Dist5 >}


