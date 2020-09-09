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
if cxx.get_id() != 'msvc' and noUndefined
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

subdir('ranlux')
subdir('crunch')
subdir('crunch++')
subdir('crunchMake')

if not isWindows and not coverage
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
