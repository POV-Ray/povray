/* $Id: dd.h,v 1.74 2002/10/11 17:41:04 brianp Exp $ */

/*
 * Mesa 3-D graphics library
 * Version:  4.1
 *
 * Copyright (C) 1999-2002  Brian Paul   All Rights Reserved.
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



#ifndef DD_INCLUDED
#define DD_INCLUDED

/* THIS FILE ONLY INCLUDED BY mtypes.h !!!!! */

struct gl_pixelstore_attrib;

/* Mask bits sent to the driver Clear() function */
#define DD_FRONT_LEFT_BIT  FRONT_LEFT_BIT         /* 1 */
#define DD_FRONT_RIGHT_BIT FRONT_RIGHT_BIT        /* 2 */
#define DD_BACK_LEFT_BIT   BACK_LEFT_BIT          /* 4 */
#define DD_BACK_RIGHT_BIT  BACK_RIGHT_BIT         /* 8 */
#define DD_AUX0            AUX0_BIT               /* future use */
#define DD_AUX1            AUX1_BIT               /* future use */
#define DD_AUX2            AUX2_BIT               /* future use */
#define DD_AUX3            AUX3_BIT               /* future use */
#define DD_DEPTH_BIT       GL_DEPTH_BUFFER_BIT    /* 0x00000100 */
#define DD_ACCUM_BIT       GL_ACCUM_BUFFER_BIT    /* 0x00000200 */
#define DD_STENCIL_BIT     GL_STENCIL_BUFFER_BIT  /* 0x00000400 */


/*
 * Device Driver function table.
 */
struct dd_function_table {

   const GLubyte * (*GetString)( GLcontext *ctx, GLenum name );
   /* Return a string as needed by glGetString().
    * Only the GL_RENDERER token must be implemented.  Otherwise,
    * NULL can be returned.
    */

   void (*UpdateState)( GLcontext *ctx, GLuint new_state );
   /*
    * UpdateState() is called to notify the driver after Mesa has made
    * some internal state changes.  This is in addition to any
    * statechange callbacks Mesa may already have made.
    */

   void (*Clear)( GLcontext *ctx, GLbitfield mask, GLboolean all,
		  GLint x, GLint y, GLint width, GLint height );
   /* Clear the color/depth/stencil/accum buffer(s).
    * 'mask' is a bitmask of the DD_*_BIT values defined above that indicates
    * which buffers need to be cleared.
    * If 'all' is true then the clear the whole buffer, else clear only the
    * region defined by (x,y,width,height).
    * This function must obey the glColorMask, glIndexMask and glStencilMask
    * settings!
    * Software Mesa can do masked clears if the device driver can't.
    */

   void (*DrawBuffer)( GLcontext *ctx, GLenum buffer );
   /*
    * Specifies the current buffer for writing.  Called via glDrawBuffer().
    * Note the driver must organize fallbacks (eg with swrast) if it
    * cannot implement the requested mode.
    */


   void (*ReadBuffer)( GLcontext *ctx, GLenum buffer );
   /*
    * Specifies the current buffer for reading.  Called via glReadBuffer().
    */

   void (*GetBufferSize)( GLframebuffer *buffer,
                          GLuint *width, GLuint *height );
   /*
    * Returns the width and height of the named buffer/window.
    * Mesa uses this to determine when the driver's window size has changed.
    */

   void (*ResizeBuffers)( GLframebuffer *buffer );
   /*
    * Resize the driver's depth/stencil/accum/back buffers to match the
    * size given in the GLframebuffer struct.  This is typically called
    * when Mesa detects that a window size has changed.
    */

   void (*Finish)( GLcontext *ctx );
   /*
    * This is called whenever glFinish() is called.
    */

   void (*Flush)( GLcontext *ctx );
   /*
    * This is called whenever glFlush() is called.
    */

   void (*Error)( GLcontext *ctx );
   /*
    * Called whenever an error is generated.  ctx->ErrorValue contains
    * the error value.
    */


