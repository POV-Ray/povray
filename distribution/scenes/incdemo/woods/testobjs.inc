// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision Raytracer sample file.
// Test object arrangement for the wood_x.pov files.

union {
    union {
        object { Test texture { Test1 rotate x*Angle } translate <-4.2, 4.2, 0> }
        object { Test texture { Test2 rotate x*Angle } translate <-2.1, 4.2, 0> }
        object { Test texture { Test3 rotate x*Angle } translate < 0.0, 4.2, 0> }
        object { Test texture { Test4 rotate x*Angle } translate < 2.1, 4.2, 0> }
        object { Test texture { Test5 rotate x*Angle } translate < 4.2, 4.2, 0> }
    }

    union {
        object { Test texture { Test6 rotate x*Angle } translate <-4.2, 2.1, 0> }
        object { Test texture { Test7 rotate x*Angle } translate <-2.1, 2.1, 0> }
        object { Test texture { Test8 rotate x*Angle } translate < 0.0, 2.1, 0> }
        object { Test texture { Test9 rotate x*Angle } translate < 2.1, 2.1, 0> }
        object { Test texture { Test10 rotate x*Angle }translate < 4.2, 2.1, 0> }
    }

    union {
        object { Test texture { Test11 rotate x*Angle } translate <-4.2, 0.0, 0> }
        object { Test texture { Test12 rotate x*Angle } translate <-2.1, 0.0, 0> }
        object { Test texture { Test13 rotate x*Angle } translate < 0.0, 0.0, 0> }
        object { Test texture { Test14 rotate x*Angle } translate < 2.1, 0.0, 0> }
        object { Test texture { Test15 rotate x*Angle } translate < 4.2, 0.0, 0> }
    }

    union {
        object { Test texture { Test16 rotate x*Angle } translate <-4.2,-2.1, 0> }
        object { Test texture { Test17 rotate x*Angle } translate <-2.1,-2.1, 0> }

        object { Test texture { Test18 rotate x*Angle } translate < 0.0,-2.1, 0> }

// The following objects are placeholders for testing other wood textures and
// are not currently used:
//      object { Test texture { Test19 rotate x*Angle } translate < 2.1,-2.1, 0> }
//      object { Test texture { Test20 rotate x*Angle } translate < 4.2,-2.1, 0> }
    }
/*
    union {
        object { Test texture { Test21 rotate x*Angle } translate <-4.2,-4.2, 0> }
        object { Test texture { Test22 rotate x*Angle } translate <-2.1,-4.2, 0> }
        object { Test texture { Test23 rotate x*Angle } translate < 0.0,-4.2, 0> }
        object { Test texture { Test24 rotate x*Angle } translate < 2.1,-4.2, 0> }
        object { Test texture { Test25 rotate x*Angle } translate < 4.2,-4.2, 0> }
    }
*/
}
