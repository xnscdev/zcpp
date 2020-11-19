project('zcpp', 'cpp', version: '0.1', license: 'GPL-3.0-or-later',
	default_options: 'cpp_std=gnu++17')

zcpp_conf = configuration_data()
zcpp_conf.set('PROJECT_VERSION', meson.project_version())
configure_file(input: 'config.h.in', output: 'config.h',
	       configuration: zcpp_conf)

zcpp_src = [
  'comments.cc',
  'console.cc',
  'define.cc',
  'directive.cc',
  'entry.cc',
  'include.cc',
  'util.cc',
  'zcpp.cc'
]
executable('zcpp', zcpp_src, install: true)
