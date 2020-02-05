@echo off
echo Test>allscene\test
if not exist allscene\test md allscene
del allscene\test>nul
del userabor.mes>nul
goto Start
:Start
povray allanim +LPOV3DEMO\ANIM\AMBIENT    AMBIENT    allanim
if EXIST userabor.mes goto End
povray allanim +LPOV3DEMO\ANIM\BOING      BOING1     allanim 
if EXIST userabor.mes goto End
povray allanim +LPOV3DEMO\ANIM\BOING      GLSBNG     allanim 
if EXIST userabor.mes goto End
povray allanim +LPOV3DEMO\ANIM\CAMERA2    CAMERA2    allanim 
if EXIST userabor.mes goto End
povray allanim +LPOV3DEMO\ANIM\DIFFUSE    DIFFUSE    allanim 
if EXIST userabor.mes goto End
povray allanim +LPOV3DEMO\ANIM\FLOAT1     FLOAT1     allanim 
if EXIST userabor.mes goto End
povray allanim +LPOV3DEMO\ANIM\FLOAT2     FLOAT2     allanim 
if EXIST userabor.mes goto End
povray allanim +LPOV3DEMO\ANIM\FLOAT3     FLOAT3     allanim 
if EXIST userabor.mes goto End
povray allanim +LPOV3DEMO\ANIM\FLOAT4     FLOAT4     allanim 
if EXIST userabor.mes goto End
povray allanim +LPOV3DEMO\ANIM\L_O        L_O        allanim
if EXIST userabor.mes goto End
povray allanim +LPOV3DEMO\ANIM\PENTMAP    PENTMAP    allanim +h80 
if EXIST userabor.mes goto End
povray allanim +LPOV3DEMO\ANIM\QUILTED    PLOTQLT    allanim 
if EXIST userabor.mes goto End
povray allanim +LPOV3DEMO\ANIM\RADDEM     RADDEM     allanim 
if EXIST userabor.mes goto End
povray allanim +LPOV3DEMO\ANIM\REFLECT    REFLECT    allanim 
if EXIST userabor.mes goto End
povray allanim +LPOV3DEMO\ANIM\SLINKY     SLNK       allanim 
if EXIST userabor.mes goto End
povray allanim +LPOV3DEMO\ANIM\SPECLR     SPECLR     allanim 
if EXIST userabor.mes goto End
povray allanim +LPOV3DEMO\ANIM\VECT1      VECT1      allanim 
if EXIST userabor.mes goto End
povray allanim +LPOV3DEMO\ANIM\VECT2      VECT2      allanim 
goto End
:End
