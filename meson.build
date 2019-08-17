project(
	'crunch',
	['c', 'cpp'],
	default_options: [
		'c_std=c99',
		'cpp_std=c++11',
		'warning_level=3',
		'b_pch=false',
		'buildtype=release'
	],
	version: '0.3.0'
)

cc = meson.get_compiler('c')
cxx = meson.get_compiler('cpp')
threading = dependency('threads')

coverage = get_option('b_coverage')
buildType = get_option('buildtype')
isWindows = target_machine.system() == 'windows'

if ['gcc', 'clang'].contains(cc.get_id())
	add_project_arguments('-Wall', '-Wextra', '-pedantic', language: 'c')
	if coverage and buildType == 'debug'
		add_project_arguments('--coverage', language: 'c')
	endif
endif

if ['gcc', 'clang'].contains(cxx.get_id())
	add_project_arguments('-Wall', '-Wextra', '-pedantic', language: 'cpp')
	if coverage and buildType == 'debug'
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
	if buildType == 'debug'
		add_project_arguments('-D_DEBUG', language: ['c', 'cpp'])
	endif
endif

if cxx.get_id() == 'msvc'
	add_project_arguments('-wd4996', '-wd4800', '-wd4459', language: ['c', 'cpp'])
endif

if not isWindows
	subdir('crunch')
endif
subdir('crunch++')
subdir('crunchMake')
subdir('test')
