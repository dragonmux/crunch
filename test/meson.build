if not isWindows
	testExt = '.so'
else
	testExt = '.tlib'
endif
subdir('ranlux')
subdir('crunch')
subdir('crunch++')

test(
	'crunchMake-none',
	crunchMake,
	workdir: meson.current_build_dir(),
	should_fail: true
)
