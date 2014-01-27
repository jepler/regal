/* NOTE: Do not edit this file, it is generated by a script:
   Export.py --api gl 4.4 --api wgl 4.4 --api glx 4.4 --api cgl 1.4 --api egl 1.0 --outdir .
*/

/*
  Copyright (c) 2011-2013 NVIDIA Corporation
  Copyright (c) 2011-2013 Cass Everitt
  Copyright (c) 2012-2013 Scott Nations
  Copyright (c) 2012 Mathias Schott
  Copyright (c) 2012-2013 Nigel Stewart
  Copyright (c) 2012-2013 Google Inc.
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification,
  are permitted provided that the following conditions are met:

    Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.

    Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
  OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
  Intended formatting conventions:
  $ astyle --style=allman --indent=spaces=2 --indent-switches
*/

#include "RegalUtil.h"

REGAL_GLOBAL_BEGIN

#include "RegalPrivate.h"
#include "RegalContext.h"
#include "RegalDispatch.h"
#include "TexSto.h"
#include "TexStoProcs.h"

extern "C" Regal::Layer * createTexSto( Regal::RegalContext * ctx ) {
  return new Regal::Emu::TexSto( ctx );
}

REGAL_GLOBAL_END

REGAL_NAMESPACE_BEGIN

using namespace Emu;

static void REGAL_CALL texsto_glDeleteTextures(Layer *_layer, GLsizei n, const GLuint *textures)
{
  TexSto * self = static_cast<TexSto *>(_layer);

  TexStoOriginate & orig = self->orig;

  // prefix
  self->DeleteTextures(n, textures );

  RglDeleteTextures( orig, n, textures );

}

static void REGAL_CALL texsto_glGetTexParameterIiv(Layer *_layer, GLenum target, GLenum pname, GLint *params)
{
  TexSto * self = static_cast<TexSto *>(_layer);

  TexStoOriginate & orig = self->orig;

  // impl
  if ( self->GetTexParameterv(target, pname, params ) ) {
     return;
  }

  RglGetTexParameterIiv( orig, target, pname, params );

}

static void REGAL_CALL texsto_glGetTexParameterIuiv(Layer *_layer, GLenum target, GLenum pname, GLuint *params)
{
  TexSto * self = static_cast<TexSto *>(_layer);

  TexStoOriginate & orig = self->orig;

  // impl
  if ( self->GetTexParameterv(target, pname, params ) ) {
     return;
  }

  RglGetTexParameterIuiv( orig, target, pname, params );

}

static void REGAL_CALL texsto_glGetTexParameterfv(Layer *_layer, GLenum target, GLenum pname, GLfloat *params)
{
  TexSto * self = static_cast<TexSto *>(_layer);

  TexStoOriginate & orig = self->orig;

  // impl
  if ( self->GetTexParameterv(target, pname, params ) ) {
     return;
  }

  RglGetTexParameterfv( orig, target, pname, params );

}

static void REGAL_CALL texsto_glGetTexParameteriv(Layer *_layer, GLenum target, GLenum pname, GLint *params)
{
  TexSto * self = static_cast<TexSto *>(_layer);

  TexStoOriginate & orig = self->orig;

  // impl
  if ( self->GetTexParameterv(target, pname, params ) ) {
     return;
  }

  RglGetTexParameteriv( orig, target, pname, params );

}

static void REGAL_CALL texsto_glTexStorage1D(Layer *_layer, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width)
{
  TexSto * self = static_cast<TexSto *>(_layer);

  TexStoOriginate & orig = self->orig;

  // impl
  self->TextureStorage(target, levels, internalformat, width ); return;

  RglTexStorage1D( orig, target, levels, internalformat, width );

}

static void REGAL_CALL texsto_glTexStorage1DEXT(Layer *_layer, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width)
{
  TexSto * self = static_cast<TexSto *>(_layer);

  TexStoOriginate & orig = self->orig;

  // impl
  self->TextureStorage(target, levels, internalformat, width ); return;

  RglTexStorage1DEXT( orig, target, levels, internalformat, width );

}

static void REGAL_CALL texsto_glTexStorage2D(Layer *_layer, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height)
{
  TexSto * self = static_cast<TexSto *>(_layer);

  TexStoOriginate & orig = self->orig;

  // impl
  self->TextureStorage(target, levels, internalformat, width, height ); return;

  RglTexStorage2D( orig, target, levels, internalformat, width, height );

}

static void REGAL_CALL texsto_glTexStorage2DEXT(Layer *_layer, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height)
{
  TexSto * self = static_cast<TexSto *>(_layer);

  TexStoOriginate & orig = self->orig;

  // impl
  self->TextureStorage(target, levels, internalformat, width, height ); return;

  RglTexStorage2DEXT( orig, target, levels, internalformat, width, height );

}

static void REGAL_CALL texsto_glTexStorage3D(Layer *_layer, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth)
{
  TexSto * self = static_cast<TexSto *>(_layer);

  TexStoOriginate & orig = self->orig;

  // impl
  self->TextureStorage(target, levels, internalformat, width, height, depth ); return;

  RglTexStorage3D( orig, target, levels, internalformat, width, height, depth );

}

static void REGAL_CALL texsto_glTexStorage3DEXT(Layer *_layer, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth)
{
  TexSto * self = static_cast<TexSto *>(_layer);

  TexStoOriginate & orig = self->orig;

  // impl
  self->TextureStorage(target, levels, internalformat, width, height, depth ); return;

  RglTexStorage3DEXT( orig, target, levels, internalformat, width, height, depth );

}

void TexStoIntercept( Layer *layer, Dispatch::GL & dt ) {
  dt.glDeleteTextures      = MakeRegalProc(texsto_glDeleteTextures, layer);
  dt.glGetTexParameterIiv  = MakeRegalProc(texsto_glGetTexParameterIiv, layer);
  dt.glGetTexParameterIuiv = MakeRegalProc(texsto_glGetTexParameterIuiv, layer);
  dt.glGetTexParameterfv   = MakeRegalProc(texsto_glGetTexParameterfv, layer);
  dt.glGetTexParameteriv   = MakeRegalProc(texsto_glGetTexParameteriv, layer);
  dt.glTexStorage1D        = MakeRegalProc(texsto_glTexStorage1D, layer);
  dt.glTexStorage1DEXT     = MakeRegalProc(texsto_glTexStorage1DEXT, layer);
  dt.glTexStorage2D        = MakeRegalProc(texsto_glTexStorage2D, layer);
  dt.glTexStorage2DEXT     = MakeRegalProc(texsto_glTexStorage2DEXT, layer);
  dt.glTexStorage3D        = MakeRegalProc(texsto_glTexStorage3D, layer);
  dt.glTexStorage3DEXT     = MakeRegalProc(texsto_glTexStorage3DEXT, layer);
}

REGAL_NAMESPACE_END
