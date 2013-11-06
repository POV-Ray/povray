// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence of Vision Ray Tracer version 3.6 / 3.7 Include File
// File: makegrass.inc
// --------------------------------------
// Author: Gilles Tran, 1999-2004, http://www.oyonale.com
// former mgrass.pov by Gilles Tran 
// Updated for POV-Ray 3.7: March-2013 
//
// Description: macros for creating grass.
// ================================================
// This file presents 3 macros
// MakeBlade() creates an individual blade of grass
// MakeGrassPatch() creates a patch of grass (mesh)
//     optional with saving the mesh in a text file
// MakePrairie() creates a prairie of grass patches
// ------------------------------------------------

#ifndef( makegrass_Inc_Temp)
#declare makegrass_Inc_Temp=version;
#version 3.6;

#ifdef(View_POV_Include_Stack)
   #debug "including makegrass.inc\n"
#end

//==============================================
// MakeBlade macro
//==============================================
// The MakeBlade macro creates a grass blade with a central fold
// The blade is positionned at posBlade and rotated around the y axis according to segBlade
// Its length is lBlade.
// The blade bends from vector startBend to vector endBend
// The  describes how the curbe bends; low power bend faster
// --------------------------------------
// It first creates an array of points (vertices)
// Then it calculates the normals (optional)
/*
#declare doSmooth = true;       // smooth triangles                           
#declare posBlade = <0,0,0>;    // position of blade
#declare rotBlade = 100;        // rotation of blade around y
#declare segBlade= 20;          // number of blade segments - try low values (5 for instance) for tests
#declare lBlade = 10;           // length of blade
#declare xBladeStart = 1;       // width of blade at start
#declare xBladeEnd = 0.1;       // width of blade at the end
#declare zBladeStart = 0.5;     // depth of blade fold at start
#declare startBend = <0,1,0>;   // bending of blade at start (<0,1,0>=no bending)
#declare vBend = <0,0,1>;      // force bending the blade (<0,1,1> = 45°)
#declare pwBend = 1;           // bending power (how slowly the curve bends)
#declare dofold = false;        // true creates a fold in the blade (twice more triangles)
#declare dofile = false;        // true creates a mesh file
// --------------------------------------
*/

//---------------------------------------------------------------------------------------------------------------
#macro MakeBlade( doSmooth,      //  0 or 1,// smooth triangles
                  posBlade,      // <0,0,0>,// position of blade 
                  rotBlade,      //      00,// rotation of blade around y
                  segBlade,      //      20,// number of blade segments, try low values (5 for instance) for tests
                  lBlade,        //      10,// length of blade
                  xBladeStart,   //       1,// width of blade at start
                  xBladeEnd,     //     0.1,// width of blade at the end
                  zBladeStart,   //     0.5,// depth of blade fold at start
                  startBend,    //<0,1,0.3>,// bending of blade at start (<0,1,0>=no bending)
                  vBend,         // <0,0,1>,// force bending the blade (<0,1,1> = 45°)
                  pwBend,        //       1,// bending power (how slowly the curve bends)
                  dofold,        //  0 or 1,// true or 1 creates a fold in the blade (twice more triangles)
                  dofile         //  0 or 1,// true or 1 creates a mesh file
                ) //--------------------------------------------------------------------------------------------
#local lsegBlade=lBlade/segBlade;
#if (dofold=true)
        #local nI=3;
#else
        #local nI=2;
#end                
#local nJ=segBlade+1;
#local nP=nI*nJ;
#local P=array[nP]
#local N=array[nP]  
#local pBlade=<0,0,0>;              
#local Count=0;                                       
#local xBlade=xBladeStart;
#local zBlade=zBladeStart;
#if (dofold=true)
        #local P[0]=vaxis_rotate(-x*xBlade,y,rotBlade)+posBlade;
        #local P[1]=vaxis_rotate(z*zBlade,y,rotBlade)+posBlade;
        #local P[2]=vaxis_rotate(x*xBlade,y,rotBlade)+posBlade;
#else
        #local P[0]=vaxis_rotate(-x*xBlade,y,rotBlade)+posBlade;
        #local P[1]=vaxis_rotate(x*xBlade,y,rotBlade)+posBlade;