   /***
    *** For hardware accumulation buffer:
    ***/
   void (*Accum)( GLcontext *ctx, GLenum op, GLfloat value,
		  GLint xpos, GLint ypos, GLint width, GLint height );
   /* Execute glAccum command within the given scissor region.
    */


   /***
    *** glDraw/Read/CopyPixels and glBitmap functions:
    ***/

   void (*DrawPixels)( GLcontext *ctx,
		       GLint x, GLint y, GLsizei width, GLsizei height,
		       GLenum format, GLenum type,
		       const struct gl_pixelstore_attrib *unpack,
		       const GLvoid *pixels );
   /* This is called by glDrawPixels.
    * 'unpack' describes how to unpack the source image data.
    */

   void (*ReadPixels)( GLcontext *ctx,
		       GLint x, GLint y, GLsizei width, GLsizei height,
		       GLenum format, GLenum type,
		       const struct gl_pixelstore_attrib *unpack,
		       GLvoid *dest );
   /* Called by glReadPixels.
    */

   void (*CopyPixels)( GLcontext *ctx,
                            GLint srcx, GLint srcy,
                            GLsizei width, GLsizei height,
                            GLint dstx, GLint dsty, GLenum type );
   /* Do a glCopyPixels.  This function must respect all rasterization
    * state, glPixelTransfer, glPixelZoom, etc.
    */

   void (*Bitmap)( GLcontext *ctx,
		   GLint x, GLint y, GLsizei width, GLsizei height,
		   const struct gl_pixelstore_attrib *unpack,
		   const GLubyte *bitmap );
   /* This is called by glBitmap.  Works the same as DrawPixels, above.
    */

   /***
    *** Texture image functions:
    ***/
   const struct gl_texture_format *
   (*ChooseTextureFormat)( GLcontext *ctx, GLint internalFormat,
                           GLenum srcFormat, GLenum srcType );
   /* This is called by the _mesa_store_tex[sub]image[123]d() fallback
    * functions.  The driver should examine <internalFormat> and return a
    * pointer to an appropriate gl_texture_format.
    */

   void (*TexImage1D)( GLcontext *ctx, GLenum target, GLint level,
                       GLint internalFormat,
                       GLint width, GLint border,
                       GLenum format, GLenum type, const GLvoid *pixels,
                       const struct gl_pixelstore_attrib *packing,
                       struct gl_texture_object *texObj,
                       struct gl_texture_image *texImage );
   void (*TexImage2D)( GLcontext *ctx, GLenum target, GLint level,
                       GLint internalFormat,
                       GLint width, GLint height, GLint border,
                       GLenum format, GLenum type, const GLvoid *pixels,
                       const struct gl_pixelstore_attrib *packing,
                       struct gl_texture_object *texObj,
                       struct gl_texture_image *texImage );
   void (*TexImage3D)( GLcontext *ctx, GLenum target, GLint level,
                       GLint internalFormat,
                       GLint width, GLint height, GLint depth, GLint border,
                       GLenum format, GLenum type, const GLvoid *pixels,
                       const struct gl_pixelstore_attrib *packing,
                       struct gl_texture_object *texObj,
                       struct gl_texture_image *texImage );
   /* Called by glTexImage1/2/3D.
    * Arguments:
    *   <target>, <level>, <format>, <type> and <pixels> are user specified.
    *   <packing> indicates the image packing of pixels.
    *   <texObj> is the target texture object.
    *   <texImage> is the target texture image.  It will have the texture
    *      width, height, depth, border and internalFormat information.
    *   <retainInternalCopy> is returned by this function and indicates whether
    *      core Mesa should keep an internal copy of the texture image.
    * Drivers should call a fallback routine from texstore.c if needed.
    */

