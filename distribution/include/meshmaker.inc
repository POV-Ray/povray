// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence of Vision Ray Tracer Include File
// File: meshmaker.inc
// Vers: 3.7
// 
// Desc: Macros and functions used in builing mesh2 objects.
// Packed together and updated for POV-Ray 3.7 
// by Friedrich A. Lohmueller, April-2013.
// Original author: Ingo Janssen 
// Date: 2002/04/27
// Rev 2002/10/23 : Added the CheckFileName macro.
//                  Added the option to write Wavefront *.obj files.
//                  Added the option to write *.pcm files, for Chris Colefax' Compressed Mesh Macros.
//                  Added the option to write *.arr files, this writes only the arrays to a file.
// Added SweepSpline1() and SweepSpline2() by Mike Williams, based on an idea by Greg M. Johnson.
// Rev 2015/08/01 : Fixed the position of the #version statement.



#ifndef(makemesh_Inc_Temp)
#declare makemesh_Inc_Temp = version;
#version 3.5;

#ifdef(View_POV_Include_Stack)
   #debug "including makemesh.inc\n"
#end


//====== Macros and Functions ======//

/*==============
LInterpolate() : Linear interpolation of a vector or float between Min and Max.
Min : minimal float value or vector.
Max : Maximal float value or vector.
Val : A float in the range 0 - 1.
*/   
   #macro LInterpolate(Val, Min, Max)
      (Min+(Max-Min)*Val) 
   #end


/*=========
RangeMM() : Adjusts input values in the range [RMin, RMax] to fit in the range
[Min, Max].
Val : A float value in the range [Rmin, Rmax].
*/   
   #declare RangeMM=function(Val,Rmin,Rmax,Min,Max){
      (((Val-Rmin)/(Rmax-Rmin))*(Max-Min)+Min)
   }

/*=================
  If Return has a value of 0 the mesh will not be build,
  but it will be parsed from file. 
*/
#macro CheckFileName(FileName)
   #local Len=strlen(FileName);
   #if(Len>0)
      #if(file_exists(FileName))
         #if(Len>=4)
            #local Ext=strlwr(substr(FileName,Len-3,4))
            #if (strcmp(Ext,".obj")=0 | strcmp(Ext,".pcm")=0 | strcmp(Ext,".arr")=0)
               #local Return=99;
            #else
               #local Return=0;
            #end  
         #else
            #local Return=0;
         #end
      #else
         #if(Len>=4)
            #local Ext=strlwr(substr(FileName,Len-3,4))
            #if (strcmp(Ext,".obj")=0 | strcmp(Ext,".pcm")=0 | strcmp(Ext,".arr")=0)
               #if (strcmp(Ext,".obj")=0)
                  #local Return=2;
               #end
               #if (strcmp(Ext,".pcm")=0)
                  #local Return=3;
               #end
               #if (strcmp(Ext,".arr")=0)
                  #local Return=4;
               #end
            #else
               #local Return=1;
            #end  
         #else
            #local Return=1;
         #end
      #end
   #else
      #local Return=1;
   #end
   (Return)
#end

