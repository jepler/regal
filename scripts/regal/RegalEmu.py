#!/usr/bin/python -B

from string import Template, upper, replace

from ApiUtil import outputCode
from ApiUtil import typeIsVoid

from ApiCodeGen import *

from RegalContext     import emu
from RegalContextInfo import cond

from Emu       import emuFindEntry, emuCodeGen, emuGetOriginateList, emuGetInterceptList

##############################################################################################

emuProcsHeaderTemplate = Template('''${AUTOGENERATED}
${LICENSE}

#ifndef ${HEADER_NAME}
#define ${HEADER_NAME}

#include "RegalUtil.h"

${IFDEF}REGAL_GLOBAL_BEGIN

#include "RegalPrivate.h"
#include "RegalContext.h"
#include "RegalDispatch.h"
${LOCAL_INCLUDE}

REGAL_GLOBAL_END

REGAL_NAMESPACE_BEGIN

${LOCAL_CODE}

REGAL_NAMESPACE_END

${ENDIF}
#endif // ${HEADER_NAME}
''')

emuProcsSourceTemplate = Template('''${AUTOGENERATED}
${LICENSE}

#include "RegalUtil.h"

${IFDEF}
REGAL_GLOBAL_BEGIN

#include "RegalPrivate.h"
#include "RegalContext.h"
#include "RegalDispatch.h"
${LOCAL_INCLUDE}

${CONSTRUCTOR}

REGAL_GLOBAL_END

REGAL_NAMESPACE_BEGIN

using namespace Emu;

${LOCAL_CODE}

REGAL_NAMESPACE_END

${ENDIF}
''')

def apiEmuProcsConstructor( e ):
  code = ''
  code +=     '\n'
  code +=     'extern "C" Regal::Layer * create%s( Regal::RegalContext * ctx ) {\n' % e['type']
  code +=     '  return new Regal::Emu::%s( ctx );\n' % e['type']
  code +=     '}\n'
  code +=     '\n'
  return code;

##############################################################################################

def apiEmuProcsHeaderCode( e, apis, orig ):
  code = ''

  code +=     'void %sIntercept( Layer *layer, Dispatch::GL & dt );\n\n' % e['type']

  o = emuGetOriginateList( e['formulae'], apis )
  for f in orig:
    if f not in o:
      o.append( f )
  o = sorted( o )

  if len(o) == 0:
    return code

  code +=     'struct %sOriginate {\n' % e['type']
  code +=     '\n'
  code +=     '  %sOriginate() {\n' % e['type']
  code +=     '    memset(this, 0, sizeof( *this ) );\n'
  code +=     '  }\n'
  code +=     '\n'
  for oe in o:
    code +=   '  REGAL%sPROC %s;' % ( oe.upper(), oe )
    code +=   '\n'
  code +=     '\n'
  code +=     '  void Initialize( Dispatch::GL & dt ) {\n'
  for oe in o:
    code +=   '    %s = dt.%s;' % ( oe, oe )
    code +=   '\n'
  code +=     '  }\n'
  code +=     '};\n\n'

  return code

def callAndReturn( e, function ):
  code = ''
  
  name   = function.name
  callParams = paramsNameCode(function.parameters, paramsPrefix="orig")
  rType  = typeCode(function.ret.type)

  if not typeIsVoid(rType):
    code += 'return '
  code += 'R%s( %s );\n' % ( name, callParams )
  
  return code

