/* $Id: vpstate.c,v 1.12 2002/10/25 21:06:33 brianp Exp $ */

/*
 * Mesa 3-D graphics library
 * Version:  4.1
 *
 * Copyright (C) 1999-2001  Brian Paul   All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * BRIAN PAUL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * -------- Regarding NV_vertex_program --------
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * o Redistribution of the source code must contain a copyright notice
 *   and this list of conditions;
 * 
 * o Redistribution in binary and source code form must contain the
 *   following Notice in the software and any documentation and/or other
 *   materials provided with the distribution; and
 * 
 * o The name of Nvidia may not be used to promote or endorse software
 *   derived from the software.
 * 
 * NOTICE: Nvidia hereby grants to each recipient a non-exclusive worldwide
 * royalty free patent license under patent claims that are licensable by
 * Nvidia and which are necessarily required and for which no commercially
 * viable non infringing alternative exists to make, use, sell, offer to sell,
 * import and otherwise transfer the vertex extension for the Mesa 3D Graphics
 * Library as distributed in source code and object code form.  No hardware or
 * hardware implementation (including a semiconductor implementation and chips)
 * are licensed hereunder. If a recipient makes a patent claim or institutes
 * patent litigation against Nvidia or Nvidia's customers for use or sale of
 * Nvidia products, then this license grant as to such recipient shall
 * immediately terminate and recipient immediately agrees to cease use and
 * distribution of the Mesa Program and derivatives thereof. 
 * 
 * THE MESA 3D GRAPHICS LIBRARY IS PROVIDED ON AN "AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED INCLUDING,
 * WITHOUT LIMITATION, ANY WARRANTIES OR CONDITIONS OF TITLE, NON-NFRINGEMENT
 * MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * 
 * NVIDIA SHALL NOT HAVE ANY LIABILITY FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING WITHOUT LIMITATION
 * LOST PROFITS), HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OR DISTRIBUTION OF THE MESA 3D GRAPHICS
 * LIBRARY OR EVIDENCE OR THE EXERCISE OF ANY RIGHTS GRANTED HEREUNDR, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * If you do not comply with this agreement, then Nvidia may cancel the license
 * and rights granted herein.
 * ---------------------------------------------
 */

/**
 * \file vpstate.c
 * \brief Vertex program state management functions (most map to API functions)
 * \author Brian Paul
 */


#include "glheader.h"
#include "context.h"
#include "hash.h"
#include "imports.h"
#include "macros.h"
#include "mtypes.h"
#include "vpexec.h"
#include "vpparse.h"
#include "vpstate.h"


/**
 * Delete a program and remove it from the hash table, ignoring the
 * reference count.
 * \note Called from the GL API dispatcher.
 */
void _mesa_delete_program(GLcontext *ctx, GLuint id)
{
   struct vp_program *vprog = (struct vp_program *)
      _mesa_HashLookup(ctx->Shared->VertexPrograms, id);

   if (vprog) {
      if (vprog->String)
         FREE(vprog->String);
      if (vprog->Instructions)
         FREE(vprog->Instructions);
      _mesa_HashRemove(ctx->Shared->VertexPrograms, id);
      FREE(vprog);
   }
}


/**
 * Bind a program (make it current)
 * \note Called from the GL API dispatcher.
 */
