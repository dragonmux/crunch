libCrunchppTestsNorm = ['testCrunch++', 'testBad', 'testRegistration', 'testLogger']
libCrunchppTestsExcept = ['testTester']
libCrunchppTests = libCrunchppTestsNorm + libCrunchppTestsExcept

if isMSVC and cxx.version().version_compare('< 19.30.30704')
	# MSVC 2019 crashes on testArgsParser
	# https://developercommunity.visualstudio.com/t/Crash-on-constexpr-creation-of-a-std::ar/1547919
	warning('Skipping testArgsParser test as it crashes MSVC')
else
	libCrunchppTestsNorm += ['testArgsParser']
endif

libCrunchppPath = join_paths([meson.project_build_root(), libCrunchpp.outdir()])

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
		testExtra = [substrate.get_variable('compile_args')]
	endif
	objects = []
	if test == 'testLogger' and not isWindows
		libSubstrate = substrate.get_variable('link_args')
		if libSubstrate.endswith('/libsubstrate.so')
			substrateDir = libSubstrate.substring(0, -16)
		elif libSubstrate.endswith('/libsubstrate.a')
			substrateDir = libSubstrate.substring(0, -15)
		else
			assert('Unexpected value for substrate link_args, got \'@0@\''.format(libSubstrate))
		endif
		testExtra += [libSubstrate, '-Wl,-rpath,@0@'.format(substrateDir)]
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
