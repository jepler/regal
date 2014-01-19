/* NOTE: Do not edit this file, it is generated by a script:
   E.py --api gl 4.4 --api wgl 4.4 --api glx 4.4 --api cgl 1.4 --api egl 1.0 --outdir .
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

#if REGAL_EMULATION

REGAL_GLOBAL_BEGIN

#include "RegalPrivate.h"
#include "RegalContext.h"
#include "RegalDispatch.h"
#include "RegalHint.h"
#include "RegalEmuProcsHint.h"

REGAL_GLOBAL_END

REGAL_NAMESPACE_BEGIN

static void REGAL_CALL emuProcInterceptHint_glGetBooleanv(RegalContext *_context, GLenum pname, GLboolean *params)
{
  RegalAssert(_context);
  EmuProcsOriginateHint & orig = _context->hint->orig;

  // impl
  if( _context->hint->glGetv( *_context, pname, params ) ) {
    return;
  }

  orig.glGetBooleanv( _context, pname, params );

}

static void REGAL_CALL emuProcInterceptHint_glGetDoublev(RegalContext *_context, GLenum pname, GLdouble *params)
{
  RegalAssert(_context);
  EmuProcsOriginateHint & orig = _context->hint->orig;

  // impl
  if( _context->hint->glGetv( *_context, pname, params ) ) {
    return;
  }

  orig.glGetDoublev( _context, pname, params );

}

static void REGAL_CALL emuProcInterceptHint_glGetFloatv(RegalContext *_context, GLenum pname, GLfloat *params)
{
  RegalAssert(_context);
  EmuProcsOriginateHint & orig = _context->hint->orig;

  // impl
  if( _context->hint->glGetv( *_context, pname, params ) ) {
    return;
  }

  orig.glGetFloatv( _context, pname, params );

}

static void REGAL_CALL emuProcInterceptHint_glGetIntegerv(RegalContext *_context, GLenum pname, GLint *params)
{
  RegalAssert(_context);
  EmuProcsOriginateHint & orig = _context->hint->orig;

  // impl
  if( _context->hint->glGetv( *_context, pname, params ) ) {
    return;
  }

  orig.glGetIntegerv( _context, pname, params );

}

static void REGAL_CALL emuProcInterceptHint_glHint(RegalContext *_context, GLenum target, GLenum mode)
{
  RegalAssert(_context);
  EmuProcsOriginateHint & orig = _context->hint->orig;

  // impl
  if( _context->hint->glHint( *_context, target, mode ) ) {
    return;
  }

  orig.glHint( _context, target, mode );

}

void EmuProcsInterceptHint( Dispatch::GL & dt ) {
  dt.glGetBooleanv = emuProcInterceptHint_glGetBooleanv;
  dt.glGetDoublev  = emuProcInterceptHint_glGetDoublev;
  dt.glGetFloatv   = emuProcInterceptHint_glGetFloatv;
  dt.glGetIntegerv = emuProcInterceptHint_glGetIntegerv;
  dt.glHint        = emuProcInterceptHint_glHint;
}

REGAL_NAMESPACE_END

#endif // REGAL_EMULATION