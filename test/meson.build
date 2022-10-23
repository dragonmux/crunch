if not isWindows
	testExt = '.so'
else
	testExt = '.dll'
endif

commandExtra = []
if coverage
	commandExtra += ['--coverage']
endif
if debug
	commandExtra += ['--debug']
endif
if not isWindows and cxx.get_linker_id() != 'ld64' and noUndefined
	commandExtra += ['-z', 'defs']
endif

if sanitizer != 'none'
	commandExtra += ['-fsanitize=' + sanitizer]
	if sanitizer.contains('address')
		commandExtra += ['-DCRUNCH_ASAN']
	endif
endif
if not debug or optimization not in ['0', '1']
	commandExtra += ['-DNO_ALLOC_TEST']
endif

if isWindows
	commandExtra += ['-DWIN32', '-D_WINDOWS']

	# Substrate API needs Windows 10
	# https://docs.microsoft.com/en-us/cpp/porting/modifying-winver-and-win32-winnt?view=msvc-160
	if cxx.get_id() == 'gcc' or cxx.get_id() == 'clang'
		commandExtra += ['-DWINVER=NTDDI_WIN10_RS5', '-D_WIN32_WINNT=NTDDI_WIN10_RS5', '-DNTDDI_VERSION=NTDDI_WIN10_RS5']
	endif
endif

subdir('ranlux')
subdir('crunch')
subdir('crunch++')
subdir('crunchMake')

if not isMSVC or not coverage
test(
	'crunchMake-none',
	crunchMake,
	workdir: meson.current_build_dir(),
	should_fail: true
)
else
test(
	'crunchMake-none',
	coverageRunner,
	args: coverageArgs + ['cobertura:crunchMake-none-coverage.xml', '--', crunchMake],
	workdir: meson.current_build_dir(),
	should_fail: true
)
endif
