ranlux = static_library(
	'ranlux',
	['ranlux32.cxx', 'ranlux64.cxx', 'helpers.cxx'],
	pic: true,
	include_directories: [projectInclude, substrate],
	install: false,
	build_by_default: true
)

ranluxTests = ['testRanlux32', 'testRanlux64']
libCrunchppPath = join_paths([meson.project_build_root(), libCrunchpp.outdir()])
ranluxSrcDir = meson.current_source_dir()

objectMap = {
	'testRanlux32': {'test': ['ranlux32.cxx'], 'name': 'ranlux32'},
	'testRanlux64': {'test': [], 'name': 'ranlux64'}
}

foreach test : ranluxTests
	map = objectMap.get(test)
	testObjs = [ranlux.extract_objects(map['test'] + ['helpers.cxx', 'ranlux64.cxx'])]
	command = [crunchMake, '-s', '@INPUT@', '-o', '@OUTPUT@', '-I' + meson.project_source_root(), '-L' + libCrunchppPath]
	if isMSVC and coverage
		command = [coverageTarget, coverageRunner] + coverageArgs + [
			'cobertura:crunchMake-@0@-coverage.xml'.format(test), '--'
		] + command
	endif

	custom_target(
		test,
		command: command + commandExtra,
		input: [test + '.cxx'] + testObjs,
		output: test + testExt,
		depends: libCrunchpp,
		build_by_default: true
	)

	if not isMSVC or not coverage
	test(
		map['name'],
		crunchpp,
		args: test,
		workdir: meson.current_build_dir()
	)
	else
	test(
		map['name'],
		coverageRunner,
		args: coverageArgs + [
			'cobertura:@0@-coverage.xml'.format(map['name']), '--', crunchpp, test
		],
		workdir: meson.current_build_dir()
	)
	endif
endforeach
