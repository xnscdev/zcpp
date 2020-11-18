project('zcpp', 'cpp', version: '0.1', license: 'GPL-3.0-or-later')

zcpp_conf = configuration_data()
zcpp_conf.set('PROJECT_VERSION', meson.project_version())
zcpp_conf.set('MAX_NESTED_IFS', get_option('max_nested_ifs'))
configure_file(input: 'config.h.in', output: 'config.h',
	       configuration: zcpp_conf)

zcpp_src = [
  'comments.cc',
  'console.cc',
  'directive.cc',
  'entry.cc',
  'include.cc',
  'util.cc',
  'zcpp.cc'
]
executable('zcpp', zcpp_src, install: true)
