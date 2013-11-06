// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

//    Persistence of Vision Ray Tracer version 3.5 Include File
//    File: glass.inc
//    Last updated: 2001.8.19
//	   Author: Christoph Hormann
//    Description: Glass materials using new POV 3.5 features.
//
//    Upated: 07Aug2008 (jh)
//    Use these pigments, finishes and interiors to make a glass "material"
//    Converted glass_old textures to materials.
//    See glass_old.inc for examples

#ifndef(Glass_Inc_Temp)
#declare Glass_Inc_Temp=version;
#version 3.5;

#ifdef(View_POV_Include_Stack)
    #debug "including glass.inc\n"
#end

#include "glass_old.inc"
#include "consts.inc"

// #### colors with transparency for use in pigment ####

// ------- colors derived from old glass.inc include file

#declare Col_Glass_Old=color rgbf <0.8, 0.9, 0.85, 0.85>;
#declare Col_Glass_Winebottle=color rgbf <0.4, 0.72, 0.4, 0.6>;
#declare Col_Glass_Beerbottle=color rgbf <0.7, 0.5, 0.1, 0.6>;
#declare Col_Glass_Ruby=color rgbf <0.9, 0.1, 0.2, 0.8>;
#declare Col_Glass_Green=color rgbf <0.8, 1, 0.95, 0.9>;
#declare Col_Glass_Dark_Green=color rgbf <0.1, 0.7, 0.8, 0.8>;
#declare Col_Glass_Yellow=color rgbf <0.8, 0.8, 0.2, 0.8>;
#declare Col_Glass_Orange=color rgbf <1.0, 0.5, 0.0, 0.8>;
#declare Col_Glass_Vicksbottle=color rgbf <0.1, 0.15, 0.5, 0.9>;

// ------- additional colors

#declare Col_Glass_Clear=color rgbf <1.0, 1.0, 1.0, 1.0>;
#declare Col_Glass_General=color rgbf <0.97, 0.99, 0.98, 0.9>;
#declare Col_Glass_Bluish=color rgbf <0.90, 0.94, 1.0, 0.8>;

// #### colors without transparency (for fade_color) ####

// ------- colors derived from old glass.inc include file

#declare Col_Winebottle=color rgb <0.4, 0.72, 0.4>;
#declare Col_Beerbottle=color rgb <0.7, 0.5, 0.1>;
#declare Col_Ruby=color rgb <0.9, 0.1, 0.2>;
#declare Col_Green=color rgb <0.8, 1, 0.95>;
#declare Col_Dark_Green=color rgb <0.1, 0.7, 0.8>;
#declare Col_Yellow=color rgb <0.8, 0.8, 0.2>;
#declare Col_Orange=color rgb <1.0, 0.5, 0.0>;
#declare Col_Vicksbottle=color rgb <0.1, 0.15, 0.5>;

// ------- additional colors

#declare Col_Red_01=color rgb <0.85, 0.10, 0.20>;
#declare Col_Red_02=color rgb <0.85, 0.35, 0.10>;
#declare Col_Red_03=color rgb <0.75, 0.20, 0.25>;
#declare Col_Red_04=color rgb <0.50, 0.10, 0.10>;

#declare Col_Green_01=color rgb <0.10, 0.85, 0.20>;
#declare Col_Green_02=color rgb <0.35, 0.85, 0.10>;
#declare Col_Green_03=color rgb <0.20, 0.75, 0.25>;
#declare Col_Green_04=color rgb <0.10, 0.50, 0.10>;

#declare Col_Blue_01=color rgb <0.35, 0.42, 0.85>;
#declare Col_Blue_02=color rgb <0.35, 0.65, 0.85>;
#declare Col_Blue_03=color rgb <0.35, 0.40, 0.75>;
#declare Col_Blue_04=color rgb <0.10, 0.10, 0.50>;

#declare Col_Yellow_01=color rgb <0.85, 0.85, 0.20>;
#declare Col_Yellow_02=color rgb <0.75, 0.78, 0.10>;
#declare Col_Yellow_03=color rgb <0.78, 0.75, 0.10>;
#declare Col_Yellow_04=color rgb <0.50, 0.50, 0.10>;

// ------- colors of common minerals

#declare Col_Amethyst_01=color rgb<0.4392, 0.1765, 0.3137>;
#declare Col_Amethyst_02=color rgb<0.5843, 0.3686, 0.4941>;
#declare Col_Amethyst_03=color rgb<0.6980, 0.5059, 0.6157>;
#declare Col_Amethyst_04=color rgb<0.9059, 0.7176, 0.8471>;
#declare Col_Amethyst_05=color rgb<0.8902, 0.4549, 0.7059>;
#declare Col_Amethyst_06=color rgb<0.4980, 0.2902, 0.4235>;

