project(
	'crunch',
	['c', 'cpp'],
	default_options: [
		'c_std=c11',
		'cpp_std=c++11',
		'warning_level=3',
		'b_pch=false',
		#'b_lto=true',
		'buildtype=release',
		'b_ndebug=if-release'
	],
	version: '1.1.1',
	meson_version: '>=0.56'
)

cc = meson.get_compiler('c')
cxx = meson.get_compiler('cpp')
threading = dependency('threads')
pkgconfig = import('pkgconfig')

sanitizer = get_option('b_sanitize')
coverage = get_option('b_coverage')
noUndefined = get_option('b_lundef')
debug = get_option('debug')
optimization = get_option('optimization')
prefix = get_option('prefix')
libdir = join_paths(prefix, get_option('libdir'))
isWindows = target_machine.system() == 'windows'
isMSVC = cxx.get_id() == 'msvc'
dl = cxx.find_library('dl', required: not isWindows)
libm = cxx.find_library('m', required: not isWindows)
c11Threading = cc.get_define('__STD_NO_THREADS__') == '' and cc.check_header('threads.h')
projectInclude = include_directories('.')
substrate = include_directories('substrate')
substrateDir = meson.current_source_dir() / 'substrate'

if cxx.get_id() == 'clang' and sanitizer.contains('undefined')
	assert(not noUndefined, 'Cannot combine b_lundef=true and b_sanitize=undefined')
endif

# Enable code coverage of the runner binaries and shared libraries - see
# https://docs.jade.fyi/gnu/gcc/gcc.html#index-coverage for more details
if ['gcc', 'clang'].contains(cc.get_id()) and coverage and debug
	add_project_arguments('--coverage', language: 'c')
endif
if ['gcc', 'clang'].contains(cxx.get_id()) and coverage and debug
	add_project_arguments('--coverage', language: 'cpp')
endif

libArgs = []
if isWindows
	libArgs += '-D__crunch_lib__'
	add_project_arguments('-DWIN32', '-D_WIN32', '-D_WINDOWS', language: ['c', 'cpp'])
	add_project_arguments(
		'-D_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1',
		'-D_CRT_NONSTDC_NO_WARNINGS=1',
		language: 'cpp'
	)
	if cxx.get_id() == 'clang'
		add_project_arguments('-D_NO_CRT_STDIO_INLINE', language: 'cpp')
	endif
	if debug
		add_project_arguments('-D_DEBUG', language: ['c', 'cpp'])
	endif

	if target_machine.cpu_family() == 'x86_64' or target_machine.cpu_family() == 'aarch64'
		add_project_arguments('-D_WIN64', language: ['c', 'cpp'])
	endif

	# MinGW defines the SDK minimum target to Server 2003
	# Console APIs require Windows 10 17763 (Redstone 5)
	if cxx.get_id() == 'gcc' or cxx.get_id() == 'clang'
		add_project_arguments(
			'-DWINVER=NTDDI_WIN10_RS5',
			'-D_WIN32_WINNT=NTDDI_WIN10_RS5',
			'-DNTDDI_VERSION=NTDDI_WIN10_RS5',
			language: ['c', 'cpp']
		)
	endif

	if isMSVC
		coverageRunner = find_program('OpenCppCoverage', required: false)
		coverage = coverageRunner.found() and debug
	endif

	if coverage
		coverageArgs = [
			'--sources', '@0@\crunch*\*'.format(meson.current_source_dir()),
			'--sources', '@0@\*'.format(meson.current_build_dir()),
			'--modules', meson.current_build_dir(),
			'--export_type'
		]
		coverageTarget = find_program('runCoverageTarget.py')
	endif
else
	add_project_arguments(
		'-D_GNU_SOURCE',
		'-D__USE_XOPEN_EXTENDED',
		language: ['c', 'cpp']
	)
endif

if isMSVC
	add_project_arguments(
		'-wd4996',
		'-wd4800',
		'-Zc:__cplusplus',
		language: ['c', 'cpp']
	)
endif

subdir('crunch')
subdir('crunch++')
subdir('crunchMake')
subdir('man')
subdir('test')

runClangTidy = find_program('runClangTidy.py')
run_target(
	'clang-tidy',
	command: [runClangTidy, '-s', meson.current_source_dir(), '-p', meson.current_build_dir()]
)