/*================= 
BuildWriteMesh2() : Builds and optionally writes a mesh2 object based on 3 input
arrays, the number of quads in U and V direction and a filename.
VecArr   : The array that contains the vertices of the triangles in the mesh.
NormArr  : The array with the normal vectors that go with the vertices.
UVArr    : The array containing the uv_vectors.
U        : The amount of subdivisions of the surface in the u-direction.
V        : The amount of subdivisions in the v-direction.
           Based on the U and V values the face_indices of the  triangles in the
           mesh are calculated.
FileName : The name of the file to which the mesh will be written. If is an
           empty string (""), no file will be written.
           If the file extension is 'obj' a Wavefront objectfile will be written.
           If the extension is 'pcm' a compressed mesh file is written.
           If a file name is given, the macro will first check if it already exists.
           If that is so, it will try to parse the existing file unless it's a '*.obj',
           '*.pcm' or '*.arr' file as POV-Ray can not read them directly. In this case a new
           mesh will be generated, but the existing files will _not_ be over-written.
*/
   #macro BuildWriteMesh2(VecArr, NormArr, UVArr, U, V, FileName)
      #if(strlen(FileName)!=0)
         #local Write=CheckFileName(FileName);
         #if(Write=99)
            #local Write=0;
         #end
         #if (Write=0)
            #debug concat(
               "\n\n The exis","ting file: '", FileName ,"' will not be over-written\n",
               " The mesh2 will not be parsed from the ", FileName," file",
               "\n   - vertex_vectors\n")   
         #else
            #debug concat(
               "\n\n Building mesh2 & Writing file: '", FileName , 
               "'\n   - vertex_vectors\n"
            )
            #fopen MeshFile FileName write
            #switch (Write)
               #case(1)
                  #write(
                     MeshFile,
                     "#declare Surface = mesh2 {\n"
                  )
               #break
               #case(2)
                  #write(
                     MeshFile,
                     "# File: ",FileName,"\n",
                  )
               #break
               #case(3)
                  #write(
                     MeshFile,
                     "\"PCM1\",\n"
                  )
               #break
               #case(4)
                  #write(
                     MeshFile,
                     "// Arrays for building a mesh2{} object.\n",
                     "// the arrays are declared with the following names:\n",
                     "// VertexVectors, NormalVectors, UVVectors and FaceIndices.\n\n"
                  )
               #break
            #end
         #end
      #else
         #local Write=0;
         #debug concat("\n\n Building mesh2: \n   - vertex_vectors\n")   
      #end
     
      #local NumVertices=dimension_size(VecArr,1);
      #switch (Write)
         #case(1)
            #write(
               MeshFile,
               "  vertex_vectors {\n",
               "    ", str(NumVertices,0,0),"\n    "
            )
         #break
         #case(2)
            #write(
               MeshFile,
               "# Vertices: ",str(NumVertices,0,0),"\n"
            )
         #break
         #case(3)
            #write(
               MeshFile,
               str(2*NumVertices,0,0),",\n"
            )
         #break
         #case(4)
            #write(
               MeshFile,
               "#declare VertexVectors= array[",str(NumVertices,0,0),"] {\n  "
            )
         #break
      #end
      mesh2 {
         vertex_vectors {
            NumVertices
            #local I=0;
            #while (I<NumVertices)
               VecArr[I]
               #switch(Write)
                  #case(1)
                     #write(MeshFile, VecArr[I])
                  #break
                  #case(2)
                     #write(
                        MeshFile,
                        "v ", VecArr[I].x," ", VecArr[I].y," ", VecArr[I].z,"\n"
                     )
                  #break
                  #case(3)
                     #write(
                        MeshFile,
                        VecArr[I].x,",", VecArr[I].y,",", VecArr[I].z,",\n"
                     )
                  #break
                  #case(4)
                     #write(MeshFile, VecArr[I])
                  #break
               #end
               #local I=I+1;
               #if(Write=1 | Write=4)
                  #if(mod(I,3)=0)
                     #write(MeshFile,"\n    ")
                  #end
               #end 
            #end
            #switch(Write)
               #case(1) 
                  #write(MeshFile,"\n  }\n")
               #break
               #case(2)
                  #write(MeshFile,"\n")
               #break
               #case(3)
                  // do nothing
               #break
               #case(4) 
                  #write(MeshFile,"\n}\n")
               #break
            #end
         }
   
         #debug concat("   - normal_vectors\n")     
         #local NumVertices=dimension_size(NormArr,1);
         #switch(Write)
            #case(1)
               #write(
                  MeshFile,
                  "  normal_vectors {\n",
                  "    ", str(NumVertices,0,0),"\n    "
               )
            #break
            #case(2)
               #write(
                  MeshFile,
                  "# Normals: ",str(NumVertices,0,0),"\n"
               )
            #break
            #case(3)
               // do nothing
            #break
            #case(4)
               #write(
                  MeshFile,
                  "#declare NormalVectors= array[",str(NumVertices,0,0),"] {\n  "
               )
            #break
         #end
         normal_vectors {
            NumVertices
            #local I=0;
            #while (I<NumVertices)
               NormArr[I]
               #switch(Write)
                  #case(1)
                     #write(MeshFile NormArr[I])
                  #break
                  #case(2)
                     #write(
                        MeshFile,
                        "vn ", NormArr[I].x," ", NormArr[I].y," ", NormArr[I].z,"\n"
                     )
                  #break
                  #case(3)
                     #write(
                        MeshFile,
                        NormArr[I].x,",", NormArr[I].y,",", NormArr[I].z,",\n"
                     )
                  #break
                  #case(4)
                     #write(MeshFile NormArr[I])
                  #break
               #end
               #local I=I+1;
               #if(Write=1 | Write=4) 
                  #if(mod(I,3)=0)
                     #write(MeshFile,"\n    ")
                  #end
               #end 
            #end
            #switch(Write)
               #case(1)
                  #write(MeshFile,"\n  }\n")
               #break
               #case(2)
                  #write(MeshFile,"\n")
               #break
               #case(3)
                  //do nothing
               #break
               #case(4)
                  #write(MeshFile,"\n}\n")
               #break
            #end
         }
         
         #debug concat("   - uv_vectors\n")   
         #local NumVertices=dimension_size(UVArr,1);
         #switch(Write)
            #case(1)
               #write(
                  MeshFile, 
                  "  uv_vectors {\n",
                  "    ", str(NumVertices,0,0),"\n    "
               )
             #break
             #case(2)
               #write(
                  MeshFile,
                  "# UV-vectors: ",str(NumVertices,0,0),"\n"
               )
             #break
             #case(3)
               // do nothing, *.pcm does not support uv-vectors
             #break
             #case(4)
                #write(
                   MeshFile,
                   "#declare UVVectors= array[",str(NumVertices,0,0),"] {\n  "
                )
             #break
         #end
         uv_vectors {
            NumVertices
            #local I=0;
            #while (I<NumVertices)
               UVArr[I]
               #switch(Write)
                  #case(1)
                     #write(MeshFile UVArr[I])
                  #break
                  #case(2)
                     #write(
                        MeshFile,
                        "vt ", UVArr[I].u," ", UVArr[I].v,"\n"
                     )
                  #break
                  #case(3)
                     //do nothing
                  #break
                  #case(4)
                     #write(MeshFile UVArr[I])
                  #break
               #end
               #local I=I+1; 
               #if(Write=1 | Write=4)
                  #if(mod(I,3)=0)
                     #write(MeshFile,"\n    ")
                  #end 
               #end
            #end 
            #switch(Write)
               #case(1)
                  #write(MeshFile,"\n  }\n")
               #break
               #case(2)
                  #write(MeshFile,"\n")
               #break
               #case(3)
                  //do nothing
               #break
               #case(4)
                  #write(MeshFile,"\n}\n")
               #break
            #end
         }
   
         #debug concat("   - face_indices\n")   
         #declare NumFaces=U*V*2;
         #switch(Write)
            #case(1)
               #write(
                  MeshFile,
                  "  face_indices {\n"
                  "    ", str(NumFaces,0,0),"\n    "
               )
            #break
            #case(2)
               #write (
                  MeshFile,
                  "# faces: ",str(NumFaces,0,0),"\n"
               )
            #break
            #case(3)
               #write (
                  MeshFile,
                  "0,",str(NumFaces,0,0),",\n"
               )
            #break
            #case(4)
               #write(
                  MeshFile,
                  "#declare FaceIndices= array[",str(NumFaces,0,0),"] {\n  "
               )
            #break
         #end
         face_indices {
            NumFaces
            #local I=0;
            #local H=0;
            #local NumVertices=dimension_size(VecArr,1);
            #while (I<V)
               #local J=0;
               #while (J<U)
                  #local Ind=(I*U)+I+J;
                  <Ind, Ind+1, Ind+U+2>, <Ind, Ind+U+1, Ind+U+2>
                  #switch(Write)
                     #case(1)
                        #write(
                           MeshFile,
                           <Ind, Ind+1, Ind+U+2>, <Ind, Ind+U+1, Ind+U+2>
                        )
                     #break
                     #case(2)
                        #write(
                           MeshFile,
                           "f ",Ind+1,"/",Ind+1,"/",Ind+1," ",Ind+1+1,"/",Ind+1+1,"/",Ind+1+1," ",Ind+U+2+1,"/",Ind+U+2+1,"/",Ind+U+2+1,"\n",
                           "f ",Ind+U+1+1,"/",Ind+U+1+1,"/",Ind+U+1+1," ",Ind+1,"/",Ind+1,"/",Ind+1," ",Ind+U+2+1,"/",Ind+U+2+1,"/",Ind+U+2+1,"\n"
                        )
                     #break
                     #case(3)
                        #write(
                           MeshFile,
                           Ind,",",Ind+NumVertices,",",Ind+1,",",Ind+1+NumVertices,",",Ind+U+2,",",Ind+U+2+NumVertices,",\n"
                           Ind+U+1,",",Ind+U+1+NumVertices,",",Ind,",",Ind+NumVertices,",",Ind+U+2,",",Ind+U+2+NumVertices,",\n"
                        )
                     #break
                     #case(4)
                        #write(
                           MeshFile,
                           <Ind, Ind+1, Ind+U+2>, <Ind, Ind+U+1, Ind+U+2>
                        )
                     #break
                  #end
                  #local J=J+1;
                  #local H=H+1;
                  #if(Write=1 | Write=4)
                     #if(mod(H,3)=0)
                        #write(MeshFile,"\n    ")
                     #end 
                  #end
               #end
               #local I=I+1;
            #end
         }
         #switch(Write)
            #case(1)
               #write(MeshFile, "\n  }\n}")
               #fclose MeshFile
               #debug concat(" Done writing\n")
            #break
            #case(2)
               #fclose MeshFile
               #debug concat(" Done writing\n")
            #break
            #case(3)
               #fclose MeshFile
               #debug concat(" Done writing\n")
            #break
            #case(4)
               #write(MeshFile, "\n}\n}")
               #fclose MeshFile
               #debug concat(" Done writing\n")
            #break
         #end
      }
   #end