void _mesa_BindProgramNV(GLenum target, GLuint id)
{
   struct vp_program *vprog;
   GET_CURRENT_CONTEXT(ctx);
   ASSERT_OUTSIDE_BEGIN_END(ctx);

   if (target != GL_VERTEX_PROGRAM_NV) {
      _mesa_error(ctx, GL_INVALID_ENUM, "glBindProgramNV");
      return;
   }

   if (id == ctx->VertexProgram.CurrentID)
      return;

   /* decrement refcount on previously bound vertex program */
   if (ctx->VertexProgram.Current) {
      ctx->VertexProgram.Current->RefCount--;
      /* and delete if refcount goes below one */
      if (ctx->VertexProgram.Current->RefCount <= 0)
         _mesa_delete_program(ctx, ctx->VertexProgram.CurrentID);
   }

   /* NOTE: binding to a non-existant program is not an error.
    * That's supposed to be caught in glBegin.
    */
   vprog = (struct vp_program *)
      _mesa_HashLookup(ctx->Shared->VertexPrograms, id);

   if (!vprog && id > 0){
      /* new program ID */
      vprog = CALLOC_STRUCT(vp_program);
      if (!vprog) {
         _mesa_error(ctx, GL_OUT_OF_MEMORY, "glBindProgramNV");
         return;
      }
      vprog->Target = target;
      vprog->Resident = GL_TRUE;
      vprog->RefCount = 1;
      _mesa_HashInsert(ctx->Shared->VertexPrograms, id, vprog);
   }

   ctx->VertexProgram.CurrentID = id;
   ctx->VertexProgram.Current = vprog;
   if (vprog)
      vprog->RefCount++;
}


/**
 * Delete a list of programs.
 * \note Not compiled into display lists.
 * \note Called from the GL API dispatcher.
 */
void _mesa_DeleteProgramsNV(GLsizei n, const GLuint *ids)
{
   GLint i;
   GET_CURRENT_CONTEXT(ctx);
   ASSERT_OUTSIDE_BEGIN_END(ctx);

   if (n < 0) {
      _mesa_error( ctx, GL_INVALID_VALUE, "glDeleteProgramsNV" );
      return;
   }

   for (i = 0; i < n; i++) {
      if (ids[i] != 0) {
         struct vp_program *vprog = (struct vp_program *)
            _mesa_HashLookup(ctx->Shared->VertexPrograms, ids[i]);
         if (ctx->VertexProgram.CurrentID == ids[i]) {
            /* unbind this currently bound program */
            _mesa_BindProgramNV(vprog->Target, 0);
         }
         if (vprog) {
            vprog->RefCount--;
            if (vprog->RefCount <= 0) {
               _mesa_delete_program(ctx, ids[i]);
            }
         }
      }
   }
}


/**
 * Execute a vertex state program.
 * \note Called from the GL API dispatcher.
 */
void _mesa_ExecuteProgramNV(GLenum target, GLuint id, const GLfloat *params)
{
   struct vp_program *vprog;
   GET_CURRENT_CONTEXT(ctx);
   ASSERT_OUTSIDE_BEGIN_END(ctx);

   if (target != GL_VERTEX_STATE_PROGRAM_NV) {
      _mesa_error(ctx, GL_INVALID_ENUM, "glExecuteProgramNV");
      return;
   }

   vprog = (struct vp_program *)
      _mesa_HashLookup(ctx->Shared->VertexPrograms, id);

   if (!vprog || vprog->Target != GL_VERTEX_STATE_PROGRAM_NV) {
      _mesa_error(ctx, GL_INVALID_OPERATION, "glExecuteProgramNV");
      return;
   }
   
   _mesa_init_vp_registers(ctx);
   _mesa_init_tracked_matrices(ctx);
   COPY_4V(ctx->VertexProgram.Machine.Registers[VP_INPUT_REG_START], params);
   _mesa_exec_program(ctx, vprog);
}


/**
 * Generate a list of new program identifiers.
 * \note Not compiled into display lists.
 * \note Called from the GL API dispatcher.
 */
void _mesa_GenProgramsNV(GLsizei n, GLuint *ids)
{
   GLuint first;
   GLuint i;
   GET_CURRENT_CONTEXT(ctx);
   ASSERT_OUTSIDE_BEGIN_END(ctx);

   if (n < 0) {
      _mesa_error(ctx, GL_INVALID_VALUE, "glGenProgramsNV");
      return;
   }

   if (!ids)
      return;

   first = _mesa_HashFindFreeKeyBlock(ctx->Shared->VertexPrograms, n);

   for (i = 0; i < (GLuint) n; i++) {
      struct vp_program *vprog = CALLOC_STRUCT(vp_program);
      if (!vprog) {
         _mesa_error(ctx, GL_OUT_OF_MEMORY, "glGenProgramsNV");
         return;
      }
      vprog->RefCount = 1;
      _mesa_HashInsert(ctx->Shared->VertexPrograms, first + i, vprog);
   }

   /* Return the program names */
   for (i = 0; i < (GLuint) n; i++) {
      ids[i] = first + i;
   }
}


