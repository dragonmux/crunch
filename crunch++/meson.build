dl = cxx.find_library('dl')

libCrunchppSrc = [
	'ArgsParser.cpp', 'StringFuncs.cpp', 'Logger.cpp', 'Tester.cpp', 'Core.cpp'
]
crunchppSrc = ['crunch++.cpp']
crunchppInc = include_directories('.')

libCrunchpp = library(
	'crunch++',
	libCrunchppSrc,
	dependencies: [threading],
	version: meson.project_version()
)

crunchpp = executable(
	'crunch++',
	crunchppSrc,
	link_with: [libCrunchpp],
	dependencies: [threading, dl]
)
