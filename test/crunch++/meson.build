libCrunchppTestsNorm = ['testArgsParser', 'testCrunch++', 'testBad']
libCrunchppTestsExcept = ['testTester']
libCrunchppTests = libCrunchppTestsNorm + libCrunchppTestsExcept

libCrunchppPath = join_paths([meson.build_root(), libCrunchpp.outdir()])

foreach test : libCrunchppTests
	custom_target(
		'crunch++-' + test,
		command: [crunchMake, '-s', '@INPUT@', '-o', '@OUTPUT@', '-I' + crunchppSrcDir, '-L' + libCrunchppPath],
		input: test + '.cpp',
		output: test + '.so',
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
