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

#include "pch.h" /* For MS precompiled header support */

#include "RegalUtil.h"

REGAL_GLOBAL_BEGIN

#include "RegalConfig.h"
#include "RegalContext.h"
#include "RegalEmuInfo.h"
#include "RegalLayerInfo.h"
#include "RegalDebugInfo.h"
#include "RegalContextInfo.h"
#include "RegalStatistics.h"

#include "RegalMarker.h"
#include "RegalFrame.h"
#if REGAL_EMULATION
#include "RegalObj.h"
#include "RegalHint.h"
#include "RegalPpa.h"
#include "RegalPpca.h"
#include "RegalBin.h"
#include "RegalXfer.h"
#include "RegalTexSto.h"
#include "RegalBaseVertex.h"
#include "RegalRect.h"
#include "RegalIff.h"
#include "RegalQuads.h"
#include "RegalSo.h"
#include "RegalDsa.h"
#include "RegalVao.h"
#include "RegalTexC.h"
#include "RegalFilt.h"
#endif

REGAL_GLOBAL_END

REGAL_NAMESPACE_BEGIN

using namespace Logging;

RegalContext::RegalContext()
: initialized(false),
  dispatcher(),
  dbg(NULL),
  info(NULL),
#if REGAL_STATISTICS
  statistics(new Statistics()),
#endif
  marker(NULL),
  frame(NULL),
#if REGAL_EMULATION
  emuLevel(0),
  obj(NULL),
  hint(NULL),
  ppa(NULL),
  ppca(NULL),
  bin(NULL),
  xfer(NULL),
  texsto(NULL),
  bv(NULL),
  rect(NULL),
  iff(NULL),
  quads(NULL),
  so(NULL),
  dsa(NULL),
  vao(NULL),
  texc(NULL),
  filt(NULL),
#endif
#if REGAL_SYS_PPAPI
  ppapiES2(NULL),
  ppapiResource(0),
  sysCtx(0),
#else
  sysCtx(NULL),
#endif
  thread(0),
#if REGAL_SYS_X11
  x11Display(NULL),
#endif
#if REGAL_SYS_GLX
  x11Drawable(0),
#endif
#if REGAL_SYS_WGL
  hdc(0),
  hglrc(0),
#endif
  logCallback(NULL),
#if REGAL_CODE
  codeSource(NULL),
  codeHeader(NULL),
  codeInputNext(0),
  codeOutputNext(0),
  codeShaderNext(0),
  codeProgramNext(0),
  codeTextureNext(0),
#endif
  depthBeginEnd(0),
  depthPushMatrix(0),
  depthPushAttrib(0),
  depthNewList(0)
{
  Internal("RegalContext::RegalContext","()");

  if (Config::enableDebug)
  {
    dbg = new DebugInfo();
    dbg->Init(this);
  }

  shareGroup.push_back(this);
}

