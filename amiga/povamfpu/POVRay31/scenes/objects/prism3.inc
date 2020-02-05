 camera {
   location <-10, 10, -10>
   right 4/3 * x
   up z
   look_at <0, 0, 0>
   angle 20
 }

 light_source { <0, 50, 10> rgb 0.7 }
 light_source { <-30, 20, -50> rgb 0.7 }
 light_source { <30, 20, -20> rgb 0.7 }

 plane { y, -1 pigment { color rgb 0.7 } }

