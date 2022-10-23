libCrunchppTestsNorm = ['testArgsParser', 'testCrunch++', 'testBad', 'testRegistration', 'testLogger']
libCrunchppTestsExcept = ['testTester']
libCrunchppTests = libCrunchppTestsNorm + libCrunchppTestsExcept

libCrunchppPath = join_paths([meson.project_build_root(), libCrunchpp.outdir()])

if not isWindows
	testSrcs = [
		substrateDir / 'impl/pty.cxx'
	]

	testHelpers = static_library(
		'testHelpers',
		testSrcs,
		pic: true,
		link_with: [libCrunchpp],
		include_directories: [substrate],
		install: false,
		build_by_default: true
	)
endif

foreach test : libCrunchppTests
	command = [crunchMake, '-s', '@INPUT@', '-o', '@OUTPUT@', '-I' + crunchppSrcDir, '-L' + libCrunchppPath]
	if isMSVC and coverage
		command = [coverageTarget, coverageRunner] + coverageArgs + [
			'cobertura:crunchMake-crunch++-@0@-coverage.xml'.format(test), '--'
		] + command
	endif
	testExtra = []
	if test == 'testCrunch++' and cxx.has_argument('-std=c++17')
		testExtra = ['-std=c++17']
	elif test == 'testCrunch++' and cxx.has_argument('-std:c++17')
		testExtra = ['-std:c++17']
	elif test == 'testArgsParser' or test == 'testLogger'
		testExtra = ['-I' + substrateDir]
	endif
	objects = []
	if test == 'testLogger' and not isWindows
		objects = [testHelpers.extract_objects(substrateDir / 'impl/pty.cxx')]
	endif
	custom_target(
		'crunch++-' + test,
		command: command + commandExtra + testExtra,
		input: [test + '.cpp', objects],
		output: test + testExt,
		depends: libCrunchpp,
		build_by_default: true
	)
endforeach

if not isMSVC or not coverage
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
else
test(
	'crunch++',
	coverageRunner,
	args: coverageArgs + [
		'cobertura:crunch++-coverage.xml', '--', crunchpp
	] + libCrunchppTestsNorm + ['testMustNotExist'],
	workdir: meson.current_build_dir()
)

test(
	'crunch++-except',
	coverageRunner,
	args: coverageArgs + [
		'cobertura:crunch++-except-coverage.xml', '--', crunchpp
	] + libCrunchppTestsExcept,
	workdir: meson.current_build_dir(),
	should_fail: true
)

test(
	'crunch++-empty',
	coverageRunner,
	args: coverageArgs + ['cobertura:crunch++-empty-coverage.xml', '--', crunchpp],
	workdir: meson.current_build_dir(),
	should_fail: true
)
endif