void
RegalContext::Init()
{
  Internal("RegalContext::Init","()");

  RegalAssert(!initialized);

  RegalAssert(this);
  if (!info)
  {
    info = new ContextInfo();
    RegalAssert(info);
    info->init(*this);
  }

  if (!emuInfo)
  {
    emuInfo = new EmuInfo();
    RegalAssert(emuInfo);
    emuInfo->init(*info);
  }

  if (!marker)
  {
    marker = new Marker;
  }
  if (!frame)
  {
    frame = new Frame;
  }

#if REGAL_EMULATION

#if !REGAL_FORCE_EMULATION
  if (Config::enableEmulation || Config::forceEmulation)
#endif
  {
    // emu
    emuLevel = 16;
    {
      Emu::LayerInfo layer;
      Emu::GetFilterLayerInfo( *this, layer );
      bool enable = false;
      #if REGAL_EMU_FILTER
      if( Config::enableEmuFilter ) {
        enable = layer.emulationSupported && ( layer.emulationNeeded || Config::forceEmuFilter );
        if( enable ) {
          Info("Activating emulation layer REGAL_EMU_FILTER");
          filt = new Emu::Filt;
          emuLevel = 0;
          filt->Init(*this);
        }
      }
      #endif /* REGAL_EMU_FILTER */
      Emu::SetFilterEmuInfo( enable, this->emuInfo, layer );
    }
    {
      Emu::LayerInfo layer;
      Emu::GetTexCLayerInfo( *this, layer );
      bool enable = false;
      #if REGAL_EMU_TEXC
      if( Config::enableEmuTexC ) {
        enable = layer.emulationSupported && ( layer.emulationNeeded || Config::forceEmuTexC );
        if( enable ) {
          Info("Activating emulation layer REGAL_EMU_TEXC");
          texc = new Emu::TexC;
          emuLevel = 1;
          texc->Init(*this);
        }
      }
      #endif /* REGAL_EMU_TEXC */
      Emu::SetTexCEmuInfo( enable, this->emuInfo, layer );
    }
    {
      Emu::LayerInfo layer;
      Emu::GetVaoLayerInfo( *this, layer );
      bool enable = false;
      #if REGAL_EMU_VAO
      if( Config::enableEmuVao ) {
        enable = layer.emulationSupported && ( layer.emulationNeeded || Config::forceEmuVao );
        if( enable ) {
          Info("Activating emulation layer REGAL_EMU_VAO");
          vao = new Emu::Vao;
          emuLevel = 2;
          vao->Init(*this);
        }
      }
      #endif /* REGAL_EMU_VAO */
      Emu::SetVaoEmuInfo( enable, this->emuInfo, layer );
    }
    {
      Emu::LayerInfo layer;
      Emu::GetDsaLayerInfo( *this, layer );
      bool enable = false;
      #if REGAL_EMU_DSA
      if( Config::enableEmuDsa ) {
        enable = layer.emulationSupported && ( layer.emulationNeeded || Config::forceEmuDsa );
        if( enable ) {
          Info("Activating emulation layer REGAL_EMU_DSA");
          dsa = new Emu::Dsa;
          emuLevel = 3;
          dsa->Init(*this);
        }
      }
      #endif /* REGAL_EMU_DSA */
      Emu::SetDsaEmuInfo( enable, this->emuInfo, layer );
    }
    {
      Emu::LayerInfo layer;
      Emu::GetSoLayerInfo( *this, layer );
      bool enable = false;
      #if REGAL_EMU_SO
      if( Config::enableEmuSo ) {
        enable = layer.emulationSupported && ( layer.emulationNeeded || Config::forceEmuSo );
        if( enable ) {
          Info("Activating emulation layer REGAL_EMU_SO");
          so = new Emu::So;
          emuLevel = 4;
          so->Init(*this);
        }
      }
      #endif /* REGAL_EMU_SO */
      Emu::SetSoEmuInfo( enable, this->emuInfo, layer );
    }
    {
      Emu::LayerInfo layer;
      Emu::GetQuadsLayerInfo( *this, layer );
      bool enable = false;
      #if REGAL_EMU_QUADS
      if( Config::enableEmuQuads ) {
        enable = layer.emulationSupported && ( layer.emulationNeeded || Config::forceEmuQuads );
        if( enable ) {
          Info("Activating emulation layer REGAL_EMU_QUADS");
          quads = new Emu::Quads;
          emuLevel = 5;
          quads->Init(*this);
        }
      }
      #endif /* REGAL_EMU_QUADS */
      Emu::SetQuadsEmuInfo( enable, this->emuInfo, layer );
    }
    {
      Emu::LayerInfo layer;
      Emu::GetIffLayerInfo( *this, layer );
      bool enable = false;
      #if REGAL_EMU_IFF
      if( Config::enableEmuIff ) {
        enable = layer.emulationSupported && ( layer.emulationNeeded || Config::forceEmuIff );
        if( enable ) {
          Info("Activating emulation layer REGAL_EMU_IFF");
          iff = new Emu::Iff;
          emuLevel = 6;
          iff->Init(*this);
        }
      }
      #endif /* REGAL_EMU_IFF */
      Emu::SetIffEmuInfo( enable, this->emuInfo, layer );
    }
    {
      Emu::LayerInfo layer;
      Emu::GetRectLayerInfo( *this, layer );
      bool enable = false;
      #if REGAL_EMU_RECT
      if( Config::enableEmuRect ) {
        enable = layer.emulationSupported && ( layer.emulationNeeded || Config::forceEmuRect );
        if( enable ) {
          Info("Activating emulation layer REGAL_EMU_RECT");
          rect = new Emu::Rect;
          emuLevel = 7;
          rect->Init(*this);
        }
      }
      #endif /* REGAL_EMU_RECT */
      Emu::SetRectEmuInfo( enable, this->emuInfo, layer );
    }
    {
      Emu::LayerInfo layer;
      Emu::GetBaseVertexLayerInfo( *this, layer );
      bool enable = false;
      #if REGAL_EMU_BASEVERTEX
      if( Config::enableEmuBaseVertex ) {
        enable = layer.emulationSupported && ( layer.emulationNeeded || Config::forceEmuBaseVertex );
        if( enable ) {
          Info("Activating emulation layer REGAL_EMU_BASEVERTEX");
          bv = new Emu::BaseVertex;
          emuLevel = 8;
          bv->Init(*this);
        }
      }
      #endif /* REGAL_EMU_BASEVERTEX */
      Emu::SetBaseVertexEmuInfo( enable, this->emuInfo, layer );
    }
    {
      Emu::LayerInfo layer;
      Emu::GetTexStoLayerInfo( *this, layer );
      bool enable = false;
      #if REGAL_EMU_TEXSTO
      if( Config::enableEmuTexSto ) {
        enable = layer.emulationSupported && ( layer.emulationNeeded || Config::forceEmuTexSto );
        if( enable ) {
          Info("Activating emulation layer REGAL_EMU_TEXSTO");
          texsto = new Emu::TexSto;
          emuLevel = 9;
          texsto->Init(*this);
        }
      }
      #endif /* REGAL_EMU_TEXSTO */
      Emu::SetTexStoEmuInfo( enable, this->emuInfo, layer );
    }
    {
      Emu::LayerInfo layer;
      Emu::GetXferLayerInfo( *this, layer );
      bool enable = false;
      #if REGAL_EMU_XFER
      if( Config::enableEmuXfer ) {
        enable = layer.emulationSupported && ( layer.emulationNeeded || Config::forceEmuXfer );
        if( enable ) {
          Info("Activating emulation layer REGAL_EMU_XFER");
          xfer = new Emu::Xfer;
          emuLevel = 10;
          xfer->Init(*this);
        }
      }
      #endif /* REGAL_EMU_XFER */
      Emu::SetXferEmuInfo( enable, this->emuInfo, layer );
    }
    {
      Emu::LayerInfo layer;
      Emu::GetBinLayerInfo( *this, layer );
      bool enable = false;
      #if REGAL_EMU_BIN
      if( Config::enableEmuBin ) {
        enable = layer.emulationSupported && ( layer.emulationNeeded || Config::forceEmuBin );
        if( enable ) {
          Info("Activating emulation layer REGAL_EMU_BIN");
          bin = new Emu::Bin;
          emuLevel = 11;
          bin->Init(*this);
        }
      }
      #endif /* REGAL_EMU_BIN */
      Emu::SetBinEmuInfo( enable, this->emuInfo, layer );
    }
    {
      Emu::LayerInfo layer;
      Emu::GetPpcaLayerInfo( *this, layer );
      bool enable = false;
      #if REGAL_EMU_PPCA
      if( Config::enableEmuPpca ) {
        enable = layer.emulationSupported && ( layer.emulationNeeded || Config::forceEmuPpca );
        if( enable ) {
          Info("Activating emulation layer REGAL_EMU_PPCA");
          ppca = new Emu::Ppca;
          emuLevel = 12;
          ppca->Init(*this);
        }
      }
      #endif /* REGAL_EMU_PPCA */
      Emu::SetPpcaEmuInfo( enable, this->emuInfo, layer );
    }
    {
      Emu::LayerInfo layer;
      Emu::GetPpaLayerInfo( *this, layer );
      bool enable = false;
      #if REGAL_EMU_PPA
      if( Config::enableEmuPpa ) {
        enable = layer.emulationSupported && ( layer.emulationNeeded || Config::forceEmuPpa );
        if( enable ) {
          Info("Activating emulation layer REGAL_EMU_PPA");
          ppa = new Emu::Ppa;
          emuLevel = 13;
          ppa->Init(*this);
        }
      }
      #endif /* REGAL_EMU_PPA */
      Emu::SetPpaEmuInfo( enable, this->emuInfo, layer );
    }
    {
      Emu::LayerInfo layer;
      Emu::GetHintLayerInfo( *this, layer );
      bool enable = false;
      #if REGAL_EMU_HINT
      if( Config::enableEmuHint ) {
        enable = layer.emulationSupported && ( layer.emulationNeeded || Config::forceEmuHint );
        if( enable ) {
          Info("Activating emulation layer REGAL_EMU_HINT");
          hint = new Emu::Hint;
          emuLevel = 14;
          hint->Init(*this);
        }
      }
      #endif /* REGAL_EMU_HINT */
      Emu::SetHintEmuInfo( enable, this->emuInfo, layer );
    }
    {
      Emu::LayerInfo layer;
      Emu::GetObjLayerInfo( *this, layer );
      bool enable = false;
      #if REGAL_EMU_OBJ
      if( Config::enableEmuObj ) {
        enable = layer.emulationSupported && ( layer.emulationNeeded || Config::forceEmuObj );
        if( enable ) {
          Info("Activating emulation layer REGAL_EMU_OBJ");
          obj = new Emu::Obj;
          emuLevel = 15;
          obj->Init(*this);
        }
      }
      #endif /* REGAL_EMU_OBJ */
      Emu::SetObjEmuInfo( enable, this->emuInfo, layer );
    }
    emuLevel = 16;

  }
#endif

#if REGAL_CODE
  if (Config::enableCode && !codeSource && !codeHeader)
  {
    if (Config::codeSourceFile.length())
    {
      codeSource = fopen(Config::codeSourceFile.c_str(),"wt");
      if (!codeSource)
        Warning("Failed to open file ",Config::codeSourceFile," for writing code source.");
    }
    if (Config::codeHeaderFile.length())
    {
      if (Config::codeHeaderFile==Config::codeSourceFile)
        codeHeader = codeSource;
      else
        codeHeader = fopen(Config::codeHeaderFile.c_str(),"wt");
      if (!codeHeader)
        Warning("Failed to open file ",Config::codeHeaderFile," for writing code header.");
    }
  }
#endif

#if REGAL_HTTP
  http.Init( this );
#endif

  initialized = true;
}