//====== End of Macros and Functions ======// former "meshmaker.inc"

// former "msm.inc"

/*======   
The uv_coordinates come from the square <0,0> - <1,1>.

           The spline is evaluated from t=0 to t=1. For the normal calculation,
           it is required that all splines (also linear_spline) have one extra
           point before t=0 and after t=1.
*/
#macro BuildSpline(Arr, SplType)
   #local Ds=dimension_size(Arr,1);
   #local Asc=asc(strupr(SplType));
   #if(Asc!=67 & Asc!=76 & Asc!=81) 
      #local Asc=76;
      #debug "\nWrong spline type defined (C/c/L/l/N/n/Q/q), using default linear_spline\n"
   #end
   spline {
      #switch (Asc)
         #case (67) //C  cubic_spline
            cubic_spline
         #break
         #case (76) //L  linear_spline
            linear_spline
         #break
         #case (78) //N  linear_spline
            natural_spline
         #break
         #case (81) //Q  Quadratic_spline
            quadratic_spline
         #break
      #end
      #local Add=1/((Ds-2)-1);
      #local J=0-Add;
      #local I=0;
      #while (I<Ds)
         J 
         Arr[I]
         #local I=I+1;
         #local J=J+Add;
      #end
   }      
#end

#macro MSM(SplineArray, SplRes, Interp_type,  InterpRes, FileName)
   #declare Build=CheckFileName(FileName);
   #if(Build=0)
      #debug concat("\n Parsing mesh2 from file: ", FileName, "\n")
      #include FileName
      object{Surface}
   #else
      #local NumVertices=(SplRes+1)*(InterpRes+1);
      #local NumFaces=SplRes*InterpRes*2;
      #debug concat("\n Calculating ",str(NumVertices,0,0)," vertices for ", str(NumFaces,0,0)," triangles\n\n")
      #local VecArr=array[NumVertices]
      #local NormArr=array[NumVertices]
      #local UVArr=array[NumVertices]
      #local N=dimension_size(SplineArray,1);
      #local TempSplArr0=array[N];
      #local TempSplArr1=array[N];
      #local TempSplArr2=array[N];
      #local PosStep=1/SplRes;
      #local InterpStep=1/InterpRes;
      #local Count=0;
      #local Pos=0;
      #while(Pos<=1)   
         #local I=0;
         #if (Pos=0)
            #while (I<N)
               #local Spl=spline{SplineArray[I]}
               #local TempSplArr0[I]=<0,0,0>+Spl(Pos);
               #local TempSplArr1[I]=<0,0,0>+Spl(Pos+PosStep);
               #local TempSplArr2[I]=<0,0,0>+Spl(Pos-PosStep);
               #local I=I+1;
            #end
            #local S0=BuildSpline(TempSplArr0, Interp_type)
            #local S1=BuildSpline(TempSplArr1, Interp_type)
            #local S2=BuildSpline(TempSplArr2, Interp_type)
         #else
            #while (I<N)
               #local Spl=spline{SplineArray[I]}
               #local TempSplArr1[I]=<0,0,0>+Spl(Pos+PosStep);
               #local I=I+1;
            #end
            #local S1=BuildSpline(TempSplArr1, Interp_type)
         #end
         #local J=0;
         #while (J<=1)
            #local P0=<0,0,0>+S0(J);
            #local P1=<0,0,0>+S1(J);
            #local P2=<0,0,0>+S2(J);
            #local P3=<0,0,0>+S0(J+InterpStep);
            #local P4=<0,0,0>+S0(J-InterpStep);
            #local B1=P4-P0;
            #local B2=P2-P0;
            #local B3=P3-P0;
            #local B4=P1-P0;
            #local N1=vcross(B1,B2);
            #local N2=vcross(B2,B3);
            #local N3=vcross(B3,B4);
            #local N4=vcross(B4,B1);
            #local Norm=vnormalize((N1+N2+N3+N4));
            #local VecArr[Count]=P0;
            #local NormArr[Count]=Norm;
            #local UVArr[Count]=<J,Pos>;
            #local J=J+InterpStep;
            #local Count=Count+1;
         #end
         #local S2=spline{S0}
         #local S0=spline{S1}
         #debug concat("\r Done ", str(Count,0,0)," vertices : ", str(100*Count/NumVertices,0,2)," %")
         #local Pos=Pos+PosStep;
      #end
      BuildWriteMesh2(VecArr, NormArr, UVArr, InterpRes, SplRes, FileName)
   #end
