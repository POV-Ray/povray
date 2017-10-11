' PLUGINS.BAS for PowerBASIC 3.xx

' Small lib to write Plugins for Denis Oliviers great POVRAY
' modeller POVLAB 3.20 and higher - converted to PowerBASIC by

' Robert Seidel
' seidel@ifk.uni-jena.de

%FALSE = 0
%TRUE  = 1

TYPE Vector
  X AS DOUBLE
  Y AS DOUBLE
  Z AS DOUBLE
END TYPE

DIM Obj.Num  AS SHARED INTEGER
DIM Obj.Name AS SHARED STRING
Obj.Name = "RSPL"

%CYLINDER= 0
%SPHERE  = 1
%CUBE    = 2
%CONE    = 3
%TORUS   = 4
%TUBE    = 5
%PLANEX  = 6
%PLANEY  = 7
%PLANEZ  = 8
%RING    = 14
%DISC    = 15
%PRISM   = 16
%QTUBE   = 17
%CONET   = 18
%PYRAMIDE= 19
%DSPHERE = 35
%QTORE   = 36
%BLOB    = 37
%HFIELD  = 38

%PAS.CSG = 0
%OK.CSG  = 1
%UNION   = 2
%INTERSE = 3
%DIFFERE = 4
%MERGE   = 5
%INVERSE = 6

SUB Make.Vector(V AS Vector, X#, Y#, Z#)
  V.X = X#
  V.Y = Y#
  V.Z = Z#
END SUB

SUB Write.Object(File%,         _' Handle to file
                 Nb%,           _' Object number Nb
                 ObjType%,      _' Object type Type
                 P AS Vector,   _' Aux vector (for some objects)
                 S AS Vector,   _' Vector scale <SX,SY,SZ>
                 R AS Vector,   _' Vector rotate <RX,RY,RZ>
                 T AS Vector,   _' Vector translate <TX,TY,TZ>
                 C%,            _' Color of the object (in modeller)
                 Texture$,      _' String for texture's name
                 Selection%,    _' Bool if object is selected
                 Hidden%,       _' Bool if object is hidden
                 ObjName$)      _' String for object's name

  ? #File%, " "
  ? #File%, "Object "+ REMOVE$(STR$(Nb%) + ":", " ") + " "; ObjType%
  ? #File%, "Object "+ REMOVE$(STR$(Nb%) + ":", " ") + " "; P.X; P.Y; P.Z
  ? #File%, "Object "+ REMOVE$(STR$(Nb%) + ":", " ") + " "; S.X; S.Y; S.Z
  ? #File%, "Object "+ REMOVE$(STR$(Nb%) + ":", " ") + " "; R.X; R.Y; R.Z
  ? #File%, "Object "+ REMOVE$(STR$(Nb%) + ":", " ") + " "; T.X; T.Y; T.Z
  ? #File%, "Object "+ REMOVE$(STR$(Nb%) + ":", " ") + " "; C%
  ? #File%, "Object "+ REMOVE$(STR$(Nb%) + ":", " ") + " "; Texture$
  ? #File%, "Object "+ REMOVE$(STR$(Nb%) + ":", " ") + " "; Selection%
  ? #File%, "Object "+ REMOVE$(STR$(Nb%) + ":", " ") + " "; Hidden%
  ? #File%, "Object "+ REMOVE$(STR$(Nb%) + ":", " ") + " "; ObjName$

END SUB

SUB Simple.Obj (File%, ObjType%, X# , Y#, Z#, Scale#)

  DIM TempTrans AS Vector
  DIM TempScale AS Vector
  DIM TempRotat AS Vector

  Make.Vector TempTrans, X#, Y#, Z#
  Make.Vector TempScale, Scale#, Scale#, Scale#
  Make.Vector TempRotat, 0, 0, 0

  NumStr$ = REMOVE$(STR$(Obj.Num) , " ")
  ObjName$ = Obj.Name + STRING$(5 - LEN(NumStr$) , "0") + NumStr$

  Write.Object File%, Obj.Num, ObjType%, TempRotat, _
               TempScale, TempRotat, TempTrans, 7, "Default", _
               %TRUE, %FALSE, ObjName$

  INCR Obj.Num

END SUB


$IF 0
OPEN "test" FOR OUTPUT AS #1
DIM a AS Vector
  Write.Object 1,1,1,a,a,a,a,1,"a",1,"a"
  Simple.Obj 1, 1,1,1,1,1
$ENDIF