   void (*TexSubImage1D)( GLcontext *ctx, GLenum target, GLint level,
                          GLint xoffset, GLsizei width,
                          GLenum format, GLenum type,
                          const GLvoid *pixels,
                          const struct gl_pixelstore_attrib *packing,
                          struct gl_texture_object *texObj,
                          struct gl_texture_image *texImage );
   void (*TexSubImage2D)( GLcontext *ctx, GLenum target, GLint level,
                          GLint xoffset, GLint yoffset,
                          GLsizei width, GLsizei height,
                          GLenum format, GLenum type,
                          const GLvoid *pixels,
                          const struct gl_pixelstore_attrib *packing,
                          struct gl_texture_object *texObj,
                          struct gl_texture_image *texImage );
   void (*TexSubImage3D)( GLcontext *ctx, GLenum target, GLint level,
                          GLint xoffset, GLint yoffset, GLint zoffset,
                          GLsizei width, GLsizei height, GLint depth,
                          GLenum format, GLenum type,
                          const GLvoid *pixels,
                          const struct gl_pixelstore_attrib *packing,
                          struct gl_texture_object *texObj,
                          struct gl_texture_image *texImage );
   /* Called by glTexSubImage1/2/3D.
    * Arguments:
    *   <target>, <level>, <xoffset>, <yoffset>, <zoffset>, <width>, <height>,
    *      <depth>, <format>, <type> and <pixels> are user specified.
    *   <packing> indicates the image packing of pixels.
    *   <texObj> is the target texture object.
    *   <texImage> is the target texture image.  It will have the texture
    *      width, height, border and internalFormat information.
    * The driver should use a fallback routine from texstore.c if needed.
    */

   void (*CopyTexImage1D)( GLcontext *ctx, GLenum target, GLint level,
                           GLenum internalFormat, GLint x, GLint y,
                           GLsizei width, GLint border );
   void (*CopyTexImage2D)( GLcontext *ctx, GLenum target, GLint level,
                           GLenum internalFormat, GLint x, GLint y,
                           GLsizei width, GLsizei height, GLint border );
   /* Called by glCopyTexImage1D and glCopyTexImage2D.
    * Drivers should use a fallback routine from texstore.c if needed.
    */

   void (*CopyTexSubImage1D)( GLcontext *ctx, GLenum target, GLint level,
                              GLint xoffset,
                              GLint x, GLint y, GLsizei width );
   void (*CopyTexSubImage2D)( GLcontext *ctx, GLenum target, GLint level,
                              GLint xoffset, GLint yoffset,
                              GLint x, GLint y,
                              GLsizei width, GLsizei height );
   void (*CopyTexSubImage3D)( GLcontext *ctx, GLenum target, GLint level,
                              GLint xoffset, GLint yoffset, GLint zoffset,
                              GLint x, GLint y,
                              GLsizei width, GLsizei height );
   /* Called by glCopyTexSubImage1/2/3D.
    * Drivers should use a fallback routine from texstore.c if needed.
    */

   GLboolean (*TestProxyTexImage)(GLcontext *ctx, GLenum target,
                                  GLint level, GLint internalFormat,
                                  GLenum format, GLenum type,
                                  GLint width, GLint height,
                                  GLint depth, GLint border);
   /* Called by glTexImage[123]D when user specifies a proxy texture
    * target.  Return GL_TRUE if the proxy test passes, return GL_FALSE
    * if the test fails.
    */

   /***
    *** Compressed texture functions:
    ***/

