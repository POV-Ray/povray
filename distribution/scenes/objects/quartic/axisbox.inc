// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer version 3.5 sample file.

// Include file for steiner.pov, grafbic.pov
// by Alexander Enzmann

// Declare the sizes of cylinders that represent coordinate axes
#declare Axes_X_Radii = <1.0, 0.01, 0.01>;
#declare Axes_Y_Radii = <0.01, 1.0, 0.01>;
#declare Axes_Z_Radii = <0.01, 0.01, 1.0>;

// Define the colors of the coordinate axes
#declare X_Axis_Color = color green 1 blue 1;
#declare Y_Axis_Color = color red 1 green 1;
#declare Z_Axis_Color = color red 1 blue 1;

// Define the ways that the coordinate axes can be translated
#declare Translate_X_Axis_Up       = < 0.0,  1.0,  0.0>;
#declare Translate_X_Axis_Down     = < 0.0, -1.0,  0.0>;
#declare Translate_X_Axis_Forward  = < 0.0,  0.0,  1.0>;
#declare Translate_X_Axis_Back     = < 0.0,  0.0, -1.0>;
#declare Translate_Y_Axis_Left     = < 1.0,  0.0,  0.0>;
#declare Translate_Y_Axis_Right    = <-1.0,  0.0,  0.0>;
#declare Translate_Y_Axis_Forward  = < 0.0,  0.0,  1.0>;
#declare Translate_Y_Axis_Back     = < 0.0,  0.0, -1.0>;
#declare Translate_Z_Axis_Up       = < 0.0,  1.0,  0.0>;
#declare Translate_Z_Axis_Down     = < 0.0, -1.0,  0.0>;
#declare Translate_Z_Axis_Left     = < 1.0,  0.0,  0.0>;
#declare Translate_Z_Axis_Right    = <-1.0,  0.0,  0.0>;

// Define coordinate axes
#declare Axes =
union {
   object {
      Cylinder_X
      scale Axes_X_Radii
      texture { pigment { X_Axis_Color } }
   }

   object {
      Cylinder_Y
      scale Axes_Y_Radii
      texture { pigment { Y_Axis_Color } }
   }
   object {
      Cylinder_Z
      scale Axes_Z_Radii
      texture { pigment { Z_Axis_Color } }
   }
}

// Translate the axis up, down, right, ...,  to form the three
// planes that divide the standard octants.
#declare Axis_Box =
union {
   object {
      Cylinder_X
      scale Axes_X_Radii
      translate Translate_X_Axis_Up
      texture { pigment { X_Axis_Color } }
   }

   object {
      Cylinder_X
      scale Axes_X_Radii
      translate Translate_X_Axis_Down
      texture { pigment { X_Axis_Color } }
   }

   object {
      Cylinder_Y
      scale Axes_Y_Radii
      translate Translate_Y_Axis_Forward
      texture { pigment { color Y_Axis_Color } }
   }

   object {
      Cylinder_Y
      scale Axes_Y_Radii
      translate Translate_Y_Axis_Back
      texture { pigment { Y_Axis_Color } }
   }

   object {
      Cylinder_Z
      scale Axes_Z_Radii
      translate Translate_Z_Axis_Left
      texture { pigment { Z_Axis_Color } }
   }

   object {
      Cylinder_Z
      scale Axes_Z_Radii
      translate Translate_Z_Axis_Right
      texture { pigment { Z_Axis_Color } }
   }
}