/**
 * Determine if a set of programs is resident in hardware.
 * \note Not compiled into display lists.
 * \note Called from the GL API dispatcher.
 */
GLboolean _mesa_AreProgramsResidentNV(GLsizei n, const GLuint *ids,
                                      GLboolean *residences)
{
   GLint i;
   GLboolean retVal;
   GET_CURRENT_CONTEXT(ctx);
   ASSERT_OUTSIDE_BEGIN_END_WITH_RETVAL(ctx, GL_FALSE);

   if (n < 0) {
      _mesa_error(ctx, GL_INVALID_VALUE, "glAreProgramsResidentNV(n)");
      return GL_FALSE;
   }

   retVal = GL_TRUE;

   for (i = 0; i < n; i++) {
      struct vp_program *vprog;

      if (ids[i] == 0) {
         _mesa_error(ctx, GL_INVALID_VALUE, "glAreProgramsResidentNV(id)");
         return GL_FALSE;
      }

      vprog = (struct vp_program *)
         _mesa_HashLookup(ctx->Shared->VertexPrograms, ids[i]);

      if (!vprog) {
         _mesa_error(ctx, GL_INVALID_VALUE, "glAreProgramsResidentNV(id)");
         return GL_FALSE;
      }

      *residences = vprog->Resident;
      if (!vprog->Resident)
         retVal = GL_FALSE;
   }

   return retVal;
}


/**
 * Request that a set of programs be resident in hardware.
 * \note Called from the GL API dispatcher.
 */
void _mesa_RequestResidentProgramsNV(GLsizei n, const GLuint *ids)
{
   GLint i;
   GET_CURRENT_CONTEXT(ctx);
   ASSERT_OUTSIDE_BEGIN_END(ctx);

   if (n < 0) {
      _mesa_error(ctx, GL_INVALID_VALUE, "glRequestResidentProgramsNV(n)");
      return;
   }

   /* just error checking for now */
   for (i = 0; i < n; i++) {
      struct vp_program *vprog;

      if (ids[i] == 0) {
         _mesa_error(ctx, GL_INVALID_VALUE, "glRequestResidentProgramsNV(id)");
         return;
      }

      vprog = (struct vp_program *)
         _mesa_HashLookup(ctx->Shared->VertexPrograms, ids[i]);

      if (!vprog) {
         _mesa_error(ctx, GL_INVALID_VALUE, "glRequestResidentProgramsNV(id)");
         return;
      }

      vprog->Resident = GL_TRUE;
   }
}


/**
 * Get a program parameter register.
 * \note Not compiled into display lists.
 * \note Called from the GL API dispatcher.
 */
void _mesa_GetProgramParameterfvNV(GLenum target, GLuint index,
                                   GLenum pname, GLfloat *params)
{
   GET_CURRENT_CONTEXT(ctx);
   ASSERT_OUTSIDE_BEGIN_END(ctx);

   if (target != GL_VERTEX_PROGRAM_NV) {
      _mesa_error(ctx, GL_INVALID_ENUM, "glGetProgramParameterfvNV");
      return;
   }

   if (index >= VP_NUM_PROG_REGS) {
      _mesa_error(ctx, GL_INVALID_VALUE, "glGetProgramParameterfvNV");
      return;
   }

   if (pname != GL_PROGRAM_PARAMETER_NV) {
      _mesa_error(ctx, GL_INVALID_ENUM, "glGetProgramParameterfvNV");
      return;
   }

   index += VP_PROG_REG_START;
   COPY_4V(params, ctx->VertexProgram.Machine.Registers[index]);
}