   void (*CompressedTexImage1D)( GLcontext *ctx, GLenum target,
                                 GLint level, GLint internalFormat,
                                 GLsizei width, GLint border,
                                 GLsizei imageSize, const GLvoid *data,
                                 struct gl_texture_object *texObj,
                                 struct gl_texture_image *texImage );
   void (*CompressedTexImage2D)( GLcontext *ctx, GLenum target,
                                 GLint level, GLint internalFormat,
                                 GLsizei width, GLsizei height, GLint border,
                                 GLsizei imageSize, const GLvoid *data,
                                 struct gl_texture_object *texObj,
                                 struct gl_texture_image *texImage );
   void (*CompressedTexImage3D)( GLcontext *ctx, GLenum target,
                                 GLint level, GLint internalFormat,
                                 GLsizei width, GLsizei height, GLsizei depth,
                                 GLint border,
                                 GLsizei imageSize, const GLvoid *data,
                                 struct gl_texture_object *texObj,
                                 struct gl_texture_image *texImage );
   /* Called by glCompressedTexImage1/2/3D.
    * Arguments:
    *   <target>, <level>, <internalFormat>, <data> are user specified.
    *   <texObj> is the target texture object.
    *   <texImage> is the target texture image.  It will have the texture
    *      width, height, depth, border and internalFormat information.
    *   <retainInternalCopy> is returned by this function and indicates whether
    *      core Mesa should keep an internal copy of the texture image.
    * Return GL_TRUE if operation completed, return GL_FALSE if core Mesa
    * should do the job.
    */

   void (*CompressedTexSubImage1D)(GLcontext *ctx, GLenum target, GLint level,
                                   GLint xoffset, GLsizei width,
                                   GLenum format,
                                   GLsizei imageSize, const GLvoid *data,
                                   struct gl_texture_object *texObj,
                                   struct gl_texture_image *texImage);
   void (*CompressedTexSubImage2D)(GLcontext *ctx, GLenum target, GLint level,
                                   GLint xoffset, GLint yoffset,
                                   GLsizei width, GLint height,
                                   GLenum format,
                                   GLsizei imageSize, const GLvoid *data,
                                   struct gl_texture_object *texObj,
                                   struct gl_texture_image *texImage);
   void (*CompressedTexSubImage3D)(GLcontext *ctx, GLenum target, GLint level,
                                   GLint xoffset, GLint yoffset, GLint zoffset,
                                   GLsizei width, GLint height, GLint depth,
                                   GLenum format,
                                   GLsizei imageSize, const GLvoid *data,
                                   struct gl_texture_object *texObj,
                                   struct gl_texture_image *texImage);
   /* Called by glCompressedTexSubImage1/2/3D.
    * Arguments:
    *   <target>, <level>, <x/z/zoffset>, <width>, <height>, <depth>,
    *      <imageSize>, and <data> are user specified.
    *   <texObj> is the target texture object.
    *   <texImage> is the target texture image.  It will have the texture
    *      width, height, depth, border and internalFormat information.
    * Return GL_TRUE if operation completed, return GL_FALSE if core Mesa
    * should do the job.
    */

   /***
    *** Texture object functions:
    ***/

   void (*BindTexture)( GLcontext *ctx, GLenum target,
                        struct gl_texture_object *tObj );
   /* Called by glBindTexture().
    */

   void (*CreateTexture)( GLcontext *ctx, struct gl_texture_object *tObj );
   /* Called when a texture object is created.
    */

   void (*DeleteTexture)( GLcontext *ctx, struct gl_texture_object *tObj );
   /* Called when a texture object is about to be deallocated.  Driver
    * should free anything attached to the DriverData pointers.
    */

   GLboolean (*IsTextureResident)( GLcontext *ctx,
                                   struct gl_texture_object *t );
   /* Called by glAreTextureResident().
    */

   void (*PrioritizeTexture)( GLcontext *ctx,  struct gl_texture_object *t,
                              GLclampf priority );
   /* Called by glPrioritizeTextures().
    */

   void (*ActiveTexture)( GLcontext *ctx, GLuint texUnitNumber );
   /* Called by glActiveTextureARB to set current texture unit.
    */

   void (*UpdateTexturePalette)( GLcontext *ctx,
                                 struct gl_texture_object *tObj );
   /* Called when the texture's color lookup table is changed.
    * If tObj is NULL then the shared texture palette ctx->Texture.Palette
    * is to be updated.
    */

   /***
    *** Imaging functionality:
    ***/
   void (*CopyColorTable)( GLcontext *ctx,
			   GLenum target, GLenum internalformat,
			   GLint x, GLint y, GLsizei width );

