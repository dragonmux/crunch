dl = cc.find_library('dl')

libCrunchSrc = [
	'ArgsParser.c', 'Memory.c', 'StringFuncs.c', 'Logger.c', 'Core.c'
]
crunchSrc = ['crunch.c']
crunchSrcDir = meson.current_source_dir()
crunchInc = include_directories('.')

libCrunch = library(
	'crunch',
	libCrunchSrc,
	dependencies: [threading],
	version: meson.project_version()
)

crunch = executable(
	'crunch',
	crunchSrc,
	link_with: [libCrunch],
	dependencies: [threading, dl]
)