#declare Col_Apatite_01=color rgb<0.0549, 0.3451, 0.4314>;
#declare Col_Apatite_02=color rgb<0.2941, 0.5765, 0.5647>;
#declare Col_Apatite_03=color rgb<0.5176, 0.7333, 0.7020>;
#declare Col_Apatite_04=color rgb<0.3412, 0.5961, 0.4118>;
#declare Col_Apatite_05=color rgb<0.3647, 0.8196, 0.7451>;

#declare Col_Aquamarine_01=color rgb<0.5333, 0.6157, 0.6196>;
#declare Col_Aquamarine_02=color rgb<0.6627, 0.7020, 0.7255>;
#declare Col_Aquamarine_03=color rgb<0.7412, 0.8235, 0.8431>;
#declare Col_Aquamarine_04=color rgb<0.6039, 0.7216, 0.7882>;
#declare Col_Aquamarine_05=color rgb<0.5804, 0.7098, 0.7765>;
#declare Col_Aquamarine_06=color rgb<0.7255, 0.8510, 0.9176>;

#declare Col_Azurite_01=color rgb<0.2863, 0.3569, 0.7216>;
#declare Col_Azurite_02=color rgb<0.5333, 0.6549, 0.9059>;
#declare Col_Azurite_03=color rgb<0.3020, 0.3529, 0.6314>;
#declare Col_Azurite_04=color rgb<0.1137, 0.1569, 0.4471>;

#declare Col_Citrine_01=color rgb<0.3020, 0.2314, 0.1529>;
#declare Col_Citrine_02=color rgb<0.4667, 0.3804, 0.2980>;
#declare Col_Citrine_03=color rgb<0.5020, 0.4118, 0.3529>;

#declare Col_Emerald_01=color rgb<0.0471, 0.4000, 0.2549>;
#declare Col_Emerald_02=color rgb<0.2157, 0.6353, 0.4902>;
#declare Col_Emerald_03=color rgb<0.3216, 0.6627, 0.5961>;
#declare Col_Emerald_04=color rgb<0.1843, 0.4549, 0.3843>;
#declare Col_Emerald_05=color rgb<0.2863, 0.6431, 0.4431>;
#declare Col_Emerald_06=color rgb<0.2353, 0.5922, 0.3804>;
#declare Col_Emerald_07=color rgb<0.4392, 0.7725, 0.5804>;

#declare Col_Fluorite_01=color rgb<0.5098, 0.3294, 0.4039>;
#declare Col_Fluorite_02=color rgb<0.3020, 0.1569, 0.2431>;
#declare Col_Fluorite_03=color rgb<0.5098, 0.2902, 0.2745>;
#declare Col_Fluorite_04=color rgb<0.5373, 0.3098, 0.0902>;
#declare Col_Fluorite_05=color rgb<0.7020, 0.5529, 0.7294>;
#declare Col_Fluorite_06=color rgb<0.4745, 0.2627, 0.4824>;
#declare Col_Fluorite_07=color rgb<0.2314, 0.0784, 0.2902>;
#declare Col_Fluorite_08=color rgb<0.5804, 0.6235, 0.6745>;
#declare Col_Fluorite_09=color rgb<0.4392, 0.4745, 0.5333>;

#declare Col_Gypsum_01=color rgb<0.9020, 0.8824, 0.7882>;
#declare Col_Gypsum_02=color rgb<0.8039, 0.8667, 0.8157>;
#declare Col_Gypsum_03=color rgb<0.7098, 0.6863, 0.6000>;
#declare Col_Gypsum_04=color rgb<0.8275, 0.8078, 0.5765>;
#declare Col_Gypsum_05=color rgb<0.8196, 0.7373, 0.5294>;
#declare Col_Gypsum_06=color rgb<0.5961, 0.4941, 0.3020>;

#declare Col_Ruby_01=color rgb<0.4980, 0.0824, 0.2275>;
#declare Col_Ruby_02=color rgb<0.6235, 0.1490, 0.3137>;
#declare Col_Ruby_03=color rgb<0.7412, 0.3294, 0.4745>;
#declare Col_Ruby_04=color rgb<0.8039, 0.2039, 0.5569>;
#declare Col_Ruby_05=color rgb<0.5882, 0.1255, 0.4784>;

#declare Col_Sapphire_01=color rgb<0.2118, 0.3020, 0.4980>;
#declare Col_Sapphire_02=color rgb<0.0588, 0.2000, 0.5569>;
#declare Col_Sapphire_03=color rgb<0.0392, 0.2353, 0.5686>;