// Note that Cleanup() may or may not have been called prior to destruction
RegalContext::~RegalContext()
{
  Internal("RegalContext::~RegalContext","()");

  #if REGAL_STATISTICS
  if (statistics && !Logging::frameStatistics)
  {
    statistics->log();
    statistics->reset();
  }
  #endif

  // Remove this context from the share group.

  shareGroup->remove(this);

#if REGAL_CODE
  if (codeSource)
    fclose(codeSource);

  if (codeHeader)
    fclose(codeHeader);
#endif
}

// Called prior to deletion, if this context is still set for this thread.
// Need to:
// 1) clean up GL state we've modified
// 2) leave the RegalContext in a state where Init() could be called again
void
RegalContext::Cleanup()
{
  Internal("RegalContext::Cleanup","()");

#if REGAL_EMULATION
  // emu
  #if REGAL_EMU_OBJ
  if (obj)
  {
    emuLevel = 15;
    obj->Cleanup(*this);
    obj.reset(NULL);
  }
  #endif /* REGAL_EMU_OBJ */
  #if REGAL_EMU_HINT
  if (hint)
  {
    emuLevel = 14;
    hint->Cleanup(*this);
    hint.reset(NULL);
  }
  #endif /* REGAL_EMU_HINT */
  #if REGAL_EMU_PPA
  if (ppa)
  {
    emuLevel = 13;
    ppa->Cleanup(*this);
    ppa.reset(NULL);
  }
  #endif /* REGAL_EMU_PPA */
  #if REGAL_EMU_PPCA
  if (ppca)
  {
    emuLevel = 12;
    ppca->Cleanup(*this);
    ppca.reset(NULL);
  }
  #endif /* REGAL_EMU_PPCA */
  #if REGAL_EMU_BIN
  if (bin)
  {
    emuLevel = 11;
    bin->Cleanup(*this);
    bin.reset(NULL);
  }
  #endif /* REGAL_EMU_BIN */
  #if REGAL_EMU_XFER
  if (xfer)
  {
    emuLevel = 10;
    xfer->Cleanup(*this);
    xfer.reset(NULL);
  }
  #endif /* REGAL_EMU_XFER */
  #if REGAL_EMU_TEXSTO
  if (texsto)
  {
    emuLevel = 9;
    texsto->Cleanup(*this);
    texsto.reset(NULL);
  }
  #endif /* REGAL_EMU_TEXSTO */
  #if REGAL_EMU_BASEVERTEX
  if (bv)
  {
    emuLevel = 8;
    bv->Cleanup(*this);
    bv.reset(NULL);
  }
  #endif /* REGAL_EMU_BASEVERTEX */
  #if REGAL_EMU_RECT
  if (rect)
  {
    emuLevel = 7;
    rect->Cleanup(*this);
    rect.reset(NULL);
  }
  #endif /* REGAL_EMU_RECT */
  #if REGAL_EMU_IFF
  if (iff)
  {
    emuLevel = 6;
    iff->Cleanup(*this);
    iff.reset(NULL);
  }
  #endif /* REGAL_EMU_IFF */
  #if REGAL_EMU_QUADS
  if (quads)
  {
    emuLevel = 5;
    quads->Cleanup(*this);
    quads.reset(NULL);
  }
  #endif /* REGAL_EMU_QUADS */
  #if REGAL_EMU_SO
  if (so)
  {
    emuLevel = 4;
    so->Cleanup(*this);
    so.reset(NULL);
  }
  #endif /* REGAL_EMU_SO */
  #if REGAL_EMU_DSA
  if (dsa)
  {
    emuLevel = 3;
    dsa->Cleanup(*this);
    dsa.reset(NULL);
  }
  #endif /* REGAL_EMU_DSA */
  #if REGAL_EMU_VAO
  if (vao)
  {
    emuLevel = 2;
    vao->Cleanup(*this);
    vao.reset(NULL);
  }
  #endif /* REGAL_EMU_VAO */
  #if REGAL_EMU_TEXC
  if (texc)
  {
    emuLevel = 1;
    texc->Cleanup(*this);
    texc.reset(NULL);
  }
  #endif /* REGAL_EMU_TEXC */
  #if REGAL_EMU_FILTER
  if (filt)
  {
    emuLevel = 0;
    filt->Cleanup(*this);
    filt.reset(NULL);
  }
  #endif /* REGAL_EMU_FILTER */
#endif

  initialized = false;
}

bool
RegalContext::groupInitialized() const
{
  Internal("RegalContext::groupInitialized","()");

  // The first context is always the first initialized context in the group.
  return shareGroup->front()->initialized;
}

void RegalContext::parkContext( DispatchTableGlobal & tbl )
{
  #if REGAL_SYS_OSX
  tbl.call(&tbl.CGLSetCurrentContext)( NULL );
  #else
  # error "Implement me!"
  #endif
  RegalMakeCurrent(NULL);
}

void RegalContext::unparkContext( DispatchTableGlobal & tbl )
{
  #if REGAL_SYS_OSX
  tbl.call(&tbl.CGLSetCurrentContext)( reinterpret_cast<CGLContextObj>(sysCtx) );
  #else
  # error "Implement me!"
  #endif
  RegalMakeCurrent(sysCtx);
}

REGAL_NAMESPACE_END