/**
 * Get a program parameter register.
 * \note Not compiled into display lists.
 * \note Called from the GL API dispatcher.
 */
void _mesa_GetProgramParameterdvNV(GLenum target, GLuint index,
                                   GLenum pname, GLdouble *params)
{
   GET_CURRENT_CONTEXT(ctx);
   ASSERT_OUTSIDE_BEGIN_END(ctx);

   if (target != GL_VERTEX_PROGRAM_NV) {
      _mesa_error(ctx, GL_INVALID_ENUM, "glGetProgramParameterfvNV");
      return;
   }

   if (index >= VP_NUM_PROG_REGS) {
      _mesa_error(ctx, GL_INVALID_VALUE, "glGetProgramParameterfvNV");
      return;
   }

   if (pname != GL_PROGRAM_PARAMETER_NV) {
      _mesa_error(ctx, GL_INVALID_ENUM, "glGetProgramParameterfvNV");
      return;
   }

   index += VP_PROG_REG_START;
   COPY_4V(params, ctx->VertexProgram.Machine.Registers[index]);
}


/**
 * Get a program attribute.
 * \note Not compiled into display lists.
 * \note Called from the GL API dispatcher.
 */
void _mesa_GetProgramivNV(GLuint id, GLenum pname, GLint *params)
{
   struct vp_program *vprog;
   GET_CURRENT_CONTEXT(ctx);
   ASSERT_OUTSIDE_BEGIN_END(ctx);

   vprog = (struct vp_program *)
      _mesa_HashLookup(ctx->Shared->VertexPrograms, id);

   if (!vprog) {
      _mesa_error(ctx, GL_INVALID_OPERATION, "glGetProgramivNV");
      return;
   }

   switch (pname) {
      case GL_PROGRAM_TARGET_NV:
         *params = vprog->Target;
         return;
      case GL_PROGRAM_LENGTH_NV:
         *params = vprog->String ? _mesa_strlen((char *) vprog->String) : 0;
         return;
      case GL_PROGRAM_RESIDENT_NV:
         *params = vprog->Resident;
         return;
      default:
         _mesa_error(ctx, GL_INVALID_ENUM, "glGetProgramivNV(pname)");
         return;
   }
}


/**
 * Get the program source code.
 * \note Not compiled into display lists.
 * \note Called from the GL API dispatcher.
 */
void _mesa_GetProgramStringNV(GLuint id, GLenum pname, GLubyte *program)
{
   struct vp_program *vprog;
   GET_CURRENT_CONTEXT(ctx);
   ASSERT_OUTSIDE_BEGIN_END(ctx);

   if (pname != GL_PROGRAM_STRING_NV) {
      _mesa_error(ctx, GL_INVALID_ENUM, "glGetProgramivNV(pname)");
      return;
   }

   vprog = (struct vp_program *)
      _mesa_HashLookup(ctx->Shared->VertexPrograms, id);

   if (!vprog) {
      _mesa_error(ctx, GL_INVALID_OPERATION, "glGetProgramivNV");
      return;
   }

   if (vprog->String) {
      MEMCPY(program, vprog->String, _mesa_strlen((char *) vprog->String));
   }
   else {
      program[0] = 0;
   }
}


/**
 * Get matrix tracking information.
 * \note Not compiled into display lists.
 * \note Called from the GL API dispatcher.
 */
