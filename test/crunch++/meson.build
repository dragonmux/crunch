libCrunchppTestsNorm = ['testArgsParser', 'testCrunch++', 'testBad', 'testRegistration', 'testLogger']
libCrunchppTestsExcept = ['testTester']
libCrunchppTests = libCrunchppTestsNorm + libCrunchppTestsExcept

libCrunchppPath = join_paths([meson.build_root(), libCrunchpp.outdir()])

foreach test : libCrunchppTests
	command = [crunchMake, '-s', '@INPUT@', '-o', '@OUTPUT@', '-I' + crunchppSrcDir, '-L' + libCrunchppPath]
	testExtra = []
	if test == 'testCrunch++' and cxx.has_argument('-std=c++17')
		testExtra = ['-std=c++17']
	elif test == 'testArgsParser'
		testExtra = ['-I' + substrateDir]
	endif
	custom_target(
		'crunch++-' + test,
		command: command + commandExtra + testExtra,
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
	'crunch++-except',
	crunchpp,
	args: libCrunchppTestsExcept,
	workdir: meson.current_build_dir(),
	should_fail: true
)

test(
	'crunch++-empty',
	crunchpp,
	workdir: meson.current_build_dir(),
	should_fail: true
)
