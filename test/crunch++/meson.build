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

libCrunchppPath = meson.global_build_root() / libCrunchpp.outdir()

foreach test : libCrunchppTests
	shared_library(
		test,
		files(test + '.cpp'),
		name_prefix: '',
		dependencies: [libCrunchppDep, substrate],
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
