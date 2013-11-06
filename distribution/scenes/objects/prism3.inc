// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

 camera {
   location <-10, 10, -10>
   right     x*image_width/image_height
   up z
   look_at <0, 0, 0>
   angle 20
 }

 light_source { <0, 50, 10> rgb 0.7 }
 light_source { <-30, 20, -50> rgb 0.7 }
 light_source { <30, 20, -20> rgb 0.7 }

 plane { y, -1 pigment { color rgb 0.1 } }