#end
// ========================== end of former "msm.inc" ======================================

// former param.inc  =======================================================================

#declare EPS=(1e-12);
#declare EPSNorm=(1e-14);

#declare __FU=0; #declare __TU=0;
#declare __FV=0; #declare __TV=0;
#macro FromU(N)#local N=(N+EPS); #declare __FU=1; (N) #end
#macro ToU(N)  #local N=(N-EPS); #declare __TU=1; (N) #end
#macro FromV(N)#local N=(N+EPS); #declare __FV=1; (N) #end
#macro ToV(N)  #local N=(N-EPS); #declare __TV=1; (N) #end

#macro Parametric(__F1__, __F2__, __F3__, UVmin, UVmax, Iter_U, Iter_V, FileName)
   #ifdef(__Fx) #undef __Fx #end
   #ifdef(__Fy) #undef __Fy #end
   #ifdef(__Fz) #undef __Fz #end
   #declare __Fx= function(u,v){__F1__(u,v)} 
   #declare __Fy= function(u,v){__F2__(u,v)}
   #declare __Fz= function(u,v){__F3__(u,v)}
   Paramcalc(UVmin, UVmax, Iter_U, Iter_V, FileName)
#end

#macro Paramcalc(UVmin, UVmax, Iter_U, Iter_V, FileName)
   #declare Build=CheckFileName(FileName);
   #if(Build=0)
      #debug concat("\n Parsing mesh2 from file: ", FileName, "\n")
      #include FileName
      object{Surface}
   #else
      #local Umin=UVmin.u; #local Vmin=UVmin.v;
      #local Umax=UVmax.u; #local Vmax=UVmax.v;
      #local dU=Umax-Umin;
      #local dV=Vmax-Vmin;
      #local iU=dU/Iter_U;
      #local iV=dV/Iter_V;
      #local NumVertices=(Iter_U+1)*(Iter_V+1);
      #local NumFaces=Iter_U*Iter_V*2;
      #debug concat("\n Calculating ",str(NumVertices,0,0)," vertices for ", str(NumFaces,0,0)," triangles\n\n")
      #debug "\n"
      #local VecArr=array[NumVertices] 
      #local NormArr=array[NumVertices] 
      #local UVArr=array[NumVertices]
      #local Count=0;       
      #local I=0;           
      #local V=Vmin-iV;     
      #while (I<Iter_V+1)   
         #local V=V+iV;     
         #local J=0;        
         #local U=Umin-iU;  
         #while (J<Iter_U+1)
            #local U=U+iU;  
            #local P=<(__Fx(U,V)),(__Fy(U,V)),(__Fz(U,V))>;       //     |      |      |        
            #local Un=U+(iU);                                     //  -- x --- Vn ---- x --
            #local Vn=V+(iV);                                     //     |   /  |   /  |   
            #local Um=U-(iU);                                     //     | /    | /    |   
            #local Vm=V-(iV);                                     //  - Um ---- P ---- Un -
            #if(__TU&Un>=Umax)                                    //     |   /  |   /  |   
               #local Un=U+EPSNorm;                               //     | /    | /    |   
            #end                                                  //  -- x --- Vm ---- x --
            #if(__TV&Vn>=Vmax)                                    //     |      |      |   
               #local Vn=V+EPSNorm;
            #end
            #if(__FU&Um<=Umin)
              #local Um=U-EPSNorm;
            #end
            #if(__FV&Vm<=Vmin)         
               #local Vm=V-EPSNorm;
            #end
            #local N1=<(__Fx(Un,V)),(__Fy(Un,V)),(__Fz(Un,V))>;   // Recalculating these points on each pass
            #local N2=<(__Fx(U,Vn)),(__Fy(U,Vn)),(__Fz(U,Vn))>;   // seems to be faster than storing them in,
            #local N3=<(__Fx(Um,V)),(__Fy(Um,V)),(__Fz(Um,V))>;   // and retreiving them from arrays.
            #local N4=<(__Fx(U,Vm)),(__Fy(U,Vm)),(__Fz(U,Vm))>;
            #local A=(N1-P);
            #local B=(N2-P);      
            #local C=(N3-P);
            #local D=(N4-P);
            #local N1=vcross(A,B);
            #local N2=vcross(B,C);
            #local N3=vcross(C,D);
            #local N4=vcross(D,A);
            #local NormArr[Count]=vnormalize(N4+N1+N2+N3); 
            #local VecArr[Count]=P;
            #local UVArr[Count]=<(U-Umin)/dU,(V-Vmin)/dV>;
            #local Count=Count+1;
            #local J=J+1;            
         #end
         #debug concat("\r Done ", str(Count,0,0)," vertices : ",str(100*Count/NumVertices,0,2)," %")
         #local I=I+1;
      #end
      BuildWriteMesh2(VecArr, NormArr, UVArr, Iter_U, Iter_V, FileName)
   #end
   #declare __FU=0;   #declare __TU=0;
   #declare __FV=0;   #declare __TV=0;
   #undef __Fx
   #undef __Fy
   #undef __Fz
