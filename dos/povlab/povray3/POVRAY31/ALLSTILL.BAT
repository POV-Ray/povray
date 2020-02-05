@echo off
echo Test>allscene\test
if not exist allscene\test md allscene
del allscene\test>nul
del userabor.mes>nul
goto Start
:Start
povray allstill +Lscenes\advanced                 +Ichess2
povray allstill +Lscenes\advanced                 +Idiffract
povray allstill +Lscenes\advanced                 +Ifloat5
povray allstill +Lscenes\advanced                 +Ilamppost
povray allstill +Lscenes\advanced                 +Imist
povray allstill +Lscenes\advanced                 +Imtmand
povray allstill +Lscenes\advanced                 +Iquilt1
povray allstill +Lscenes\advanced                 +Iskyvase
povray allstill +Lscenes\advanced                 +Isombrero
povray allstill +Lscenes\advanced                 +Isunsethf
povray allstill +Lscenes\advanced                 +Iswirlbox
povray allstill +Lscenes\advanced                 +Iwg6
povray allstill +Lscenes\advanced                 +Iwhiltile
povray allstill +Lscenes\advanced                 +Iwoodbox
if EXIST userabor.mes goto End
povray allstill +Lscenes\advanced\drums2          +Idrums
povray allstill +Lscenes\advanced\fish13          +Ifish13
povray allstill +Lscenes\advanced\ionic5          +Iionic5
povray allstill +Lscenes\advanced\newltpot        +Iteapot2
povray allstill +Lscenes\advanced\newltpot        +Iteapot3
povray allstill +Lscenes\advanced\newltpot        +Iteapot4
povray allstill +Lscenes\advanced\ntreal          +Intreal
povray allstill +Lscenes\advanced\piece2          +Ipiece2
povray allstill +Lscenes\advanced\piece3          +Ipiece3
povray allstill +Lscenes\advanced\teapot          +Iteapot
if EXIST userabor.mes goto End
povray allstill +Lscenes\arrays                   +Iarray1
povray allstill +Lscenes\arrays                   +Iarray2
povray allstill +Lscenes\arrays                   +Iarray3
if EXIST userabor.mes goto End
povray allstill +Lscenes\camera                   +Ibasicvue
povray allstill +Lscenes\camera                   +Icamera1a
povray allstill +Lscenes\camera                   +Icamera1b
povray allstill +Lscenes\camera                   +Icamera1c
povray allstill +Lscenes\camera                   +Icamera1d
povray allstill +Lscenes\camera                   +Icamera1e
povray allstill +Lscenes\camera                   +Icamera1f
povray allstill +Lscenes\camera                   +Icamera3a
povray allstill +Lscenes\camera                   +Icamera3b
povray allstill +Lscenes\camera                   +Icamera3c
povray allstill +Lscenes\camera                   +Icamera3d
povray allstill +Lscenes\camera                   +Ifisheye
povray allstill +Lscenes\camera                   +Ifocalb1
if EXIST userabor.mes goto End
povray allstill +Lscenes\csg                      +Icliptst2
povray allstill +Lscenes\csg                      +Igranite
povray allstill +Lscenes\csg                      +Ihfclip
povray allstill +Lscenes\csg                      +Iintee1
povray allstill +Lscenes\fileio                   +Ifileio
if EXIST userabor.mes goto End
povray allstill +Lscenes\incdemo                  +Ichars
povray allstill +Lscenes\incdemo                  +Icolors
povray allstill +Lscenes\incdemo                  +Ifinish
povray allstill +Lscenes\incdemo                  +Ishapes
povray allstill +Lscenes\incdemo                  +Ishapes2
povray allstill +Lscenes\incdemo                  +Itexture1
povray allstill +Lscenes\incdemo                  +Itexture2
povray allstill +Lscenes\incdemo                  +Itexture3
if EXIST userabor.mes goto End
povray allstill +Lscenes\incdemo\glasses          +Iglasses
povray allstill +Lscenes\incdemo\metals           +Ibrasses
povray allstill +Lscenes\incdemo\metals           +Ichromes
povray allstill +Lscenes\incdemo\metals           +Icoppers
povray allstill +Lscenes\incdemo\metals           +Igolds
povray allstill +Lscenes\incdemo\metals           +Isilvers
povray allstill +Lscenes\incdemo\stones           +Ibenediti
povray allstill +Lscenes\incdemo\stones           +Imarbteal
povray allstill +Lscenes\incdemo\stones           +Ipinkmarb
povray allstill +Lscenes\incdemo\stones           +Irdgranit
povray allstill +Lscenes\incdemo\stones           +Istones1
povray allstill +Lscenes\incdemo\stones           +Istones2
if EXIST userabor.mes goto End
povray allstill +Lscenes\incdemo\woods            +Iash
povray allstill +Lscenes\incdemo\woods            +Ibubinga
povray allstill +Lscenes\incdemo\woods            +Icedar
povray allstill +Lscenes\incdemo\woods            +Iorngwood
povray allstill +Lscenes\incdemo\woods            +Iteak
povray allstill +Lscenes\incdemo\woods            +Iwhiteash
povray allstill +Lscenes\incdemo\woods            +Iwoods1
povray allstill +Lscenes\incdemo\woods            +Iwoods2
povray allstill +Lscenes\incdemo\woods            +Iwood_1
povray allstill +Lscenes\incdemo\woods            +Iwood_10
povray allstill +Lscenes\incdemo\woods            +Iwood_11
povray allstill +Lscenes\incdemo\woods            +Iwood_12
povray allstill +Lscenes\incdemo\woods            +Iwood_13
povray allstill +Lscenes\incdemo\woods            +Iwood_14
povray allstill +Lscenes\incdemo\woods            +Iwood_15
povray allstill +Lscenes\incdemo\woods            +Iwood_16
povray allstill +Lscenes\incdemo\woods            +Iwood_17
povray allstill +Lscenes\incdemo\woods            +Iwood_18
povray allstill +Lscenes\incdemo\woods            +Iwood_19
povray allstill +Lscenes\incdemo\woods            +Iwood_2
povray allstill +Lscenes\incdemo\woods            +Iwood_3
povray allstill +Lscenes\incdemo\woods            +Iwood_4
povray allstill +Lscenes\incdemo\woods            +Iwood_5
povray allstill +Lscenes\incdemo\woods            +Iwood_6
povray allstill +Lscenes\incdemo\woods            +Iwood_7
povray allstill +Lscenes\incdemo\woods            +Iwood_8
povray allstill +Lscenes\incdemo\woods            +Iwood_9
if EXIST userabor.mes goto End
povray allstill +Lscenes\interior                 +Iatten1
povray allstill +Lscenes\interior                 +Iatten2
povray allstill +Lscenes\interior                 +Iballbox1
povray allstill +Lscenes\interior                 +Icaustic2
povray allstill +Lscenes\interior                 +Icrystal
povray allstill +Lscenes\interior                 +Ifoglayr
povray allstill +Lscenes\interior                 +Ifoglyr2
povray allstill +Lscenes\interior                 +Ifog_f
povray allstill +Lscenes\interior                 +Ifog_ft
povray allstill +Lscenes\interior                 +Ifog_std
povray allstill +Lscenes\interior                 +Iiortest
povray allstill +Lscenes\interior                 +Imagglass
povray allstill +Lscenes\interior                 +Inufog2
povray allstill +Lscenes\interior                 +Irainbow1
povray allstill +Lscenes\interior                 +Iskysph1
povray allstill +Lscenes\interior                 +Iskysph2
if EXIST userabor.mes goto End
povray allstill +Lscenes\interior\media           +Igalaxy
povray allstill +Lscenes\interior\media           +Ihollow1
povray allstill +Lscenes\interior\media           +Ihollow2
povray allstill +Lscenes\interior\media           +Ihollow3
povray allstill +Lscenes\interior\media           +Imedia1
povray allstill +Lscenes\interior\media           +Imedia2
povray allstill +Lscenes\interior\media           +Imedia3
povray allstill +Lscenes\interior\media           +Imedia4
povray allstill +Lscenes\interior\media           +Imedia5
if EXIST userabor.mes goto End
povray allstill +Lscenes\lights                   +Iarealit1
povray allstill +Lscenes\lights                   +Iarealit2
povray allstill +Lscenes\lights                   +Iarealit3
povray allstill +Lscenes\lights                   +Ifillite
povray allstill +Lscenes\lights                   +Ilaser
povray allstill +Lscenes\lights                   +Ishadows
povray allstill +Lscenes\lights                   +Isoft
povray allstill +Lscenes\lights                   +Ispotlite
if EXIST userabor.mes goto End
povray allstill +Lscenes\macros                   +Ilocal
povray allstill +Lscenes\macros                   +Imacro1
povray allstill +Lscenes\macros                   +Imacro2
povray allstill +Lscenes\macros                   +Imacro3
povray allstill +Lscenes\macros                   +Imacro4
povray allstill +Lscenes\macros                   +Ipyramid
povray allstill +Lscenes\macros                   +Ipyramid2
if EXIST userabor.mes goto End
povray allstill +Lscenes\objects                  +Iblob
povray allstill +Lscenes\objects                  +Iblob1a
povray allstill +Lscenes\objects                  +Iblob1b
povray allstill +Lscenes\objects                  +Iblob1c
povray allstill +Lscenes\objects                  +Ibox
povray allstill +Lscenes\objects                  +Ichesmsh
povray allstill +Lscenes\objects                  +Ifractal1
povray allstill +Lscenes\objects                  +Ifractal2
povray allstill +Lscenes\objects                  +Ifractal3
povray allstill +Lscenes\objects                  +Ifractal4
povray allstill +Lscenes\objects                  +Ilathe1a
povray allstill +Lscenes\objects                  +Ilathe1b
povray allstill +Lscenes\objects                  +Ilathe1c
povray allstill +Lscenes\objects                  +Ilathe2
povray allstill +Lscenes\objects                  +Ipawns
povray allstill +Lscenes\objects                  +Ipolygon
povray allstill +Lscenes\objects                  +Iprimitiv
povray allstill +Lscenes\objects                  +Iprism1
povray allstill +Lscenes\objects                  +Iprism2
povray allstill +Lscenes\objects                  +Iprism3a
povray allstill +Lscenes\objects                  +Iprism3b
povray allstill +Lscenes\objects                  +Iprism3c
povray allstill +Lscenes\objects                  +Isor1
povray allstill +Lscenes\objects                  +Isuperel1
povray allstill +Lscenes\objects                  +Isuperel2
povray allstill +Lscenes\objects                  +Isuperel3
povray allstill +Lscenes\objects                  +Itorus1
povray allstill +Lscenes\objects                  +Ittf1
povray allstill +Lscenes\objects                  +Iwtorus
if EXIST userabor.mes goto End
povray allstill +Lscenes\objects\math             +Ibezier
povray allstill +Lscenes\objects\math             +Ibicube
povray allstill +Lscenes\objects\math             +Ifolium
povray allstill +Lscenes\objects\math             +Igrafbic
povray allstill +Lscenes\objects\math             +Ihelix
povray allstill +Lscenes\objects\math             +Ihyptorus
povray allstill +Lscenes\objects\math             +Ilemnisc2
povray allstill +Lscenes\objects\math             +Ilemnisca
povray allstill +Lscenes\objects\math             +Imonkey
povray allstill +Lscenes\objects\math             +Ipartorus
povray allstill +Lscenes\objects\math             +Ipiriform
povray allstill +Lscenes\objects\math             +Iquarcyl
povray allstill +Lscenes\objects\math             +Iquarpara
povray allstill +Lscenes\objects\math             +Isteiner
povray allstill +Lscenes\objects\math             +Itcubic
povray allstill +Lscenes\objects\math             +Iteardrop
povray allstill +Lscenes\objects\math             +Itorus
povray allstill +Lscenes\objects\math             +Itrough
povray allstill +Lscenes\objects\math             +Iwitch
if EXIST userabor.mes goto End
povray allstill +Lscenes\radios                   +Irad2  +qr
if EXIST userabor.mes goto End
povray allstill +Lscenes\textures\pigments        +Iblkhole
povray allstill +Lscenes\textures\pigments        +Icantelop
povray allstill +Lscenes\textures\pigments        +Ichecker2
povray allstill +Lscenes\textures\pigments        +Icrack1
povray allstill +Lscenes\textures\pigments        +Idodec2
povray allstill +Lscenes\textures\pigments        +Ieight
povray allstill +Lscenes\textures\pigments        +Ifiltfun
povray allstill +Lscenes\textures\pigments        +Iinterp
povray allstill +Lscenes\textures\pigments        +Iirid
povray allstill +Lscenes\textures\pigments        +Imapper
povray allstill +Lscenes\textures\pigments        +Imappr2
povray allstill +Lscenes\textures\pigments        +Imatmap
povray allstill +Lscenes\textures\pigments        +Ipigmap
povray allstill +Lscenes\textures\pigments        +Ipigment
povray allstill +Lscenes\textures\pigments        +Iplanet
povray allstill +Lscenes\textures\pigments        +Ispiral1a
povray allstill +Lscenes\textures\pigments        +Itxtravg
povray allstill +Lscenes\textures\pigments        +Itxtrmap
povray allstill +Lscenes\textures\pigments        +Iwarp1
povray allstill +Lscenes\textures\pigments        +Iwarp2
if EXIST userabor.mes goto End
povray allstill +Lscenes\textures\pigments\skies  +Istars
povray allstill +Lscenes\textures\pigments\skies  +Is_cloud1
povray allstill +Lscenes\textures\pigments\skies  +Is_cloud2
povray allstill +Lscenes\textures\pigments\skies  +Is_cloud3
povray allstill +Lscenes\textures\pigments\skies  +Is_cloud4
povray allstill +Lscenes\textures\pigments\skies  +Is_cloud5
if EXIST userabor.mes goto End
povray allstill +Lscenes\textures\surfaces        +Iagate
povray allstill +Lscenes\textures\surfaces        +Iarches
povray allstill +Lscenes\textures\surfaces        +Ibozo
povray allstill +Lscenes\textures\surfaces        +Ibrick
povray allstill +Lscenes\textures\surfaces        +Ibumpmap
povray allstill +Lscenes\textures\surfaces        +Ibumps
povray allstill +Lscenes\textures\surfaces        +Icluster
povray allstill +Lscenes\textures\surfaces        +Icrackle
povray allstill +Lscenes\textures\surfaces        +Idensfile
povray allstill +Lscenes\textures\surfaces        +Idents
povray allstill +Lscenes\textures\surfaces        +Igradient
povray allstill +Lscenes\textures\surfaces        +Igranite
povray allstill +Lscenes\textures\surfaces        +Ihexagon
povray allstill +Lscenes\textures\surfaces        +Ileopard
povray allstill +Lscenes\textures\surfaces        +Imandel
if EXIST userabor.mes goto End
povray allstill +Lscenes\textures\surfaces        +Imarble
povray allstill +Lscenes\textures\surfaces        +Inormal
povray allstill +Lscenes\textures\surfaces        +Inormavg
povray allstill +Lscenes\textures\surfaces        +Inormmap
povray allstill +Lscenes\textures\surfaces        +Ionion
povray allstill +Lscenes\textures\surfaces        +Iquilt2
povray allstill +Lscenes\textures\surfaces        +Iquilted
povray allstill +Lscenes\textures\surfaces        +Iradial
povray allstill +Lscenes\textures\surfaces        +Iripples
povray allstill +Lscenes\textures\surfaces        +Islopemap
povray allstill +Lscenes\textures\surfaces        +Ispiral1
povray allstill +Lscenes\textures\surfaces        +Ispiral2
povray allstill +Lscenes\textures\surfaces        +Ispotted
povray allstill +Lscenes\textures\surfaces        +Iwaves
povray allstill +Lscenes\textures\surfaces        +Iwrinkles                

if EXIST userabor.mes goto End
rem crat_dat.pov renders height_field data for crater.pov
povray +w640 +i480 -d +v -p +ft +lscenes\objects  +icrat_dat +oscenes\objects\
povray allstill +lscenes\objects +icrater
del scenes\objects\crat_dat.tga

if EXIST userabor.mes goto End
rem DESK.POV is a multi-frame rendering but the initial frames
rem render image_maps for subsequent frames.  The end result is
rem a stlll image. Uses DESK.INI
povray          +Lscenes\advanced  DESK allstill 
ren allscene\DESK4.TGA DESK.TGA
tga2gif -d allscene\DESK
del allscene\DES??.TGA
goto End
:End