   void (*CopyColorSubTable)( GLcontext *ctx,
			      GLenum target, GLsizei start,
			      GLint x, GLint y, GLsizei width );

   void (*CopyConvolutionFilter1D)( GLcontext *ctx, GLenum target,
				    GLenum internalFormat,
				    GLint x, GLint y, GLsizei width );

   void (*CopyConvolutionFilter2D)( GLcontext *ctx, GLenum target,
				    GLenum internalFormat,
				    GLint x, GLint y,
				    GLsizei width, GLsizei height );



   /***
    *** State-changing functions (drawing functions are above)
    ***
    *** These functions are called by their corresponding OpenGL API functions.
    *** They're ALSO called by the gl_PopAttrib() function!!!
    *** May add more functions like these to the device driver in the future.
    ***/
   void (*AlphaFunc)(GLcontext *ctx, GLenum func, GLfloat ref);
   void (*BlendColor)(GLcontext *ctx, const GLfloat color[4]);
   void (*BlendEquation)(GLcontext *ctx, GLenum mode);
   void (*BlendFunc)(GLcontext *ctx, GLenum sfactor, GLenum dfactor);
   void (*BlendFuncSeparate)(GLcontext *ctx,
                             GLenum sfactorRGB, GLenum dfactorRGB,
                             GLenum sfactorA, GLenum dfactorA);
   void (*ClearColor)(GLcontext *ctx, const GLfloat color[4]);
   void (*ClearDepth)(GLcontext *ctx, GLclampd d);
   void (*ClearIndex)(GLcontext *ctx, GLuint index);
   void (*ClearStencil)(GLcontext *ctx, GLint s);
   void (*ClipPlane)(GLcontext *ctx, GLenum plane, const GLfloat *equation );
   void (*ColorMask)(GLcontext *ctx, GLboolean rmask, GLboolean gmask,
                     GLboolean bmask, GLboolean amask );
   void (*ColorMaterial)(GLcontext *ctx, GLenum face, GLenum mode);
   void (*CullFace)(GLcontext *ctx, GLenum mode);
   void (*FrontFace)(GLcontext *ctx, GLenum mode);
   void (*DepthFunc)(GLcontext *ctx, GLenum func);
   void (*DepthMask)(GLcontext *ctx, GLboolean flag);
   void (*DepthRange)(GLcontext *ctx, GLclampd nearval, GLclampd farval);
   void (*Enable)(GLcontext* ctx, GLenum cap, GLboolean state);
   void (*Fogfv)(GLcontext *ctx, GLenum pname, const GLfloat *params);
   void (*Hint)(GLcontext *ctx, GLenum target, GLenum mode);
   void (*IndexMask)(GLcontext *ctx, GLuint mask);
   void (*Lightfv)(GLcontext *ctx, GLenum light,
		   GLenum pname, const GLfloat *params );
   void (*LightModelfv)(GLcontext *ctx, GLenum pname, const GLfloat *params);
   void (*LineStipple)(GLcontext *ctx, GLint factor, GLushort pattern );
   void (*LineWidth)(GLcontext *ctx, GLfloat width);
   void (*LogicOpcode)(GLcontext *ctx, GLenum opcode);
   void (*PointParameterfv)(GLcontext *ctx, GLenum pname,
                            const GLfloat *params);
   void (*PointSize)(GLcontext *ctx, GLfloat size);
   void (*PolygonMode)(GLcontext *ctx, GLenum face, GLenum mode);
   void (*PolygonOffset)(GLcontext *ctx, GLfloat factor, GLfloat units);
   void (*PolygonStipple)(GLcontext *ctx, const GLubyte *mask );
   void (*RenderMode)(GLcontext *ctx, GLenum mode );
   void (*Scissor)(GLcontext *ctx, GLint x, GLint y, GLsizei w, GLsizei h);
   void (*ShadeModel)(GLcontext *ctx, GLenum mode);
   void (*StencilFunc)(GLcontext *ctx, GLenum func, GLint ref, GLuint mask);
   void (*StencilMask)(GLcontext *ctx, GLuint mask);
   void (*StencilOp)(GLcontext *ctx, GLenum fail, GLenum zfail, GLenum zpass);
   void (*ActiveStencilFace)(GLcontext *ctx, GLuint face);
   void (*TexGen)(GLcontext *ctx, GLenum coord, GLenum pname,
		  const GLfloat *params);
   void (*TexEnv)(GLcontext *ctx, GLenum target, GLenum pname,
                  const GLfloat *param);
   void (*TexParameter)(GLcontext *ctx, GLenum target,
                        struct gl_texture_object *texObj,
                        GLenum pname, const GLfloat *params);
   void (*TextureMatrix)(GLcontext *ctx, GLuint unit, const GLmatrix *mat);
   void (*Viewport)(GLcontext *ctx, GLint x, GLint y, GLsizei w, GLsizei h);

