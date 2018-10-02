dl = cc.find_library('dl', required: not isWindows)

libCrunchSrc = [
	'ArgsParser.c', 'Memory.c', 'StringFuncs.c', 'Logger.c', 'Core.c'
]
crunchSrc = ['crunch.c']
crunchSrcDir = meson.current_source_dir()
crunchInc = include_directories('.')

libCrunch = shared_library(
	'libcrunch',
	libCrunchSrc,
	name_prefix: '',
	cpp_args: libArgs,
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

install_headers('crunch.h')