#end
#local Count=1; 
#local pBlade=vnormalize(startBend)*lsegBlade;
// --------------------------------------
// Fills the array of points
// --------------------------------------
#while (Count<nJ)          
        #local tBlade=Count/segBlade;
        // This is where the blade shape is defined
        #local pBlade=pBlade+lsegBlade*vnormalize(vnormalize(pBlade)+ vBend*pow(tBlade,pwBend));
        #local xBlade=xBladeStart+tBlade*(xBladeEnd-xBladeStart);
        #if (dofold=true)
                #local zBlade=zBladeStart*(1-tBlade);
                #local P[Count*nI]=vaxis_rotate(pBlade-x*xBlade,y,rotBlade)+posBlade;
                #local P[Count*nI+1]=vaxis_rotate(pBlade+z*zBlade,y,rotBlade)+posBlade;
                #local P[Count*nI+2]=vaxis_rotate(pBlade+x*xBlade,y,rotBlade)+posBlade;
        #else
                #local P[Count*nI]=vaxis_rotate(pBlade-x*xBlade,y,rotBlade)+posBlade;
                #local P[Count*nI+1]=vaxis_rotate(pBlade+x*xBlade,y,rotBlade)+posBlade;
        #end
        #local Count=Count+1;        
#end
// --------------------------------------
// Calculates normals if doSmooth = true
// --------------------------------------
#if (doSmooth=true)
#local N = array[nP] // Normales
#local q=0;
#while (q<nP)
#local i=mod(q,nI);#local j=(q-i)/nI;
#local V0 = q-nI-1;#if (i=0) #local V0=-1; #end
#local V1 = q-nI;
#local V2 = q-nI+1;#if (i=nI-1) #local V2=-1; #end
#local V3 = q-1;#if (i=0) #local V3=-1; #end
#local V4 = q;
#local V5 = q+1;#if (i=nI-1) #local V5=-1; #end
#local V6 = q+nI-1;#if (i=0) #local V6=-1; #end
#local V7 = q+nI;
#local V8 = q+nI+1;#if (i=nI-1) #local V8=-1; #end

#if (j=0) #local V0=-1;#local V1=-1;#local V2=-1;#end
#if (j=nJ-1) #local V6=-1;#local V7=-1;#local V8=-1; #end

#local N[q]=<0,0,0>;
#local k=0;
#if (V5>-1 & V8>-1) #local N[q]=N[q]+vcross(P[V4]-P[V5],P[V8]-P[V4]);#local k=k+1;#end
#if (V2>-1 & V5>-1) #local N[q]=N[q]+vcross(P[V4]-P[V2],P[V5]-P[V4]);#local k=k+1;#end
#if (V1>-1 & V2>-1) #local N[q]=N[q]+vcross(P[V4]-P[V1],P[V2]-P[V4]);#local k=k+1;#end
#if (V0>-1 & V1>-1) #local N[q]=N[q]+vcross(P[V4]-P[V0],P[V1]-P[V4]);#local k=k+1;#end
#if (V3>-1 & V0>-1) #local N[q]=N[q]+vcross(P[V4]-P[V3],P[V0]-P[V4]);#local k=k+1;#end
#if (V6>-1 & V3>-1) #local N[q]=N[q]+vcross(P[V4]-P[V6],P[V3]-P[V4]);#local k=k+1;#end
#if (V7>-1 & V6>-1) #local N[q]=N[q]+vcross(P[V4]-P[V7],P[V6]-P[V4]);#local k=k+1;#end
#if (V8>-1 & V7>-1) #local N[q]=N[q]+vcross(P[V4]-P[V8],P[V7]-P[V4]);#local k=k+1;#end
#local N[q]=N[q]/k;
#local q=q+1;#end
#end
// --------------------------------------
// Writes the triangles
// --------------------------------------

#local q=0;
#while (q<(nI*(nJ-1)-1))
#local i=mod(q,nI);#local j=(q-i)/nI;
#if (i <nI-1)
    #if (doSmooth=true)
        #if (dofile=true)
            #write(filehandle,"smooth_triangle{<",P[q].x,",",P[q].y,",",P[q].z,">,<",N[q].x,",",N[q].y,",",N[q].z,">,<",P[q+1].x,",",P[q+1].y,",",P[q+1].z,">,<",N[q+1].x,",",N[q+1].y,",",N[q+1].z,">,<",P[q+nI+1].x,",",P[q+nI+1].y,",",P[q+nI+1].z,">,<",N[q+nI+1].x,",",N[q+nI+1].y,",",N[q+nI+1].z,">}\n")
            #write(filehandle,"smooth_triangle{<",P[q].x,",",P[q].y,",",P[q].z,">,<",N[q].x,",",N[q].y,",",N[q].z,">,<",P[q+nI].x,",",P[q+nI].y,",",P[q+nI].z,">,<",N[q+nI].x,",",N[q+nI].y,",",N[q+nI].z,">,<",P[q+nI+1].x,",",P[q+nI+1].y,",",P[q+nI+1].z,">,<",N[q+nI+1].x,",",N[q+nI+1].y,",",N[q+nI+1].z,">}\n")
         #else
            smooth_triangle{P[q],N[q],P[q+1],N[q+1],P[q+nI+1],N[q+nI+1]}
            smooth_triangle{P[q],N[q],P[q+nI],N[q+nI],P[q+nI+1],N[q+nI+1]}
         #end

    #else        
         #if (dofile=true)
             #write(filehandle,"triangle{<",P[q].x,",",P[q].y,",",P[q].z,">,<",P[q+1].x,",",P[q+1].y,",",P[q+1].z,">,<",P[q+nI+1].x,",",P[q+nI+1].y,",",P[q+nI+1].z,">}\n")
             #write(filehandle,"triangle{<",P[q].x,",",P[q].y,",",P[q].z,">,<",P[q+nI].x,",",P[q+nI].y,",",P[q+nI].z,">,<",P[q+nI+1].x,",",P[q+nI+1].y,",",P[q+nI+1].z,">}\n")
         #else                        
             triangle{P[q],P[q+1],P[q+nI+1]}
             triangle{P[q],P[q+nI],P[q+nI+1]}
         #end
    #end       