   /***
    *** Vertex array functions
    ***
    *** Called by the corresponding OpenGL functions.
    ***/
   void (*VertexPointer)(GLcontext *ctx, GLint size, GLenum type,
			 GLsizei stride, const GLvoid *ptr);
   void (*NormalPointer)(GLcontext *ctx, GLenum type,
			 GLsizei stride, const GLvoid *ptr);
   void (*ColorPointer)(GLcontext *ctx, GLint size, GLenum type,
			GLsizei stride, const GLvoid *ptr);
   void (*FogCoordPointer)(GLcontext *ctx, GLenum type,
			   GLsizei stride, const GLvoid *ptr);
   void (*IndexPointer)(GLcontext *ctx, GLenum type,
			GLsizei stride, const GLvoid *ptr);
   void (*SecondaryColorPointer)(GLcontext *ctx, GLint size, GLenum type,
				 GLsizei stride, const GLvoid *ptr);
   void (*TexCoordPointer)(GLcontext *ctx, GLint size, GLenum type,
			   GLsizei stride, const GLvoid *ptr);
   void (*EdgeFlagPointer)(GLcontext *ctx, GLsizei stride, const GLvoid *ptr);
   void (*VertexAttribPointer)(GLcontext *ctx, GLuint index, GLint size,
                               GLenum type, GLsizei stride, const GLvoid *ptr);


   /*** State-query functions
    ***
    *** Return GL_TRUE if query was completed, GL_FALSE otherwise.
    ***/
   GLboolean (*GetBooleanv)(GLcontext *ctx, GLenum pname, GLboolean *result);
   GLboolean (*GetDoublev)(GLcontext *ctx, GLenum pname, GLdouble *result);
   GLboolean (*GetFloatv)(GLcontext *ctx, GLenum pname, GLfloat *result);
   GLboolean (*GetIntegerv)(GLcontext *ctx, GLenum pname, GLint *result);
   GLboolean (*GetPointerv)(GLcontext *ctx, GLenum pname, GLvoid **result);

   /***
    *** Support for multiple t&l engines
    ***/

   GLuint NeedValidate;
   /* Bitmask of state changes that require the current tnl module to be
    * validated, using ValidateTnlModule() below.
    */

   void (*ValidateTnlModule)( GLcontext *ctx, GLuint new_state );
   /* Validate the current tnl module.  This is called directly after
    * UpdateState() when a state change that has occured matches the
    * NeedValidate bitmask above.  This ensures all computed values are
    * up to date, thus allowing the driver to decide if the current tnl
    * module needs to be swapped out.
    *
    * This must be non-NULL if a driver installs a custom tnl module and
    * sets the NeedValidate bitmask, but may be NULL otherwise.
    */


#define PRIM_OUTSIDE_BEGIN_END   GL_POLYGON+1
#define PRIM_INSIDE_UNKNOWN_PRIM GL_POLYGON+2
#define PRIM_UNKNOWN             GL_POLYGON+3

   GLuint CurrentExecPrimitive;
   /* Set by the driver-supplied t&l engine.  Set to
    * PRIM_OUTSIDE_BEGIN_END when outside begin/end.
    */