void _mesa_GetTrackMatrixivNV(GLenum target, GLuint address,
                              GLenum pname, GLint *params)
{
   GLuint i;
   GET_CURRENT_CONTEXT(ctx);
   ASSERT_OUTSIDE_BEGIN_END(ctx);

   if (target != GL_VERTEX_PROGRAM_NV) {
      _mesa_error(ctx, GL_INVALID_ENUM, "glGetTrackMatrixivNV");
      return;
   }

   if ((address & 0x3) || address > VP_NUM_PROG_REGS) {
      _mesa_error(ctx, GL_INVALID_VALUE, "glGetTrackMatrixivNV(address)");
      return;
   }

   i = address / 4;

   switch (pname) {
      case GL_TRACK_MATRIX_NV:
         params[0] = (GLint) ctx->VertexProgram.TrackMatrix[i];
         return;
      case GL_TRACK_MATRIX_TRANSFORM_NV:
         params[0] = (GLint) ctx->VertexProgram.TrackMatrixTransform[i];
         return;
      default:
         _mesa_error(ctx, GL_INVALID_ENUM, "glGetTrackMatrixivNV");
         return;
   }
}


/**
 * Get a vertex (or vertex array) attribute.
 * \note Not compiled into display lists.
 * \note Called from the GL API dispatcher.
 */
void _mesa_GetVertexAttribdvNV(GLuint index, GLenum pname, GLdouble *params)
{
   GET_CURRENT_CONTEXT(ctx);
   ASSERT_OUTSIDE_BEGIN_END(ctx);

   if (index == 0 || index >= VP_NUM_INPUT_REGS) {
      _mesa_error(ctx, GL_INVALID_VALUE, "glGetVertexAttribdvNV(index)");
      return;
   }

   switch (pname) {
      case GL_ATTRIB_ARRAY_SIZE_NV:
         params[0] = ctx->Array.VertexAttrib[index].Size;
         break;
      case GL_ATTRIB_ARRAY_STRIDE_NV:
         params[0] = ctx->Array.VertexAttrib[index].Stride;
         break;
      case GL_ATTRIB_ARRAY_TYPE_NV:
         params[0] = ctx->Array.VertexAttrib[index].Type;
         break;
      case GL_CURRENT_ATTRIB_NV:
         COPY_4V(params, ctx->Current.Attrib[index]);
         break;
      default:
         _mesa_error(ctx, GL_INVALID_ENUM, "glGetVertexAttribdvNV");
         return;
   }
}

/**
 * Get a vertex (or vertex array) attribute.
 * \note Not compiled into display lists.
 * \note Called from the GL API dispatcher.
 */
void _mesa_GetVertexAttribfvNV(GLuint index, GLenum pname, GLfloat *params)
{
   GET_CURRENT_CONTEXT(ctx);
   ASSERT_OUTSIDE_BEGIN_END(ctx);

   if (index == 0 || index >= VP_NUM_INPUT_REGS) {
      _mesa_error(ctx, GL_INVALID_VALUE, "glGetVertexAttribdvNV(index)");
      return;
   }

   switch (pname) {
      case GL_ATTRIB_ARRAY_SIZE_NV:
         params[0] = (GLfloat) ctx->Array.VertexAttrib[index].Size;
         break;
      case GL_ATTRIB_ARRAY_STRIDE_NV:
         params[0] = (GLfloat) ctx->Array.VertexAttrib[index].Stride;
         break;
      case GL_ATTRIB_ARRAY_TYPE_NV:
         params[0] = (GLfloat) ctx->Array.VertexAttrib[index].Type;
         break;
      case GL_CURRENT_ATTRIB_NV:
         COPY_4V(params, ctx->Current.Attrib[index]);
         break;
      default:
         _mesa_error(ctx, GL_INVALID_ENUM, "glGetVertexAttribdvNV");
         return;
   }
}

/**
 * Get a vertex (or vertex array) attribute.
 * \note Not compiled into display lists.
 * \note Called from the GL API dispatcher.
 */