def apiEmuProcsSourceCode( e, apis, orig ):
  code = ''

  intercept = []
  ore = re.compile( "Rgl[A-Za-z0-9_]+" );
  sre = re.compile( "self->" );

  for api in apis:

    code += '\n'

    for function in sorted( api.functions, key = lambda function: function.name):
      if not function.needsContext:
        continue
      if getattr(function,'regalOnly',False)==True:
        continue

      name   = function.name
      params = paramsDefaultCode(function.parameters, True, paramsPrefix = "Layer *_layer")
      callParams = paramsNameCode(function.parameters)
      rType  = typeCode(function.ret.type)
      category  = getattr(function, 'category', None)
      version   = getattr(function, 'version', None)

      emue = emuFindEntry( function, e['formulae'], e['name'] )

      if not emue:
        continue

      intercept.append( name )

      code +=      '\nstatic %sREGAL_CALL %s%s(%s) \n{\n' % (rType, '%s_' % e['name'], name, params)

      body =       ''
      if emue != None and 'prefix' in emue and len(emue['prefix']):
        body +=    '  // prefix\n'
        body += listToString( indent( emue['prefix'], '  ' ) )
        if body.find("return") > 0:
          raise Exception("Cannot early return in prefix clause. - %s %s" % ( e['type'], name ) )

      elif emue != None and 'impl' in emue and len( emue['impl'] ):
        body +=    '  // impl\n'
        body += listToString( indent( emue['impl'], '  ' ) )
        if body.find("return") < 0:
          raise Exception("Must have at least one return in impl clause. - %s %s" % ( e['type'], name ) )


      body +=      '\n'
      body +=      '  ' + callAndReturn( e, function )
      body +=      '\n'
      body +=      '}\n'

      selfcalls = sre.findall( body )
      origcalls = ore.findall( body )
      if selfcalls or origcalls:
        code +=      '  %s * self = static_cast<%s *>(_layer);\n' % ( e['type'], e['type'] )
      code +=      '\n'
      if origcalls:
        for c in origcalls:
          c = c[1:]
          if c not in orig:
            orig.append( c )
        code +=      '  %sOriginate & orig = self->orig;\n' % e['type']
      code +=      '\n'

      code += body

  code +=     'void %sIntercept( Layer *layer, Dispatch::GL & dt ) {\n' % e['type']
  maxf = 0
  for f in intercept:
    maxf = max( maxf, len(f) )
  for f in sorted(intercept):
    spc = ' ' * ( maxf - len(f) )
    code +=     '  dt.%s%s = MakeRegalProc(%s_%s, layer);\n' % ( f, spc, e['name'], f )
  code +=     '}\n'
  return code


def generateEmuSource(apis, args):

  orig = {}
 
  for e in emu:
    if not e['formulae']:
      continue
    s = {}
    s['LICENSE']         = args.license
    s['AUTOGENERATED']   = args.generated
    s['COPYRIGHT']       = args.copyright
    origfuncs = orig[ e['type'] ] = []
    s['LOCAL_CODE']      = apiEmuProcsSourceCode( e, apis, origfuncs )
    s['LOCAL_INCLUDE']   = '#include "%s.h"\n#include "%sProcs.h"\n' % (e['type'],e['type'])
    s['CONSTRUCTOR']     = apiEmuProcsConstructor( e )
    s['API_DISPATCH_FUNC_DEFINE'] = ''
    s['API_DISPATCH_FUNC_INIT'] = ''
    s['API_DISPATCH_GLOBAL_FUNC_INIT'] = ''
    s['IFDEF'] = ''
    s['ENDIF'] = ''

    outputCode( '%s/layer/%s/%sProcs.cpp' % (args.srcdir, e['name'], e['type']), emuProcsSourceTemplate.substitute(s))

  for e in emu:
    if not e['formulae']:
      continue
    s = {}
    s['LICENSE']         = args.license
    s['AUTOGENERATED']   = args.generated
    s['COPYRIGHT']       = args.copyright
    s['HEADER_NAME']   = 'REGAL_LAYER_%s_PROCS_H' % e['type'].upper()
    origfuncs = orig[ e['type'] ]
    s['LOCAL_CODE']      =  apiEmuProcsHeaderCode( e, apis, origfuncs )
    s['LOCAL_INCLUDE']   = ''
    s['API_DISPATCH_FUNC_DEFINE'] = ''
    s['API_DISPATCH_FUNC_INIT'] = ''
    s['API_DISPATCH_GLOBAL_FUNC_INIT'] = ''
    s['IFDEF'] = ''
    s['ENDIF'] = ''

    outputCode( '%s/layer/%s/%sProcs.h' % (args.srcdir, e['name'], e['type']), emuProcsHeaderTemplate.substitute(s))