   GLuint CurrentSavePrimitive;
   /* Current state of an in-progress compilation.  May take on any of
    * the additional values defined above.
    */


#define FLUSH_STORED_VERTICES 0x1
#define FLUSH_UPDATE_CURRENT  0x2
   GLuint NeedFlush;
   /* Set by the driver-supplied t&l engine whenever vertices are
    * buffered between begin/end objects or ctx->Current is not uptodate.
    *
    * The FlushVertices() call below may be used to resolve
    * these conditions.
    */

   void (*FlushVertices)( GLcontext *ctx, GLuint flags );
   /* If inside begin/end, ASSERT(0).
    * Otherwise,
    *   if (flags & FLUSH_STORED_VERTICES) flushes any buffered vertices,
    *   if (flags & FLUSH_UPDATE_CURRENT) updates ctx->Current
    *                                     and ctx->Light.Material
    *
    * Note that the default t&l engine never clears the
    * FLUSH_UPDATE_CURRENT bit, even after performing the update.
    */

   void (*LightingSpaceChange)( GLcontext *ctx );
   /* Notify driver that the special derived value _NeedEyeCoords has
    * changed.
    */

   void (*NewList)( GLcontext *ctx, GLuint list, GLenum mode );
   void (*EndList)( GLcontext *ctx );
   /* Let the t&l component know what is going on with display lists
    * in time to make changes to dispatch tables, etc.
    * Called by glNewList() and glEndList(), respectively.
    */

   void (*BeginCallList)( GLcontext *ctx, GLuint list );
   void (*EndCallList)( GLcontext *ctx );
   /* Notify the t&l component before and after calling a display list.
    * Called by glCallList(s), but not recursively.
    */

   void (*MakeCurrent)( GLcontext *ctx, GLframebuffer *drawBuffer,
			GLframebuffer *readBuffer );
   /* Let the t&l component know when the context becomes current.
    */


   void (*LockArraysEXT)( GLcontext *ctx, GLint first, GLsizei count );
   void (*UnlockArraysEXT)( GLcontext *ctx );
   /* Called by glLockArraysEXT() and glUnlockArraysEXT(), respectively.
    */
};



/*
 * Transform/Clip/Lighting interface
 */