void _mesa_GetVertexAttribivNV(GLuint index, GLenum pname, GLint *params)
{
   GET_CURRENT_CONTEXT(ctx);
   ASSERT_OUTSIDE_BEGIN_END(ctx);

   if (index == 0 || index >= VP_NUM_INPUT_REGS) {
      _mesa_error(ctx, GL_INVALID_VALUE, "glGetVertexAttribdvNV(index)");
      return;
   }

   switch (pname) {
      case GL_ATTRIB_ARRAY_SIZE_NV:
         params[0] = ctx->Array.VertexAttrib[index].Size;
         break;
      case GL_ATTRIB_ARRAY_STRIDE_NV:
         params[0] = ctx->Array.VertexAttrib[index].Stride;
         break;
      case GL_ATTRIB_ARRAY_TYPE_NV:
         params[0] = ctx->Array.VertexAttrib[index].Type;
         break;
      case GL_CURRENT_ATTRIB_NV:
         COPY_4V_CAST(params, ctx->Current.Attrib[index], GLint);
         break;
      default:
         _mesa_error(ctx, GL_INVALID_ENUM, "glGetVertexAttribdvNV");
         return;
   }
}


/**
 * Get a vertex array attribute pointer.
 * \note Not compiled into display lists.
 * \note Called from the GL API dispatcher.
 */
void _mesa_GetVertexAttribPointervNV(GLuint index, GLenum pname, GLvoid **pointer)
{
   GET_CURRENT_CONTEXT(ctx);
   ASSERT_OUTSIDE_BEGIN_END(ctx);

   if (index >= VP_NUM_PROG_REGS) {
      _mesa_error(ctx, GL_INVALID_VALUE, "glGetVertexAttribPointerNV(index)");
      return;
   }

   if (pname != GL_ATTRIB_ARRAY_POINTER_NV) {
      _mesa_error(ctx, GL_INVALID_ENUM, "glGetVertexAttribPointerNV(pname)");
      return;
   }

   *pointer = ctx->Array.VertexAttrib[index].Ptr;;
}


/**
 * Determine if id names a program.
 * \note Not compiled into display lists.
 * \note Called from the GL API dispatcher.
 * \param id is the program identifier
 * \return GL_TRUE if id is a program, else GL_FALSE.
 */
GLboolean _mesa_IsProgramNV(GLuint id)
{
   struct vp_program *vprog;
   GET_CURRENT_CONTEXT(ctx);
   ASSERT_OUTSIDE_BEGIN_END_WITH_RETVAL(ctx, GL_FALSE);

   if (id == 0)
      return GL_FALSE;

   vprog = (struct vp_program *)
      _mesa_HashLookup(ctx->Shared->VertexPrograms, id);

   if (vprog && vprog->Target)
      return GL_TRUE;
   else
      return GL_FALSE;
}


/**
 * Load a vertex program.
 * \note Called from the GL API dispatcher.
 */
void _mesa_LoadProgramNV(GLenum target, GLuint id, GLsizei len,
                         const GLubyte *program)
{
   struct vp_program *vprog;
   GLboolean newProgram = GL_FALSE;
   GLubyte *programCopy;
   GET_CURRENT_CONTEXT(ctx);
   ASSERT_OUTSIDE_BEGIN_END(ctx);

   if (id == 0) {
      _mesa_error(ctx, GL_INVALID_VALUE, "glLoadProgramNV(id)");
      return;
   }

   vprog = (struct vp_program *)
      _mesa_HashLookup(ctx->Shared->VertexPrograms, id);

   if (vprog && vprog->Target != 0 && vprog->Target != target) {
      _mesa_error(ctx, GL_INVALID_OPERATION, "glLoadProgramNV(target)");
      return;
   }

   /* make a copy of the program string so that we can null-terminate it */
   /* if we change the parser to stop after <len> characters, instead of */
   /* looking for '\0' we can eliminate this. */
   programCopy = (GLubyte *) MALLOC(len + 1);
   if (!programCopy) {
      _mesa_error(ctx, GL_OUT_OF_MEMORY, "glLoadProgramNV");
      return;
   }
   MEMCPY(programCopy, program, len);
   programCopy[len] = 0;

   if (!vprog) {
      newProgram = GL_TRUE;
      vprog = CALLOC_STRUCT(vp_program);
      if (!vprog) {
         _mesa_error(ctx, GL_OUT_OF_MEMORY, "glLoadProgramNV");
         return;
      }
   }

   _mesa_parse_program(ctx, target, programCopy, vprog);
   if (ctx->VertexProgram.ErrorPos == -1) {
      /* loaded and parsed w/out errors */
      if (newProgram) {
         _mesa_HashInsert(ctx->Shared->VertexPrograms, id, vprog);
      }
      vprog->RefCount = 1;
      vprog->Resident = GL_TRUE;
   }

   FREE(programCopy);
}