#end
#local q=q+1;#end

#end
// --------------------------------------
// End of the MakeBlade macro
//==============================================

//==============================================
// MakeGrassPatch macro
//==============================================
// The MakeGrassPatch macro creates a grass patch by creating
// individual blades with the MakeBlade macro
// The resulting patch is a mesh of nBlade*nBlade individual blades
// Its size is lPatch*lPatch                    
// The main tuning parameters are segBlade and nBlade (keep low to test) 
// --------------------------------------
// Patch parameters
// --------------------------------------
/*
#declare lPatch=50;               // size of patch
#declare nBlade=40;               // number of blades per line
#declare ryBlade = 60;            // initial y rotation of blade
#declare segBlade= 15;            // number of blade segments
#declare lBlade = 15;             // length of blade
#declare wBlade = 1;              // width of blade at start
#declare wBladeEnd = 0.3;         // width of blade at the end
#declare doSmooth=false;          // true makes smooth triangles
#declare startBend = <0,1,0.3>;   // bending of blade at start (<0,1,0>=no bending)
#declare vBend = <0,-1,2>;       // direction of the force bending the blade (<0,1,1> = 45°)
#declare pwBend = 3;             // bending power (how slowly the curve bends)
#declare rd = 459;                // seed
#declare stdposBlade = 1;         // standard deviation of blade position 0..1
#declare stdrotBlade = 360;       // standard deviation of rotation
#declare stdBlade = 3;           // standard deviation of blade scale;
#declare stdBend = 0;             // standard deviation of blade bending;
#declare dofold = false;          // true creates a central fold in the blade (twice more triangles)
#declare dofile = true;           // true creates a mesh file
#declare fname = "fgrass.inc"     // name of the mesh file to create
// --------------------------------------
*/
// -------------------------------------------------------------------------------------------------------
#macro MakeGrassPatch( lPatch,      //   50, // size of patch
                       nBlade,      //   40, // number of blades per line
                       ryBlade,     //   60, // initial y rotation of blade
                       segBlade,    //   15, // number of blade segments
                       lBlade,      //   15, // length of blade
                       wBlade,      //    1, // width of blade at start
                       wBladeEnd,   //  0.3, // width of blade at the end

                       doSmooth,  // 0 or 1, // true or 1 makes smooth triangles   
                       startBend,//<0,1,0.3>,// bending of blade at start (<0,1,0>=no bending)
                       vBend,    //<0,-1,2>, // direction of the force bending the blade (<0,1,1> = 45°)
                       pwBend,      //    3, // bending power (how slowly the curve bends)
                       rd,          //18264, // random seed
                       stdposBlade, // 0..1, // standard deviation of blade position
                       stdrotBlade, //  360, // standard deviation of rotation
                       stdBlade,    //    3, // standard deviation of blade scale
                       stdBend,     //    0, // standard deviation of blade bending
                       dofold,     //0 or 1, // true or 1 creates a central fold in the blade (twice more triangles)
                       dofile,     //0 or 1, // true or 1 creates a mesh file
                       fname  // "grass_01.inc" // string, name of the mesh file to create
                     ) //-------------------------------------------------------------------------------