#end   

// ====================== end of former "param.inc" ====================================

// former "prism.inc" ==================================================================
/*======
Prism(Spl, ResSpl, Height, HRes, FileName): Extrudes the spline along the y-axis.
           The uv_coordinates come from the square <0,0> - <1,1>.

Spl      : The spline to be extruded. 
           The spline is evaluated from t=0 to t=1. For the normal calculation,
           it is required that all splines (also linear_spline) have one extra
           point before t=0 and after t=1.
ResSpl   : The amount of triangles to be used along the spline
Height   : The amount of POV-units to extrude the shape.
HRes     : The amount of triangles to be used in the height.
FileName : The name of the file to which the mesh will be written. If is an
           empty string (""), no file will be written.
           If the file extension is 'obj' a Wavefront objectfile will be written.
           If the extension is 'pcm' a compressed mesh file is written.
           If a file name is given, the macro will first check if it already exists.
           If that is so, it will try to parse the existing file unless it's a '*.obj',
           '*.pcm' or '*.arr' file as POV-Ray can not read them directly. In this case a new
           mesh will be generated, but the existing files will _not_ be over-written.
*/ 
#macro Prism(Spl, ResSpl, Height, HRes, FileName)
   #local __LinSpl=spline {
      linear_spline
      -1,<0,-1,0>
      1e-50, <0,0,0>
      1,<0,Height,0>
      2,<0,Height+1,0>
   }
   Prism1(Spl, ResSpl, __LinSpl, HRes,FileName)
#end

/*======
Prism1(Spl, ResSpl, PSpl, PRes, FileName): Extrudes the spline along the y-axis.
           In every step the spline is scaled by the 'relative' distance from the
           y-axis of the second spline (PSpl).
           The uv_coordinates come from the square <0,0> - <1,1>.

Spl      : The spline to be extruded.
           The spline is evaluated from t=0 to t=1. For the normal calculation,
           it is required that all splines (also linear_spline) have one extra
           point before t=0 and after t=1.
ResSpl   : The amount of triangles to be used along the spline.
PSpl     : The spline that determines by what amount the extrusion is scaled in
           each step. The scaling is based on the relative distance from the y-axis.
           That is, at t=0 the scale is always 1, so that the start of the shape is 
           identical to the spline Spl.
           PSpl also sets the height of the resulting shape (its y-value at t=1).
           The spline is evaluated from t=0 to t=1. For the normal calculation,
           it is required that all splines (also linear_spline) have one extra
           point before t=0 and after t=1.
FileName : The name of the file to which the mesh will be written. If is an empty
           string (""), no file will be written. If a file name is given, the macro
	        will first check if it already exists. If that is so, it will expect a
	        mesh2 with the name "Surface" and try to parse the existing file.
*/

#macro Prism1(Spl, ResSpl, PSpl, PRes, FileName)
   #declare Build=CheckFileName(FileName);
   #if(Build=0)
      #debug concat("\n Parsing mesh2 from file: ", FileName, "\n")
      #include FileName
      object{Surface}
   #else
      #local NumVertices=(PRes+1)*(ResSpl+1);
      #local NumFaces=PRes*ResSpl*2;
      #debug concat("\n Calculating ",str(NumVertices,0,0)," vertices for ", str(NumFaces,0,0)," triangles\n\n")
      #local VecArr=array[NumVertices]
      #local NormArr=array[NumVertices]
      #local UVArr=array[NumVertices]
      #local H=PSpl(0);
      #local Scale=pow( pow(H.x,2) + pow(H.z,2), 0.5);
      #local H=H.y;
      #local P=Spl(0);
      #local P=pow( pow(P.x,2) + pow(P.z,2), 0.5);
      #local Corr=(P-Scale);
      #local IStep=1/PRes;
      #local JStep=1/ResSpl;
      #local Count=0;
      #local I=0;
      #while(I<=1)
         #local Scale0=PSpl(I);
         #local H0=<0,Scale0.y-H,0>;
         #local Scale1=PSpl(I+IStep);
         #local H1=<0,Scale1.y-H,0>;
         #local Scale2=PSpl(I-IStep);
         #local H2=<0,Scale2.y-H,0>;
         #local Scale0=RangeMM( pow( pow(Scale0.x,2)+ pow(Scale0.z,2), 0.5)+(Corr), 0,P,0,1);
         #local Scale1=RangeMM( pow( pow(Scale1.x,2)+ pow(Scale1.z,2), 0.5)+(Corr), 0,P,0,1);
         #local Scale2=RangeMM( pow( pow(Scale2.x,2)+ pow(Scale2.z,2), 0.5)+(Corr), 0,P,0,1);
         #local J=0;
         #while (J<=1)
            #local P0=(Spl(J)*Scale0)+H0;
            #local P1=(Spl(J)*Scale1)+H1;
            #local P2=(Spl(J)*Scale2)+H2;
            #local P3=(Spl(J+JStep)*Scale0)+H0;
            #local P4=(Spl(J-JStep)*Scale0)+H0;
            #local B1=P4-P0;
            #local B2=P2-P0;
            #local B3=P3-P0;
            #local B4=P1-P0;
            #local N1=vcross(B1,B2);
            #local N2=vcross(B2,B3);
            #local N3=vcross(B3,B4);
            #local N4=vcross(B4,B1);
            #local Norm=vnormalize((N1+N2+N3+N4));
            #local VecArr[Count]=P0;
            #local NormArr[Count]=Norm;
            #local UVArr[Count]=<J,I>;
            #local J=J+JStep;
            #local Count=Count+1;
         #end
         #debug concat("\r Done ", str(Count,0,0)," vertices : ", str(100*Count/NumVertices,0,2)," %")
         #local I=I+IStep;
      #end
      BuildWriteMesh2(VecArr, NormArr, UVArr, ResSpl, PRes, FileName)
   #end