#declare Col_Topaz_01=color rgb<0.7333, 0.7451, 0.6980>;
#declare Col_Topaz_02=color rgb<0.5922, 0.6196, 0.5922>;
#declare Col_Topaz_03=color rgb<0.5922, 0.6157, 0.6510>;

#declare Col_Tourmaline_01=color rgb<0.3725, 0.4235, 0.2941>;
#declare Col_Tourmaline_02=color rgb<0.2745, 0.3490, 0.2235>;
#declare Col_Tourmaline_03=color rgb<0.2627, 0.2549, 0.1451>;
#declare Col_Tourmaline_04=color rgb<0.1333, 0.4706, 0.3412>;
#declare Col_Tourmaline_05=color rgb<0.0627, 0.2980, 0.2118>;
#declare Col_Tourmaline_06=color rgb<0.0196, 0.2706, 0.1961>;

#declare Col_Amber_01=color rgb<0.3725, 0.4235, 0.2941>;
#declare Col_Amber_02=color rgb<0.8941, 0.6863, 0.2902>;
#declare Col_Amber_03=color rgb<0.7412, 0.4235, 0.2588>;
#declare Col_Amber_04=color rgb<0.7059, 0.3961, 0.1216>;
#declare Col_Amber_05=color rgb<0.5647, 0.2392, 0.0745>;
#declare Col_Amber_06=color rgb<0.8157, 0.6549, 0.2392>;
#declare Col_Amber_07=color rgb<0.9882, 0.6941, 0.0431>;
#declare Col_Amber_08=color rgb<0.8549, 0.4706, 0.1294>;
#declare Col_Amber_09=color rgb<0.5294, 0.2431, 0.1529>;


// #### additional glass finishes ####

#declare F_Glass5 =
  finish {
    specular 0.7
    roughness 0.001
    ambient 0
    diffuse 0
    reflection {
      0.2, 1.0
      fresnel on
    }
    conserve_energy
  }

#declare F_Glass6 =
  finish {
    specular 0.6
    roughness 0.002
    ambient 0
    diffuse 0.1
    brilliance 5
    reflection {
      0.1, 1.0
      fresnel on
    }
    conserve_energy
  }

#declare F_Glass7 =
  finish {
    specular 0.9
    roughness 0.001
    ambient 0
    diffuse 0
    reflection {
      0.0, 1.0
      fresnel on
    }
    conserve_energy
 }

#declare F_Glass8 =
  finish {
    specular 0.6
    roughness 0.005
    ambient 0
    diffuse 0.15
    brilliance 4
    reflection {
      0.2, 1.0
      fresnel on
    }
    conserve_energy
 }

#declare F_Glass9 =
  finish {
    specular 0.8
    roughness 0.001
    ambient 0
    diffuse 0
    reflection {
      0.05, 1.0
    }
    conserve_energy
 }

#declare F_Glass10 =
  finish {
    specular 0.6
    roughness 0.002
    ambient 0
    diffuse 0.1
    reflection {
      0.05, 1.0
    }
    conserve_energy
 }


// #### additional glass interiors ####

#declare I_Glass1=
  interior {
    ior 1.5
    fade_distance 1.0
    fade_power 2
  }

#declare I_Glass2=
  interior {
    ior 1.5
    fade_distance 0.5
    fade_power 1001
  }

#declare I_Glass3=
  interior {
    ior 1.5
    fade_distance 1.0
    fade_power 1001
  }

#declare I_Glass4=
  interior {
    ior 1.5
    fade_distance 2.0
    fade_power 1001
  }

#declare I_Glass_Fade_Sqr1=interior { I_Glass1 }
#declare I_Glass_Fade_Exp1=interior { I_Glass2 }
#declare I_Glass_Fade_Exp2=interior { I_Glass3 }
#declare I_Glass_Fade_Exp3=interior { I_Glass4 }


#declare I_Glass_Dispersion1=
  interior {
    ior 1.5
    dispersion 1.02
    fade_distance 1
    fade_power 1001
  }

#declare I_Glass_Dispersion2=
  interior {
    ior 1.5
    dispersion 1.15
    fade_distance 1
    fade_power 1001
  }

#declare I_Glass_Caustics1=
  interior {
    ior 1.5
    caustics 0.5
    fade_distance 1
    fade_power 1001
  }

#declare I_Glass_Caustics2=
  interior {
    ior 1.5
    caustics 1.0
    fade_distance 1
    fade_power 1001
  }

#macro I_Glass_Exp(Distance)
  ior 1.5
  fade_distance Distance
  fade_power 1001
#end

#macro I_Glass_Sqr(Distance)
  ior 1.5
  fade_distance Distance
  fade_power 2
#end

#version Glass_Inc_Temp;
#end