#if(dofile=true)          
        #warning concat(fname," mesh file creation starts\n")

        #fopen filehandle fname write  // creates the leaf mesh (individual leaf)
        #write(filehandle,"mesh{\n")
        #else
        mesh{
#end
#local iBlade=lPatch/(nBlade-1);
#local s1=seed(rd);
#local zCount=0;
#while (zCount<nBlade)
        #local xCount=0;                          
        
        #while (xCount<nBlade)                                          

                #local posBlade=<xCount*iBlade,0,zCount*iBlade>+<iBlade*(0.5-rand(s1))*stdposBlade,0,iBlade*(0.5-rand(s1))*stdposBlade>;
                #local rotBlade=ryBlade+(0.5-rand(s1))*stdrotBlade;
//                #local lBladetmp=max(lBlade*0.3,lBlade+(0.5-rand(s1))*stdlBlade);     
                #local scBlade=max(0.5,(0.5-rand(s1))*stdBlade*2);
                #local lBladetmp=lBlade*scBlade;     
                #local xBladeStart=wBlade*scBlade;
                #local xBladeEnd=wBladeEnd*scBlade;
                #local zBladeStart=xBladeStart*0.5;
                #local vBendtmp=vBend + <(0.5-rand(s1))*0.3,0.5-rand(s1),rand(s1)>*stdBend;
                MakeBlade(doSmooth,posBlade,rotBlade,segBlade,lBladetmp,xBladeStart,xBladeEnd,zBladeStart,startBend,vBendtmp,pwBend,dofold,dofile)
                #warning concat("blade ",str(zCount*nBlade+xCount+1,0,0),"/",str(nBlade*nBlade,0,0),"\n")

                #local xCount=xCount+1;
        #end
        #local zCount=zCount+1;
#end

#if(dofile = true)
        #write (filehandle,"translate <",-lPatch/2,",0,",-lPatch/2,">}\n")
        #fclose filehandle
        #warning concat(fname," file created\n")
#else
        translate <-lPatch/2,0,-lPatch/2>
        }
        
#end

#end
// --------------------------------------
// End of the MakeGrassPatch macro
//==============================================


//===============================================
// MakePrairie macro
// --------------------------------------
// The MakePrairie macro creates a prairie by collating patches
// along the z axis, starting with nxPrairie patches and adding addPatches patches
// at each line, so that the prairie gets wider
// Because the patch is a mesh object, you can have multiple copies of it without
// running out of memory too soon
// --------------------------------------
/*
#declare lPatch=50; //size of individual patches
#declare nxPrairie=3; //number of patches for the first line
#declare addPatches=1; //number of patches to add at each line
#declare nzPrairie=5; //number of lines of patches
#declare objectPatch=GrassPatch; //patch object
#declare rd=seed(779);  // random seed
#declare stdscale=1; // stddev of scale
#declare stdrotate=1; // stddev of rotation
#declare doTest=false; // replaces the patch with a sphere
// --------------------------------------
*/
//-----------------------------------------------------------------------------------
#macro MakePrairie( lPatch,     // 50, // size of individual patches
                    nxPrairie,  //  3, // number of patches for the first line
                    addPatches, //  1, // number of patches to add at each line
                    nzPrairie,  //  5, // number of lines of patches
                    objectPatch,// GrassPatch, // the name of the patch object
                    rd,         //779, // random seed
                    stdscale,   //  1, // stddev of scale
                    stdrotate,  //  1, // stddev of rotation
                    doTest      //  0 or 1, // replaces the patch with a sphere
                  ) //---------------------------------------------------------------
union{
#local zCount=0;
#local nxCounttmp=nxPrairie;
#local s2=seed(rd);
#while (zCount<nzPrairie)                            
        #local xCount=0;
        
        #while (xCount<nxCounttmp)
                
                #if (doTest=true)
//                        sphere{<-nxCounttmp*0.5+xCount,0,zCount>*lPatch,lPatch*0.5 pigment{Red} scale <1,0.2,1>}
                        sphere{0,lPatch*0.5 pigment{Red} scale <1,0.2,1>
                        scale (1+stdscale*rand(s2)) rotate y*360*(0.5-rand(rd))*stdrotate 
                        translate <xCount-(nxCounttmp-1)*0.5,0,zCount>*lPatch}
                #else
                        object{ objectPatch 
                                scale (1+stdscale*rand(s2)) 
                                rotate y*360*(0.5-rand(s2))*stdrotate 
                                translate <-(nxCounttmp-1)*0.5+xCount,0,zCount>*lPatch}
                #end
                #local xCount=xCount+1;
        #end
        #local nxCounttmp=nxCounttmp+addPatches;
        #local zCount=zCount+1;
#end
}
#end

//-----------------------------------------------
// End of MakePrairie macro
//===============================================

#version makegrass_Inc_Temp;
#end
// End of "makegrass.inc" -----------------------
