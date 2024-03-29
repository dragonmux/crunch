# SPDX-License-Identifier: LGPL-3.0-or-later

libCrunchppSrc = [
	'argsParser.cxx', 'stringFuncs.cxx', 'logger.cxx', 'tester.cxx', 'core.cxx'
]
crunchppSrc = ['crunch++.cpp']
crunchppSrcDir = meson.current_source_dir()
crunchppInc = include_directories('.')

config = configuration_data()
config.set('AUTOGEN_HEADER', '/* THIS FILE IS AUTOGENERATED, DO NOT EDIT */')
config.set('VERSION', '@VERSION@')
config.set('COMPILER', cc.get_id())
config.set('COMPILER_VERSION', cc.version())
config.set('TARGET_SYS', target_machine.system())
config.set('TARGET_ARCH', target_machine.cpu())

versionHeaderSrc = configure_file(
	configuration: config,
	input: 'version.hxx.in',
	output: 'version.hxx.in'
)

versionHeader = vcs_tag(
	input: versionHeaderSrc,
	output: 'version.hxx',
	replace_string: '@VERSION@',
	fallback: 'v@0@'.format(meson.project_version())
)

libCrunchpp = shared_library(
	'libcrunch++',
	libCrunchppSrc,
	name_prefix: '',
	cpp_args: libArgs,
	dependencies: [threading, substrate],
	version: meson.project_version(),
	install: true
)

libCrunchppDep = declare_dependency(
	link_with: libCrunchpp,
	include_directories: crunchppInc,
	variables: {
		'compile_args': ' '.join([
			'-I@0@'.format(meson.current_build_dir()),
			'-I@0@'.format(meson.current_source_dir()),
		]),
		'link_args': libCrunchpp.full_path(),
	}
)

crunchpp = executable(
	'crunch++',
	crunchppSrc + [versionHeader],
	dependencies: [threading, dl, substrate, libCrunchppDep],
	install: true,
	install_rpath: libdir
)

pkgconfig.generate(
	libCrunchpp,
	name: 'crunch++',
	filebase: 'crunch++',
	version: meson.project_version(),
	description: 'crunch++ is a unit testing framework for C++',
	unescaped_variables: {
		'compile_args': '-I${includedir}',
		'link_args': '-L${libdir} -lcrunch++',
	},
)

install_headers('crunch++.h')

meson.override_find_program('crunch++', crunchpp)
meson.override_dependency('crunch++', libCrunchppDep)
