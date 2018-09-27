dl = cc.find_library('dl', required: not isWindows)

libCrunchSrc = [
	'ArgsParser.c', 'Memory.c', 'StringFuncs.c', 'Logger.c', 'Core.c'
]
crunchSrc = ['crunch.c']
crunchSrcDir = meson.current_source_dir()
crunchInc = include_directories('.')

crunchArgs = []
if isWindows
	crunchArgs += '-D__crunch_lib__'
endif

libCrunch = library(
	'crunch',
	libCrunchSrc,
	cpp_args: crunchArgs,
	dependencies: [threading],
	version: meson.project_version(),
	install: true
)

crunch = executable(
	'crunch',
	crunchSrc,
	link_with: [libCrunch],
	dependencies: [threading, dl],
	install: true
)