/**
 * Set a program parameter register.
 * \note Called from the GL API dispatcher.
 */
void _mesa_ProgramParameter4dNV(GLenum target, GLuint index,
                                GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
   GET_CURRENT_CONTEXT(ctx);
   ASSERT_OUTSIDE_BEGIN_END(ctx);

   if (target != GL_VERTEX_PROGRAM_NV) {
      _mesa_error(ctx, GL_INVALID_ENUM, "glProgramParameter4dNV");
      return;
   }

   if (index >= VP_NUM_PROG_REGS) {
      _mesa_error(ctx, GL_INVALID_VALUE, "glProgramParameter4dNV");
      return;
   }

   index += VP_PROG_REG_START;
   ASSIGN_4V(ctx->VertexProgram.Machine.Registers[index], 
	     (GLfloat)x, (GLfloat)y, (GLfloat)z, (GLfloat)w);
}


/**
 * Set a program parameter register.
 * \note Called from the GL API dispatcher.
 */
void _mesa_ProgramParameter4dvNV(GLenum target, GLuint index,
                                 const GLdouble *params)
{
   GET_CURRENT_CONTEXT(ctx);
   ASSERT_OUTSIDE_BEGIN_END(ctx);

   if (target != GL_VERTEX_PROGRAM_NV) {
      _mesa_error(ctx, GL_INVALID_ENUM, "glProgramParameter4dvNV");
      return;
   }

   if (index >= VP_NUM_PROG_REGS) {
      _mesa_error(ctx, GL_INVALID_VALUE, "glProgramParameter4dvNV");
      return;
   }

   index += VP_PROG_REG_START;
   COPY_4V_CAST(ctx->VertexProgram.Machine.Registers[index], params, GLfloat);
}


/**
 * Set a program parameter register.
 * \note Called from the GL API dispatcher.
 */
void _mesa_ProgramParameter4fNV(GLenum target, GLuint index,
                                GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
   GET_CURRENT_CONTEXT(ctx);
   ASSERT_OUTSIDE_BEGIN_END(ctx);

   if (target != GL_VERTEX_PROGRAM_NV) {
      _mesa_error(ctx, GL_INVALID_ENUM, "glProgramParameter4fNV");
      return;
   }

   if (index >= VP_NUM_PROG_REGS) {
      _mesa_error(ctx, GL_INVALID_VALUE, "glProgramParameter4fNV");
      return;
   }

   index += VP_PROG_REG_START;
   ASSIGN_4V(ctx->VertexProgram.Machine.Registers[index], x, y, z, w);
}


/**
 * Set a program parameter register.
 * \note Called from the GL API dispatcher.
 */
void _mesa_ProgramParameter4fvNV(GLenum target, GLuint index,
                                 const GLfloat *params)
{
   GET_CURRENT_CONTEXT(ctx);
   ASSERT_OUTSIDE_BEGIN_END(ctx);

   if (target != GL_VERTEX_PROGRAM_NV) {
      _mesa_error(ctx, GL_INVALID_ENUM, "glProgramParameter4fNV");
      return;
   }

   if (index >= VP_NUM_PROG_REGS) {
      _mesa_error(ctx, GL_INVALID_VALUE, "glProgramParameter4fNV");
      return;
   }

   index += VP_PROG_REG_START;
   COPY_4V(ctx->VertexProgram.Machine.Registers[index], params);
}


