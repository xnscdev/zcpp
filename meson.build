project('zcpp', 'cpp', version: '0.1', license: 'GPL-3.0-or-later',
	default_options: 'cpp_std=gnu++17')

# C++ 17 deprecates use of `register' but Flex uses it
cxx = meson.get_compiler('cpp')
if cxx.has_argument('-Wdeprecated-register')
  add_project_arguments('-Wno-deprecated-register', language: 'cpp')
endif
if cxx.has_argument('-Wregister')
  add_project_arguments('-Wno-register', language: 'cpp')
endif

assert(cxx.has_function('fmemopen'))

bison = find_program('bison', required: true)
flex = find_program('flex', required: true)

bison_gen = generator(bison, output: ['@BASENAME@.cc', '@BASENAME@.hh'],
		      arguments: ['-d', '-Wno-other', '-Wcex', '-o',
				  '@OUTPUT0@', '@INPUT@'])
flex_gen = generator(flex, output: '@BASENAME@.cc',
		     arguments: ['-o', '@OUTPUT@', '@INPUT@'])

if_parser = bison_gen.process('if-parser.yy')
if_scanner = flex_gen.process('if-scanner.ll')

zcpp_conf = configuration_data()

zcpp_conf.set('PROJECT_VERSION', meson.project_version())

if host_machine.cpu_family() == 'x86'
  zcpp_conf.set('ARCH_MACRO', '__i386__')
elif host_machine.cpu_family() == 'x86_64'
  zcpp_conf.set('ARCH_MACRO', '__x86_64__')
elif host_machine.cpu_family() == 'arm'
  zcpp_conf.set('ARCH_MACRO', '__arm__')
elif host_machine.cpu_family() == 'aarch64'
  zcpp_conf.set('ARCH_MACRO', '__arm64__')
endif

zcpp_conf.set('SIZEOF_INT', cxx.sizeof('int'))
zcpp_conf.set('SIZEOF_LONG', cxx.sizeof('long'))
zcpp_conf.set('SIZEOF_LONG_LONG', cxx.sizeof('long long'))
zcpp_conf.set('SIZEOF_SHORT', cxx.sizeof('short'))
zcpp_conf.set('SIZEOF_POINTER', cxx.sizeof('void *'))
zcpp_conf.set('SIZEOF_FLOAT', cxx.sizeof('float'))
zcpp_conf.set('SIZEOF_DOUBLE', cxx.sizeof('double'))
zcpp_conf.set('SIZEOF_LONG_DOUBLE', cxx.sizeof('long double'))
zcpp_conf.set('SIZEOF_SIZE_T',
	      cxx.sizeof('size_t', prefix: '#include <stddef.h>'))
zcpp_conf.set('SIZEOF_WCHAR_T',
	      cxx.sizeof('wchar_t', prefix: '#include <wchar.h>'))
zcpp_conf.set('SIZEOF_WINT_T',
	      cxx.sizeof('wint_t', prefix: '#include <wchar.h>'))
zcpp_conf.set('SIZEOF_PTRDIFF_T',
	      cxx.sizeof('ptrdiff_t', prefix: '#include <stddef.h>'))

configure_file(input: 'config.h.in', output: 'config.h',
	       configuration: zcpp_conf)

zcpp_src = [
  'comments.cc',
  'console.cc',
  'define.cc',
  'directive.cc',
  'entry.cc',
  if_parser,
  if_scanner,
  'include.cc',
  'util.cc',
  'zcpp.cc'
]
executable('zcpp', zcpp_src, install: true)
