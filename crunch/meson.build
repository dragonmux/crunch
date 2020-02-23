libCrunchSrc = [
	'ArgsParser.c', 'StringFuncs.c', 'Logger.c', 'Core.c'
]
crunchSrc = ['crunch.c']
crunchSrcDir = meson.current_source_dir()
crunchInc = include_directories('.')

threadArgs = []
if c11Threading
	threadArgs += ['-DUSE_C11_THREADING']
else
	libCrunchSrc += ['threading/threadShim.c']
endif

libCrunch = shared_library(
	'libcrunch',
	libCrunchSrc,
	name_prefix: '',
	c_args: libArgs + threadArgs,
	dependencies: [threading, libm],
	version: meson.project_version(),
	install: true
)

crunch = executable(
	'crunch',
	crunchSrc,
	link_with: [libCrunch],
	dependencies: [threading, dl],
	install: true,
	install_rpath: libdir
)

pkgconfig.generate(
	libCrunch,
	name: 'crunch',
	filebase: 'crunch',
	version: meson.project_version(),
	description: 'crunch is a unit testing framework for C'
)

install_headers('crunch.h')
