// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision raytracer version 3.1 sample file.
camera {
    location <-2.0, 1.75, -5>
    right x*1.3333
    angle 70
    look_at <-0.75, -0.5, 0>
}

light_source { <20, 20, -25> rgb 1 }
plane { y, 0  pigment { Plum }}