/**
 * Set a sequence of program parameter registers.
 * \note Called from the GL API dispatcher.
 */
void _mesa_ProgramParameters4dvNV(GLenum target, GLuint index,
                                  GLuint num, const GLdouble *params)
{
   GLuint i;
   GET_CURRENT_CONTEXT(ctx);
   ASSERT_OUTSIDE_BEGIN_END(ctx);

   if (target != GL_VERTEX_PROGRAM_NV) {
      _mesa_error(ctx, GL_INVALID_ENUM, "glProgramParameters4dvNV");
      return;
   }

   if (index + num > VP_NUM_PROG_REGS) {
      _mesa_error(ctx, GL_INVALID_VALUE, "glProgramParameters4dvNV");
      return;
   }

   index += VP_PROG_REG_START;
   for (i = 0; i < num; i++) {
      COPY_4V_CAST(ctx->VertexProgram.Machine.Registers[index + i], 
		   params, GLfloat);
      params += 4;
   };
}


/**
 * Set a sequence of program parameter registers.
 * \note Called from the GL API dispatcher.
 */
void _mesa_ProgramParameters4fvNV(GLenum target, GLuint index,
                                  GLuint num, const GLfloat *params)
{
   GLuint i;
   GET_CURRENT_CONTEXT(ctx);
   ASSERT_OUTSIDE_BEGIN_END(ctx);

   if (target != GL_VERTEX_PROGRAM_NV) {
      _mesa_error(ctx, GL_INVALID_ENUM, "glProgramParameters4fvNV");
      return;
   }

   if (index + num > VP_NUM_PROG_REGS) {
      _mesa_error(ctx, GL_INVALID_VALUE, "glProgramParameters4fvNV");
      return;
   }

   index += VP_PROG_REG_START;
   for (i = 0; i < num; i++) {
      COPY_4V(ctx->VertexProgram.Machine.Registers[index + i], params);
      params += 4;
   };
}


/**
 * Setup tracking of matrices into program parameter registers.
 * \note Called from the GL API dispatcher.
 */
void _mesa_TrackMatrixNV(GLenum target, GLuint address,
                         GLenum matrix, GLenum transform)
{
   GET_CURRENT_CONTEXT(ctx);
   ASSERT_OUTSIDE_BEGIN_END(ctx);

   if (target != GL_VERTEX_PROGRAM_NV) {
      _mesa_error(ctx, GL_INVALID_ENUM, "glTrackMatrixNV(target)");
      return;
   }

   if (address & 0x3) {
      /* addr must be multiple of four */
      _mesa_error(ctx, GL_INVALID_VALUE, "glTrackMatrixNV(address)");
      return;
   }

   switch (matrix) {
      case GL_NONE:
      case GL_MODELVIEW:
      case GL_PROJECTION:
      case GL_TEXTURE:
      case GL_COLOR:
      case GL_MODELVIEW_PROJECTION_NV:
      case GL_MATRIX0_NV:
      case GL_MATRIX1_NV:
      case GL_MATRIX2_NV:
      case GL_MATRIX3_NV:
      case GL_MATRIX4_NV:
      case GL_MATRIX5_NV:
      case GL_MATRIX6_NV:
      case GL_MATRIX7_NV:
         /* OK, fallthrough */
         break;
      default:
         _mesa_error(ctx, GL_INVALID_ENUM, "glTrackMatrixNV(matrix)");
         return;
   }

   switch (transform) {
      case GL_IDENTITY_NV:
      case GL_INVERSE_NV:
      case GL_TRANSPOSE_NV:
      case GL_INVERSE_TRANSPOSE_NV:
         /* OK, fallthrough */
         break;
      default:
         _mesa_error(ctx, GL_INVALID_ENUM, "glTrackMatrixNV(transform)");
         return;
   }

   ctx->VertexProgram.TrackMatrix[address / 4] = matrix;
   ctx->VertexProgram.TrackMatrixTransform[address / 4] = transform;
}
