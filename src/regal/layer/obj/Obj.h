/*
  Copyright (c) 2011-2013 NVIDIA Corporation
  Copyright (c) 2011-2012 Cass Everitt
  Copyright (c) 2012 Scott Nations
  Copyright (c) 2012 Mathias Schott
  Copyright (c) 2012 Nigel Stewart
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

 Regal object renaming layer
 Cass Everitt

 */

#ifndef __REGAL_OBJ_H__
#define __REGAL_OBJ_H__

#include "RegalUtil.h"

REGAL_GLOBAL_BEGIN

#include "RegalEmu.h"
#include "RegalPrivate.h"
#include "RegalSharedMap.h"
#include "ObjProcs.h"

REGAL_GLOBAL_END

REGAL_NAMESPACE_BEGIN

namespace Emu {

struct Name
{
  GLuint app;
  GLuint drv;
  Name() : app( 0 ), drv( 0 ) {}
};

struct NameTranslator
{
  shared_map< GLuint, Name > app2drv;
  shared_map< GLuint, Name * > drv2app;

  typedef void (REGAL_CALL *GenProc)( Layer *layer, GLsizei n, GLuint * objs );
  typedef void (REGAL_CALL *DelProc)( Layer *layer, GLsizei n, const GLuint * objs );
  RegalProc<GenProc> gen;
  RegalProc<DelProc> del;

  NameTranslator()
  {
    drv2app[ 0 ] = & app2drv[ 0 ];  // special case 0
  }

  GLboolean IsObject( GLuint appName ) const
  {
    return app2drv.count( appName ) > 0 ? GL_TRUE : GL_FALSE;
  }

  GLuint Gen()
  {
    Name name;
    gen.proc( gen.layer, 1, & name.drv );
    // could be more clever here, and this could fail...
    name.app = name.drv;
    const GLuint searchLimit = 1000000000;
    for( GLuint i = 0; i < searchLimit; i++ ) {
        if( app2drv.count( name.app ) == 0 ) {
            break;
        }
        name.app++;
    }
    if( ( name.app - name.drv ) >= searchLimit ) {
        return 0;
    }
    app2drv[ name.app ] = name;
    drv2app[ name.drv ] = & app2drv[ name.app ];
    return name.app;
  }

  GLuint ToDriverName( GLuint appName )
  {
    if( app2drv.count( appName ) == 0 )
    {
        Name & name = app2drv[ appName ];
        name.app = appName;
        gen.proc( gen.layer, 1, & name.drv );
        drv2app[ name.drv ] = &name;
    }
    return app2drv[ appName ].drv;
  }

  GLuint ToAppName( GLuint drvName )
  {
    if( drv2app.count( drvName ) )
    {
        RegalAssert( drv2app[ drvName ] != NULL );
        return drv2app[ drvName ]->app;
    }
    return 0;
  }

  void Delete( GLuint appName )
  {
    if( appName == 0 || app2drv.count( appName ) == 0 ) {
        return;
    }
    Name n = app2drv[ appName ];
    app2drv.erase( n.app );
    RegalAssert( drv2app.count( n.drv ) != 0 );
    drv2app.erase( n.drv );
    del.proc( del.layer, 1, & n.drv );
  }
};
  
struct Obj : public Layer
{
  NameTranslator bufferNames;
  NameTranslator vaoNames;
  NameTranslator textureNames;
  ObjOriginate orig;

  Obj( RegalContext * ctx ) : Layer( ctx ) {}
  
  virtual std::string GetName() const { return "obj"; }
  
  virtual void ResetInterception() {
    RegalContext &ctx = *GetContext();
    orig.Initialize( ctx.dispatchGL );
    ObjIntercept( this, ctx.dispatchGL );
  }
  
  virtual bool Initialize( const std::string & instanceInfo )
  {
    RegalContext &ctx = *GetContext();
    orig.Initialize( ctx.dispatchGL );
    ObjIntercept( this, ctx.dispatchGL );

    RegalContext *sharingWith = ctx.shareGroup->front();
    if (sharingWith)
    {
      Obj *sharedObj = static_cast<Obj *>(sharingWith->find( GetName() ));
      if( sharedObj ) {
        bufferNames.app2drv  = sharedObj->bufferNames.app2drv;
        bufferNames.drv2app  = sharedObj->bufferNames.drv2app;
        vaoNames.app2drv     = sharedObj->vaoNames.app2drv;
        vaoNames.drv2app     = sharedObj->vaoNames.drv2app;
        textureNames.app2drv = sharedObj->textureNames.app2drv;
        textureNames.drv2app = sharedObj->textureNames.drv2app;
      }
    }
    bufferNames.gen  = orig.glGenBuffers;
    bufferNames.del  = orig.glDeleteBuffers;
    vaoNames.gen     = orig.glGenVertexArrays;
    vaoNames.del     = orig.glDeleteVertexArrays;
    textureNames.gen = orig.glGenTextures;
    textureNames.del = orig.glDeleteTextures;
    return true;
  }

  void BindBuffer(GLenum target, GLuint bufferBinding)
  {
    RglBindBuffer( orig, target, bufferNames.ToDriverName( bufferBinding ) );
  }

  void GenBuffers(GLsizei n, GLuint *buffers)
  {
    UNUSED_PARAMETER(ctx);
    for( int i = 0; i < n; i++ ) {
      buffers[ i ] = bufferNames.Gen();
    }
  }

  void DeleteBuffers(GLsizei n, const GLuint *buffers)
  {
    UNUSED_PARAMETER(ctx);
    for( int i = 0; i < n; i++ ) {
      bufferNames.Delete( buffers[ i ] );
    }
  }

  GLboolean IsBuffer(GLuint appName) const
  {
    UNUSED_PARAMETER(ctx);
    return bufferNames.IsObject( appName );
  }

  void BindVertexArray(GLuint vao)
  {
    RglBindVertexArray( orig, vaoNames.ToDriverName( vao ) );
  }

  void GenVertexArrays(GLsizei n, GLuint *vaos)
  {
    UNUSED_PARAMETER(ctx);
    for( int i = 0; i < n; i++ ) {
      vaos[ i ] = vaoNames.Gen();
    }
  }

  void DeleteVertexArrays(GLsizei n, const GLuint * vaos)
  {
    UNUSED_PARAMETER(ctx);
    for( int i = 0; i < n; i++ ) {
      vaoNames.Delete( vaos[ i ] );
    }
  }

  GLboolean IsVertexArray(GLuint appName) const
  {
    UNUSED_PARAMETER(ctx);
    return vaoNames.IsObject( appName );
  }

  void BindTexture(GLenum target, GLuint name)
  {
    RglBindTexture( orig, target, textureNames.ToDriverName( name ) );
  }

  void GenTextures(GLsizei n, GLuint *names)
  {
    UNUSED_PARAMETER(ctx);
    for( int i = 0; i < n; i++ ) {
      names[ i ] = textureNames.Gen();
    }
  }

  void DeleteTextures(GLsizei n, const GLuint * names)
  {
    UNUSED_PARAMETER(ctx);
    for( int i = 0; i < n; i++ ) {
      textureNames.Delete( names[ i ] );
    }
  }

  GLboolean IsTexture(GLuint name) const
  {
    UNUSED_PARAMETER(ctx);
    return textureNames.IsObject( name );
  }
};

}

REGAL_NAMESPACE_END

#endif // ! __REGAL_OBJ_H__
