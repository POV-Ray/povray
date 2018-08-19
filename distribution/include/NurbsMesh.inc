// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

/**
 *
 * Author: Jérôme Grimbert
 *
 * Description: macros to get triangles of mesh from UVMeshable object, in particular nurbs
 *
 */

#ifndef ( NurbsMesh_Inc_Temp )
#declare NurbsMesh_Inc_Temp = version;
#version 3.7;

/**
 * Internal macro, generate an uv-mapped triangle (only if it is not a degenerate triangle)
 *
 * -# OO : UVMeshable object
 * -# STEPU : the number of intervals along the U axis, integer
 * -# STEPV : the number of intervals along the V axis, integer
 * -# AX : the u index of first point (A)
 * -# AY : the v index of first point (A)
 * -# BX : the u index of second point (B)
 * -# BY : the v index of second point (B)
 * -# CX : the u index of third point (C)
 * -# CY : the v index of third point (C)
 * -# UVMIN : 2D vector, minimal uv coordinates 
 * -# UVRANGE : 2D vector, add UVMIN to it to get the maximal uv coordinates
 */
#macro NMTriangle( OO,  STEPU, STEPV, AX, AY, BX, BY, CX, CY, UVMIN, UVRANGE )
#local U1= UVMIN.u + AX/STEPU*UVRANGE.u;
#local V1= UVMIN.v + AY/STEPV*UVRANGE.v;
#local U2= UVMIN.u + BX/STEPU*UVRANGE.u;
#local V2= UVMIN.v + BY/STEPV*UVRANGE.v;
#local U3= UVMIN.u + CX/STEPU*UVRANGE.u;
#local V3= UVMIN.v + CY/STEPV*UVRANGE.v;
// uv_vertex takes UVMeshable object and two coordinates in [0;1) range each
#local P1= uv_vertex( OO, U1, V1 );
#local P2= uv_vertex( OO, U2, V2 );
#local P3= uv_vertex( OO, U3, V3 );
#if ( (vlength(P1-P2)>0)
& (vlength(P1-P3)>0)
& (vlength(P2-P3)>0) )
triangle{ P1, P2, P3 uv_vectors <U1,V1>,<U2,V2>,<U3,V3> }
#end
#end

/**
 * internal macro, generate the triangle with increasing v but decreasing u coordinates
 *
 * -# OO : the UVMeshable object
 * -# STEPU : the number of intervals along the U axis, integer
 * -# STEPV : the number of intervals along the V axis, integer
 * -# UU : the u index (from 0 to STEPU), integer
 * -# VV : the v index (from 0 to STEPV), integer
 * -# PP : parity of the node (UU,VV) on the lattice
 * -# UVMIN : 2D vector, minimal uv coordinates 
 * -# UVRANGE : 2D vector, add UVMIN to it to get the maximal uv coordinates
 *
 */
#macro PreviousNMTriangle( OO, STEPU, STEPV, UU, VV, PP, UVMIN, UVRANGE )
#if(VV<STEPV)
#if((UU-1)>=0)
NMTriangle( OO, STEPU, STEPV, UU, VV, UU-1, VV+PP, UU, VV+1, UVMIN, UVRANGE)
#end
#end
#end

/**
 * internal macro, generate the triangle with increasing uv coordinates
 *
 * -# OO : the UVMeshable object
 * -# STEPU : the number of intervals along the U axis, integer
 * -# STEPV : the number of intervals along the V axis, integer
 * -# UU : the u index (from 0 to STEPU), integer
 * -# VV : the v index (from 0 to STEPV), integer
 * -# PP : parity of the node (UU,VV) on the lattice
 * -# UVMIN : 2D vector, minimal uv coordinates 
 * -# UVRANGE : 2D vector, add UVMIN to it to get the maximal uv coordinates
 *
 */
#macro NextNMTriangle( OO, STEPU, STEPV, UU, VV, PP, UVMIN, UVRANGE )
#if(VV<STEPV)
#if (UU< STEPU)
NMTriangle( OO, STEPU, STEPV, UU, VV, UU+1, VV+PP, UU, VV+1, UVMIN, UVRANGE)
#end
#end
#end

/**
 * internal macro, generate a pair of triangles for uv indexes
 *
 * -# OO : the UVMeshable object
 * -# UU : the u index (from 0 to STEPU), integer
 * -# VV : the v index (from 0 to STEPV), integer
 * -# STEPU : the number of intervals along the U axis, integer
 * -# STEPV : the number of intervals along the V axis, integer
 * -# UVMIN : 2D vector, minimal uv coordinates 
 * -# UVRANGE : 2D vector, add UVMIN to it to get the maximal uv coordinates
 *
 */
#macro NMTriangles(OO, UU,VV, STEPU, STEPV, UVMIN, UVRANGE) 
#local Parity = 1-bitwise_and(1, bitwise_xor(UU, VV));
PreviousNMTriangle( OO, STEPU, STEPV, UU, VV, Parity, UVMIN, UVRANGE )
NextNMTriangle( OO, STEPU, STEPV, UU, VV, Parity, UVMIN, UVRANGE )
#end

/**
 * generate a set of triangle from an UVMeshable object (including nurbs)
 * can be use inside a mesh{} block
 *
 * parameters:
 * -# OO : the UVMeshable object
 * -# STEPU : the number of intervals along the U axis, positive integer
 * -# STEPV : the number of intervals along the V axis, positive integer
 */
#macro UVMeshable( OO, STEPU, STEPV )
#local UVMIN = uv_min(OO);
#local UVRANGE = uv_max(OO)-UVMIN;
#for(AA,0,STEPU,1)
#for(BB,0,STEPV,1)
NMTriangles( OO, AA, BB, STEPU, STEPV, UVMIN, UVRANGE)
#end
#end
#end

#version NurbsMesh_Inc_Temp;
#end // NurbsMesh.inc