#end
// ======================== end of former "prism.inc" =======================================
 
// former: lathe.inc
/*=======
Lathe(Spl, ResSpl, Rot, ResRot, FileName) : The Lathe  macro generates an
           object by rotating a two-dimensional curve about the y-axis. The
           result is a mesh2 object. The uv_coordinates come from the square
           <0,0> - <1,1>.

Spl      : The spline to be rotated.
           The spline is evaluated from t=0 to t=1. For the normal calculation,
           it is required that all splines (also linear_spline) have one extra
           point before t=0 and after t=1.
ResSpl   : The amount of triangles to be used along the spline
Rot      : The angle the spline has to be rotated.
ResRot   : The amount of triangles to be used in the circumference.
FileName : The name of the file to which the mesh will be written. If is an
           empty string (""), no file will be written.
           If the file extension is 'obj' a Wavefront objectfile will be written.
           If the extension is 'pcm' a compressed mesh file is written.
           If a file name is given, the macro will first check if it already exists.
           If that is so, it will try to parse the existing file unless it's a '*.obj',
           '*.pcm' or '*.arr' file as POV-Ray can not read them directly. In this case a new
           mesh will be generated, but the existing files will _not_ be over-written.
*/ 
#macro Lathe(Spl, ResSpl, Rot, ResRot, FileName)
   #declare Build=CheckFileName(FileName);
   #if(Build=0)
      #debug concat("\n Parsing mesh2 from file: ", FileName, "\n")
      #include FileName
      object{Surface}
   #else
      #local NumVertices=(ResRot+1)*(ResSpl+1);
      #local NumFaces=ResRot*ResSpl*2;
      #debug concat("\n Calculating ",str(NumVertices,0,0)," vertices for ",str(NumFaces,0,0)," triangles\n\n")
      #local I=0;
      #local VNArr=array[ResSpl+1][2]     //retreive the needed amount of points
      #while (I<=ResSpl)                  //from the spline and calculate the
         #local P0=Spl(I/ResSpl);         //normals to go with these points.
         #if (P0.x=0 & P0.z=0)            //put the result in VNArr.
            #local P0=<1e-25,P0.y,1e-25>;
         #end
         #if (I=0)
            #local P1=Spl(((I-0.5)/ResSpl));
            #local P2=Spl(((I+0.5)/ResSpl));
         #else
            #local P1=P2;
            #local P2=0+Spl(((I+0.5)/ResSpl));
         #end
         #local P3=vrotate(P0,<0,1,0>);
         #local P4=vrotate(P0,<0,-1,0>);
         #local B1=P4-P0;
         #local B2=P2-P0;
         #local B3=P3-P0;
         #local B4=P1-P0;
         #local N1=vcross(B1,B2);
         #local N2=vcross(B2,B3);
         #local N3=vcross(B3,B4);
         #local N4=vcross(B4,B1);
         #local Norm=vnormalize((N1+N2+N3+N4)*-1);
         #local VNArr[I][0]=P0;
         #local VNArr[I][1]=Norm;
         #local I=I+1;
      #end
      #local VecArr=array[NumVertices]
      #local NormArr=array[NumVertices]
      #local UVArr=array[NumVertices]
      #local R=Rot/ResRot;
      #local Dim=dimension_size(VNArr,1);
      #local Count=0;
      #local I=0;
      #while (I<=ResRot)
         #local J=0;
         #while (J<Dim)
            #local VecArr[Count]=vrotate(VNArr[J][0],<0,R*I,0>);
            #local NormArr[Count]=vrotate(VNArr[J][1],<0,R*I,0>);
            #local UVArr[Count]=<I/ResRot,J/(Dim-1)>;
            #local J=J+1;
            #local Count=Count+1;
         #end
         #debug concat("\r Done ", str(Count,0,0)," vertices : ",str(100*Count/NumVertices,0,2)," %")
         #local I=I+1;
      #end
      BuildWriteMesh2(VecArr, NormArr, UVArr, ResSpl, ResRot, FileName)
   #end
#end

// ========== end of former "lathe.inc" ===============================================
// former "coons.inc" 
/*======
Coons(Spl1, Spl2, Spl3, Spl4, Iter_U, Iter_V, FileName): Generates a "coons
           surface". The surface is defined by four splines, all attached head
           to tail to the previous / next one.
           The uv_coordinates come from the square <0,0> - <1,1>.

Spl1 - 4 : The four spline that define the surface.
           The splines are evaluated from t=0 to t=1.
Iter_U   : The resolution for the splines 1 and 3.
Iter_V   : The resolution for the splines 2 and 4.
FileName : The name of the file to which the mesh will be written. If is an
           empty string (""), no file will be written.
           If the file extension is 'obj' a Wavefront objectfile will be written.
           If the extension is 'pcm' a compressed mesh file is written.
           If a file name is given, the macro will first check if it already exists.
           If that is so, it will try to parse the existing file unless it's a '*.obj',
           '*.pcm' or '*.arr' file as POV-Ray can not read them directly. In this case a new
           mesh will be generated, but the existing files will _not_ be over-written.
*/

