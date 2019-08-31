libCrunchppTestsNorm = ['testArgsParser', 'testCrunch++', 'testBad', 'testLogger']
libCrunchppTestsExcept = ['testTester']
libCrunchppTests = libCrunchppTestsNorm + libCrunchppTestsExcept

libCrunchppPath = join_paths([meson.build_root(), libCrunchpp.outdir()])

foreach test : libCrunchppTests
	command = [crunchMake, '-s', '@INPUT@', '-o', '@OUTPUT@', '-I' + crunchppSrcDir, '-L' + libCrunchppPath]
	if cxx.get_id() != 'msvc'
		command += ['-z', 'defs']
	endif
	if debug
		command += ['--debug']
	endif
	custom_target(
		'crunch++-' + test,
		command: command,
		input: test + '.cpp',
		output: test + testExt,
		depends: libCrunchpp,
		build_by_default: true
	)
endforeach

test(
	'crunch++',
	crunchpp,
	args: libCrunchppTestsNorm + ['testMustNotExist'],
	workdir: meson.current_build_dir()
)

test(
	'except',
	crunchpp,
	args: libCrunchppTestsExcept,
	workdir: meson.current_build_dir(),
	should_fail: true
)

test(
	'empty',
	crunchpp,
	workdir: meson.current_build_dir(),
	should_fail: true
)
