project(
	'crunch',
	['c', 'cpp'],
	default_options: [
		'c_std=c11',
		'cpp_std=c++11',
		'warning_level=3',
		'b_pch=false',
		'buildtype=release'
	],
	version: '0.3.2'
)

cc = meson.get_compiler('c')
cxx = meson.get_compiler('cpp')
threading = dependency('threads')
pkgconfig = import('pkgconfig')

coverage = get_option('b_coverage')
debug = get_option('debug')
prefix = get_option('prefix')
libdir = join_paths(prefix, get_option('libdir'))
isWindows = target_machine.system() == 'windows'
dl = cxx.find_library('dl', required: not isWindows)

if ['gcc', 'clang'].contains(cc.get_id())
	add_project_arguments('-Wall', '-Wextra', '-pedantic', language: 'c')
	if coverage and debug
		add_project_arguments('--coverage', language: 'c')
	endif
endif

if ['gcc', 'clang'].contains(cxx.get_id())
	add_project_arguments('-Wall', '-Wextra', '-pedantic', language: 'cpp')
	if coverage and debug
		add_project_arguments('--coverage', language: 'cpp')
	endif
endif

libArgs = []
if isWindows
	libArgs += '-D__crunch_lib__'
	add_project_arguments('-DWIN32', '-D_WINDOWS', language: ['c', 'cpp'])
	add_project_arguments(
		'-D_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1',
		'-D_CRT_NONSTDC_NO_WARNINGS=1',
		language: 'cpp')
	if cxx.get_id() == 'clang'
		add_project_arguments('-D_NO_CRT_STDIO_INLINE', language: 'cpp')
	endif
	if debug
		add_project_arguments('-D_DEBUG', language: ['c', 'cpp'])
	endif
endif

if cxx.get_id() == 'msvc'
	add_project_arguments('-wd4996', '-wd4800', language: ['c', 'cpp'])
endif

subdir('crunch')
subdir('crunch++')
subdir('crunchMake')
subdir('test')
