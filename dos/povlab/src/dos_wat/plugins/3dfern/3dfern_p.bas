' 3D_FERN.BAS V1.02

' This PowerBASIC PLUGIN for POVLAB generates a 3D fern by
' using an 3D Iterated Function System !

' Robert Seidel
' seidel@ifk.uni-jena.de


$INCLUDE "argcomm" 'for splitting COMMAND$
$INCLUDE "plugin"  'POVLAB lib for PowerBASIC by R. Seidel

Obj.Name = "3DF"
Plugin.Name$ = "3DFERN_P"

DEFDBL A-Z

IF COMMAND$ = "/ASK" THEN
  OPEN Plugin.Name$ + ".PLG" FOR OUTPUT AS #1
  ? #1 , "TITLE: 3D_FERN-GENERATOR_V1.01_beta_with_PLUGIN.BAS"
  ? #1 , "COPYRIGHT: Robert_Seidel_1996_(seidel@ifk.uni-jena.de)"
  ? #1 , "WINDOW: 300 130"
  ? #1 , "TEXTZONE:  54 45 Angle_X -20 Angle_in_the_X-Plane"
  ? #1 , "TEXTZONE:  54 65 Angle_Y -90 Angle_in_the_Y-Plane"
  ? #1 , "TEXTZONE:  54 85 Angle_Z   0 Angle_in_the_Z-Plane"
  ? #1 , "TEXTZONE: 200 45 Iterations 950 Number_of_Iterations"
  ? #1 , "TEXTZONE: 200 65 Scale 0.05 Size_of_the_Spheres"
  ? #1 , "CASE:      10 107 Graphical_Preview 1 Preview_in_VGA 2"

  ? #1 , "MESSAGE: 3DFERN_is_CARDWARE,_it's_free_if_you_send_me_a_"+_
         "postcard_or_your_pic_via_e-mail_!"
ELSE

IF COMMAND$ = "" THEN

  ?
  ? "3D-FERN V1.02 beta
  ? "3D Iterated Function System FERN Plugin for POVLAB 3.20 and above"
  ?
  ? "(C) Robert Seidel 1996 (seidel@ifk.uni-jena)
  ?
  ? "3D-FERN is CARDWARE, if you send me a nice postcard or your picture"
  ? "created with 3D-FERN, you can use it for free ! ELSE SHAME ON YOU !"
  ?
  ? "3D-FERN created using PowerBASIC and my PLUGIN.BAS for POVLAB
  ?

ELSE

  C$ = COMMAND$
  Angle.X  = VAL(ArgV(1 , C$))
  Angle.Y  = VAL(ArgV(2 , C$))
  Angle.Z  = VAL(ArgV(3 , C$))
  Iter     = VAL(ArgV(4 , C$))
  Scale    = VAL(ArgV(5 , C$))
  Preview% = VAL(ArgV(6 , C$))

  SHARED Preview%

%abstand.X = 10
%abstand.Y = 10
%abstand.Z = 10

IF Preview% THEN
  SCREEN 0
  SCREEN 12
END IF

OPEN Plugin.Name$ + ".INC" FOR OUTPUT AS #1
MAKEFERN Angle.X, Angle.Y, Angle.Z, Iter , Scale

IF Preview% THEN
  ! MOV AX ,3
  ! INT &h10
END IF

END IF
END IF

SUB MAKEFERN(ALPHA , BETA , GAMMA, num , scale)

' 3D IFS data for the FERN

i = 3
DIM A(i),B(i),C(i),D(i),E(i),F(i),G(i),H(i),M(i),N(i),Q(i),R(i),P(i)

FOR i = 0 TO 3
 READ A(i),B(i),C(i),D(i),E(i),F(i),G(i),H(i),M(i),N(i),Q(i),R(i),P(i)
NEXT

DATA 0  , 0   ,0,0  ,.18,0 ,0,   0,  0,0,0   ,0,0.01
DATA .83, 0   ,0,0  ,.86,.1,0,-.12,.84,0,1.62,0,.85
DATA .22, -.23,0,.24,.22,0 ,0,0   ,.32,0, .82,0,.92
DATA-.22,  .23,0,.24,.22,0 ,0,0   ,.32,0, .82,0,1.0


 CA = COS(ALPHA * 0.0174533)
 CB = COS(BETA  * 0.0174533)
 CG = COS(GAMMA * 0.0174533)
 SA = SIN(ALPHA * 0.0174533)
 SB = SIN(BETA  * 0.0174533)
 SG = SIN(GAMMA * 0.0174533)

  loc.X = %abstand.X * CB * CG +_
          %abstand.Y * ( CA * SG + SA * SB * CG ) +_
          %abstand.Z * ( SA * SG - CA * SB * CG )
  loc.Y = %abstand.X * ( -CB * SG ) +_
          %abstand.Y * ( CA * CG - SA * SB * SG ) +_
          %abstand.Z * ( SA * CG + CA * SB * SG )
  loc.Z = %abstand.X * SB +_
          %abstand.Y * ( -SA * CB ) +_
          %abstand.Z * ( CA * CB )

FOR I = 1 TO Num

  PK = RND

  IF PK < P(0) THEN K = 0 _
    ELSE IF PK < P(1) THEN K = 1 _
    ELSE IF PK < P(2) THEN K = 2 _
    ELSE K = 3
  X1 = (A(K) * X + B(K) * Y + C(K) * Z + N(K))
  Y1 = (D(K) * X + E(K) * Y + F(K) * Z + Q(K))
  Z  = (G(K) * X + H(K) * Y + M(K) * Z + R(K))

  loc.X = X1 * CB * CG +_
          Y1 * ( CA * SG + SA * SB * CG ) +_
          Z * ( SA * SG - CA * SB * CG )
  loc.Y = X1 * ( -CB * SG ) +_
          Y1 * ( CA * CG - SA * SB * SG ) +_
          Z * ( SA * CG + CA * SB * SG )
  loc.Z = X1 * SB +_
          Y1 * ( -SA * CB ) +_
          Z * ( CA * CB )

  IF Preview% THEN CIRCLE ((VX * 40) + 365 , 50 - (VY * 48)) , 2
  Simple.Obj 1, %SPHERE, Loc.X , Loc.Y, Loc.Z, Scale

  X = X1
  Y = Y1
  VX = X * CA + Y * CB + Z * CG
  VY = X * SA + Y * SB + Z * SG

NEXT

END SUB