#macro Coons(Spl1, Spl2, Spl3, Spl4, Iter_U, Iter_V, FileName)
   #declare Build=CheckFileName(FileName);
   #if(Build=0)
      #debug concat("\n Parsing mesh2 from file: ", FileName, "\n")
      #include FileName
      object{Surface}
   #else
      #local NumVertices=(Iter_U+1)*(Iter_V+1);
      #local NumFaces=Iter_U*Iter_V*2;
      #debug concat("\n Calculating ", str(NumVertices,0,0), " vertices for ",str(NumFaces,0,0), " triangles\n\n")
      #declare VecArr=array[NumVertices]   
      #declare NormArr=array[NumVertices]   
      #local UVArr=array[NumVertices]      
      #local Spl1_0=Spl1(0);
      #local Spl2_0=Spl2(0);
      #local Spl3_0=Spl3(0);
      #local Spl4_0=Spl4(0);
      #local UStep=1/Iter_U;
      #local VStep=1/Iter_V;
      #local Count=0;
      #local I=0;
      #while (I<=1)
         #local Im=1-I;
         #local J=0;
         #while (J<=1)
            #local Jm=1-J;
            #local C0=Im*Jm*(Spl1_0)+Im*J*(Spl2_0)+I*J*(Spl3_0)+I*Jm*(Spl4_0);
            #local P0=LInterpolate(I, Spl1(J), Spl3(Jm)) + 
               LInterpolate(Jm, Spl2(I), Spl4(Im))-C0;
            #declare VecArr[Count]=P0;
            #local UVArr[Count]=<J,I>;
            #local J=J+UStep;
            #local Count=Count+1;
         #end
         #debug concat(
            "\r Done ", str(Count,0,0)," vertices :         ",
            str(100*Count/NumVertices,0,2)," %"
         )
         #local I=I+VStep;
      #end
      #debug "\r Normals                                  "
      #local Count=0;
      #local I=0;
      #while (I<=Iter_V)
         #local J=0;
         #while (J<=Iter_U)
            #local Ind=(I*Iter_U)+I+J;
            #local P0=VecArr[Ind];
            #if(J=0)
               #local P1=P0+(P0-VecArr[Ind+1]);
            #else
               #local P1=VecArr[Ind-1];
            #end
            #if (J=Iter_U)
               #local P2=P0+(P0-VecArr[Ind-1]);
            #else
               #local P2=VecArr[Ind+1];
            #end
            #if (I=0)
               #local P3=P0+(P0-VecArr[Ind+Iter_U+1]);
            #else
               #local P3=VecArr[Ind-Iter_U-1];                       
            #end
            #if (I=Iter_V)
               #local P4=P0+(P0-VecArr[Ind-Iter_U-1]);
            #else
               #local P4=VecArr[Ind+Iter_U+1];
            #end
            #local B1=P4-P0;
            #local B2=P2-P0;
            #local B3=P3-P0;
            #local B4=P1-P0;
            #local N1=vcross(B1,B2);
            #local N2=vcross(B2,B3);
            #local N3=vcross(B3,B4);
            #local N4=vcross(B4,B1);
            #local Norm=vnormalize((N1+N2+N3+N4));      
            #declare NormArr[Count]=Norm;
            #local J=J+1;
            #local Count=Count+1;
         #end
         #debug concat("\r Done ", str(Count,0,0)," normals : ",str(100*Count/NumVertices,0,2), " %")
         #local I=I+1;
      #end
      BuildWriteMesh2(VecArr, NormArr, UVArr, Iter_U, Iter_V, FileName)
   #end
#end
// ========================= end of former "coons.inc" =============================

// former "twovar.inc"

#macro TwoVarSurf(__Fuv, Urange, Vrange, Iter_U, Iter_V, FileName)
   #declare Build=CheckFileName(FileName);
   #if(Build=0)
      #debug concat("\n Parsing mesh2 from file: ", FileName, "\n")
      #include FileName
      object{Surface}
   #else
      #local Umin=Urange.u;
      #local Umax=Urange.v;
      #local Vmin=Vrange.u;
      #local Vmax=Vrange.v;
      #local dU=Umax-Umin;
      #local dV=Vmax-Vmin;
      #local iU=dU/Iter_U;
      #local iV=dV/Iter_V;
      #local NumVertices=(Iter_U+1)*(Iter_V+1);
      #declare NumFaces=Iter_U*Iter_V*2;
      #debug concat("\n Calculating ",str(NumVertices,0,0)," vertices for ", str(NumFaces,0,0)," triangles\n\n")
      #local VecArr=array[NumVertices] 
      #local NormArr=array[NumVertices] 
      #local UVArr=array[NumVertices]
      #local Count=0;   
      #local I=0;  
      #while (I<Iter_V+1)
         #local V=RangeMM(I,0,Iter_V,Vmin,Vmax);
         #local J=0;
         #while (J<Iter_U+1)
            #local U=RangeMM(J,0,Iter_U,Umin,Umax);
            #if(J=0)
               #local P0=<U,V,__Fuv(U,V)>;
               #local P2=<U-iU,V,__Fuv(U-iU,V)>;
            #else
               #local P0=P1;
               #local P2=P0;
            #end
            #local P1=<U+iU,V,__Fuv(U+iU,V)>;
            #local P3=<U,V+iV,__Fuv(U,V+iV)>;
            #local P4=<U,V-iV,__Fuv(U,V-iV)>;
            #local B1=P4-P0;
            #local B2=P2-P0;
            #local B3=P3-P0;
            #local B4=P1-P0;
            #local N1=vcross(B1,B2);
            #local N2=vcross(B2,B3);
            #local N3=vcross(B3,B4);
            #local N4=vcross(B4,B1);
            #local Norm=vnormalize((N1+N2+N3+N4));
            #local VecArr[Count]=P0;  
            #local NormArr[Count]=Norm;  
            #local UVArr[Count]=<(U-Umin)/dU,(V-Vmin)/dV>;
            #local Count=Count+1;
            #local J=J+1;            
         #end
         #debug concat("\r Done ", str(Count,0,0)," vertices : ",str(100*Count/NumVertices,0,2)," %")
         #local I=I+1;
      #end
      BuildWriteMesh2(VecArr, NormArr, UVArr, Iter_U, Iter_V, FileName)
   #end
#end

// =============== end of former "twovar.inc"