typedef struct {
   void (*ArrayElement)( GLint ); /* NOTE */
   void (*Color3f)( GLfloat, GLfloat, GLfloat );
   void (*Color3fv)( const GLfloat * );
   void (*Color3ub)( GLubyte, GLubyte, GLubyte );
   void (*Color3ubv)( const GLubyte * );
   void (*Color4f)( GLfloat, GLfloat, GLfloat, GLfloat );
   void (*Color4fv)( const GLfloat * );
   void (*Color4ub)( GLubyte, GLubyte, GLubyte, GLubyte );
   void (*Color4ubv)( const GLubyte * );
   void (*EdgeFlag)( GLboolean );
   void (*EdgeFlagv)( const GLboolean * );
   void (*EvalCoord1f)( GLfloat );          /* NOTE */
   void (*EvalCoord1fv)( const GLfloat * ); /* NOTE */
   void (*EvalCoord2f)( GLfloat, GLfloat ); /* NOTE */
   void (*EvalCoord2fv)( const GLfloat * ); /* NOTE */
   void (*EvalPoint1)( GLint );             /* NOTE */
   void (*EvalPoint2)( GLint, GLint );      /* NOTE */
   void (*FogCoordfEXT)( GLfloat );
   void (*FogCoordfvEXT)( const GLfloat * );
   void (*Indexi)( GLint );
   void (*Indexiv)( const GLint * );
   void (*Materialfv)( GLenum face, GLenum pname, const GLfloat * ); /* NOTE */
   void (*MultiTexCoord1fARB)( GLenum, GLfloat );
   void (*MultiTexCoord1fvARB)( GLenum, const GLfloat * );
   void (*MultiTexCoord2fARB)( GLenum, GLfloat, GLfloat );
   void (*MultiTexCoord2fvARB)( GLenum, const GLfloat * );
   void (*MultiTexCoord3fARB)( GLenum, GLfloat, GLfloat, GLfloat );
   void (*MultiTexCoord3fvARB)( GLenum, const GLfloat * );
   void (*MultiTexCoord4fARB)( GLenum, GLfloat, GLfloat, GLfloat, GLfloat );
   void (*MultiTexCoord4fvARB)( GLenum, const GLfloat * );
   void (*Normal3f)( GLfloat, GLfloat, GLfloat );
   void (*Normal3fv)( const GLfloat * );
   void (*SecondaryColor3fEXT)( GLfloat, GLfloat, GLfloat );
   void (*SecondaryColor3fvEXT)( const GLfloat * );
   void (*SecondaryColor3ubEXT)( GLubyte, GLubyte, GLubyte );
   void (*SecondaryColor3ubvEXT)( const GLubyte * );
   void (*TexCoord1f)( GLfloat );
   void (*TexCoord1fv)( const GLfloat * );
   void (*TexCoord2f)( GLfloat, GLfloat );
   void (*TexCoord2fv)( const GLfloat * );
   void (*TexCoord3f)( GLfloat, GLfloat, GLfloat );
   void (*TexCoord3fv)( const GLfloat * );
   void (*TexCoord4f)( GLfloat, GLfloat, GLfloat, GLfloat );
   void (*TexCoord4fv)( const GLfloat * );
   void (*Vertex2f)( GLfloat, GLfloat );
   void (*Vertex2fv)( const GLfloat * );
   void (*Vertex3f)( GLfloat, GLfloat, GLfloat );
   void (*Vertex3fv)( const GLfloat * );
   void (*Vertex4f)( GLfloat, GLfloat, GLfloat, GLfloat );
   void (*Vertex4fv)( const GLfloat * );
   void (*CallList)( GLuint );	/* NOTE */
   void (*Begin)( GLenum );
   void (*End)( void );
   void (*VertexAttrib4fNV)( GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w );
   void (*VertexAttrib4fvNV)( GLuint index, const GLfloat *v );

   /* Drivers present a reduced set of the functions possible in
    * begin/end objects.  Core mesa provides translation stubs for the
    * remaining functions to map down to these entrypoints.
    *
    * These are the initial values to be installed into dispatch by
    * mesa.  If the t&l driver wants to modify the dispatch table
    * while installed, it must do so itself.  It would be possible for
    * the vertexformat to install it's own initial values for these
    * functions, but this way there is an obvious list of what is
    * expected of the driver.
    *
    * If the driver wants to hook in entrypoints other than those
    * listed above, it must restore them to their original values in
    * the disable() callback, below.
    */

   void (*Rectf)( GLfloat, GLfloat, GLfloat, GLfloat );
   /*
    */

   void (*DrawArrays)( GLenum mode, GLint start, GLsizei count );
   void (*DrawElements)( GLenum mode, GLsizei count, GLenum type,
			 const GLvoid *indices );
   void (*DrawRangeElements)( GLenum mode, GLuint start,
			      GLuint end, GLsizei count,
			      GLenum type, const GLvoid *indices );
   /* These may or may not belong here.  Heuristic: If an array is
    * enabled, the installed vertex format should support that array and
    * it's current size natively.
    */

   void (*EvalMesh1)( GLenum mode, GLint i1, GLint i2 );
   void (*EvalMesh2)( GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2 );
   /* If you don't support eval, fallback to the default vertex format
    * on receiving an eval call and use the pipeline mechanism to
    * provide partial t&l acceleration.
    *
    * Mesa will provide a set of helper functions to do eval within
    * accelerated vertex formats, eventually...
    */

   GLboolean prefer_float_colors;
   /* Should core try to send colors to glColor4f or glColor4chan,
    * where it has a choice?
    */
} GLvertexformat;


#endif /* DD_INCLUDED */
