dl = cxx.find_library('dl', required: not isWindows)

libCrunchppSrc = [
	'ArgsParser.cpp', 'StringFuncs.cpp', 'Logger.cpp', 'Tester.cpp', 'Core.cpp'
]
crunchppSrc = ['crunch++.cpp']
crunchppSrcDir = meson.current_source_dir()
crunchppInc = include_directories('.')

libCrunchpp = library(
	'crunch++',
	libCrunchppSrc,
	cpp_args: libArgs,
	dependencies: [threading],
	version: meson.project_version(),
	install: true
)

crunchpp = executable(
	'crunch++',
	crunchppSrc,
	link_with: [libCrunchpp],
	dependencies: [threading, dl],
	install: true
)

install_headers('crunch++.h')