// Persistence of Vision Ray Tracer Include File
// File: SweepSpline.inc
// For POV Version: 3.5
// Desc: Macro sweeping one spline along another
// SweepSpline Version: 3
// Date: 1-Mar-2004
// Auth: Mike Williams
// Based on an idea by Greg M. Johnson
// Requires makemesh.inc by Ingo Janssen 
//
// Parameters
// Track        A spline that specifies the path along the object
//              The section of the spline between control points 0 and 1 will be used
// Shape        A spline that describes the cross section 
//              The section of the spline between control points 0 and 1 will be used
// Waist        A spline with x coordinates that specify the radius of the spheresweep
//              The section of the spline between control points 0 and 1 will be used
// U            The number of vertices along the path
// V            The number of vertices around the circumpherence
// FileName     The name of the file to which the mesh will be written. If is an
//              empty string (""), no file will be written. If a file name is given,
//              the macro will first check if it already exists. If that is so, it
//              will expect a mesh2 with the name "Surface" and try to parse the
//              existing file.
//#include "meshmaker.inc" //former: #include "makemesh.inc"

#macro FindPoint(su,sv)

     // Spline point and radius 
     #local P = Track(su);
     #local W = vlength(Waist(su).x);

     // Vertex position
     // To prevent flipping, calculate from the previous DRY vector 
     #local DRX = W*vnormalize(vcross(DR,DRY));
     #local DRY = W*vnormalize(vcross(DRX,DR));
     P + (Shape(sv).x)*DRX + (Shape(sv).y)*DRY;

#end


#macro SweepSpline1(Track,Shape,Waist,U,V,Filename)
#if(strlen(Filename)>0)
  #if(file_exists(Filename))
    #debug concat("\n Parsing mesh2 from file: ", Filename, "\n")
    #local Build=0;
    #include Filename
     object{Surface}
  #else
    #local Build=1;
  #end
#else
  #local Build=1;
#end

#if (Build)
#local Verts = array[U*(V+1)]
#local Norms = array[U*(V+1)]
#local UV    = array[U*(V+1)]
                           
// Calculate the Vertexes, Normals and UV arrays
#local DRY = y; // Arbitrary initial vector that X will be perpendicular to
#local uu=0;
#while (uu<U)
  #local vv=0;
  #while (vv<=V)
     // UV information
     #local su = uu/U;
     #local sv = vv/V;
     #local UV[uu*(V+1)+vv] = <su,sv>;

     // Direction the spline is pointing
     #local DR = Track(su+0.001)-Track(su-0.001); 
     
     // Find some points
     #local P = FindPoint(su,sv);
     #local Verts[uu*(V+1)+vv] = P;
     
     #local Pu0=P; 
     #local Pu1 = FindPoint(su+0.001,sv);
     #if (vlength(Pu1-Pu0)=0)
       #local Pu1 = Pu0;
       #local Pu0 = FindPoint(su-0.001,sv);
     #end
     
     #local Pv0=P; 
     #local Pv1 = FindPoint(su,sv+0.001);
     #if (vlength(Pv1-Pv0)=0)
       #local Pv1 = Pv0;
       #local Pv0 = FindPoint(su,sv-0.001);
     #end
                        
     // calculate the normal                        
     #local Norms[uu*(V+1)+vv] = vcross(Pu1-Pu0,Pv1-Pv0);

     #local vv=vv+1;
  #end
  #local uu=uu+1;
#end

  BuildWriteMesh2(Verts, Norms, UV, V, U-1, Filename)

#end
#end // ----------------------------------------------------- end SweepSpline1()


#macro SweepSpline2(Track,Shape,Waist,U,V,Filename)
#if(strlen(Filename)>0)
  #if(file_exists(Filename))
    #debug concat("\n Parsing mesh2 from file: ", Filename, "\n")
    #local Build=0;
    #include Filename
     object{Surface}
  #else
    #local Build=1;
  #end
#else
  #local Build=1;
#end

#if (Build)#local Verts = array[U*(V+1)]
#local Norms = array[U*(V+1)]
#local UV    = array[U*(V+1)]
                           
// Calculate the Vertexes, Normals and UV arrays
#local DRY = y; // Arbitrary initial vector that X will be perpendicular to
#local uu=0;
#while (uu<U)
  #local vv=0;
  #while (vv<=V)
     // UV information
     #local su = uu/U;
     #local sv = vv/V;
     #local UV[uu*(V+1)+vv] = <su,sv>;
     
     // Spline point and radius 
     #local P = Track(su);
     #local W = vlength(Waist(su).x);

     // Direction the spline is pointing
     #local DR = Track(su+0.001)-Track(su-0.001); 

     // Vertex position
     // To prevent flipping, calculate from the previous DRY vector 
     #local DRX = W*vnormalize(vcross(DR,DRY));
     #local DRY = W*vnormalize(vcross(DRX,DR));
     #local Verts[uu*(V+1)+vv] = P + (Shape(sv).x)*DRX + (Shape(sv).y)*DRY;

     // Normal
     #local NP1 = (Shape(sv).x)*DRX + (Shape(sv).y)*DRY; 
     #local NP2 = (Shape(sv+0.001).x)*DRX + (Shape(sv+0.001).y)*DRY;
     #local DS = NP2-NP1;
     #if (vlength(DS) = 0) // can happen at end point of Shape
       #local NP1 = (Shape(sv-0.001).x)*DRX + (Shape(sv-0.001).y)*DRY; 
       #local NP2 = (Shape(sv+0.001).x)*DRX + (Shape(sv+0.001).y)*DRY;
       #local DS = NP2-NP1;
     #end     

     #local Norms[uu*(V+1)+vv] = vcross(DR,DS);

     #local vv=vv+1;
  #end
  #local uu=uu+1;
#end

BuildWriteMesh2(Verts, Norms, UV, V, U-1, Filename)

#end
#end// ----------------------------------------------------- end SweepSpline2()


// ------------------------------------------------------
#version makemesh_Inc_Temp; 
#end //ifndef
