if not isWindows
	testExt = '.so'
else
	testExt = '.tlib'
endif

commandExtra = []
if coverage
	commandExtra += ['--coverage']
endif
if debug
	commandExtra += ['--debug']
endif
if cxx.get_id() != 'msvc'
	commandExtra += ['-z', 'defs']
endif

if sanitizer != 'none'
	commandExtra += ['-fsanitize=' + sanitizer]
endif

subdir('ranlux')
subdir('crunch')
subdir('crunch++')
subdir('crunchMake')

test(
	'crunchMake-none',
	crunchMake,
	workdir: meson.current_build_dir(),
	should_fail: true
)
