#version 3.8;
global_settings{ assumed_gamma 1.0 }
#default{ finish { emission 1 diffuse 0 } }
#declare Teo="Li Europan lingues es membres del sam familie. Lor separat existentie es un myth. Por scientie, musica, sport etc, litot Europa usa li sam vocabular. Li lingues differe solmen in li grammatica, li pronunciation e li plu commun vocabules. Omnicos directe al desirabilite de un nov lingua franca: On refusa continuar payar custosi traductores.\n At solmen va esser necessi far uniform grammatica, pronunciation e plu commun paroles. Ma quande lingues coalesce, li grammatica del resultant lingue es plu simplic e regulari quam ti del coalescent lingues. Li nov lingua franca va esser plu simplic e regulari quam li existent Europanlingues. It va esser tam simplic quam Occidental in fact, it va esser Occidental. A un Angleso it va semblar un simplificat Angles, quam un skeptic Cambridge amico dit me que Occidental es. ";

#declare k=25;
galley{ internal 1 thickness 0.1 spacing 1.8 wrap true width k leading 1.2 indentation 2 Teo
no_shadow
translate 9*y-13*x
texture { pigment { rgb 0 } }
}


camera { orthographic
location <0,2,-30>
up y
right image_width/image_height*x
direction z
look_at 0 
angle 50
}

#declare IS=texture { pigment { color srgb <253,248,242>/255  } } ;

plane { z,0 texture { IS } translate 10*z }

