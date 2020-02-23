ranlux = static_library(
	'ranlux',
	['ranlux32.cxx', 'ranlux64.cxx', 'helpers.cxx'],
	pic: true,
	include_directories: projectInclude,
	install: false,
	build_by_default: true
)

ranluxTests = ['testRanlux32', 'testRanlux64']
libCrunchppPath = join_paths([meson.build_root(), libCrunchpp.outdir()])
ranluxSrcDir = meson.current_source_dir()

objectMap = {
	'testRanlux32': {'test': ['ranlux32.cxx'], 'name': 'ranlux32'},
	'testRanlux64': {'test': [], 'name': 'ranlux64'}
}

foreach test : ranluxTests
	map = objectMap.get(test)
	testObjs = [ranlux.extract_objects(map['test'] + ['helpers.cxx', 'ranlux64.cxx'])]
	command = [crunchMake, '-s', '@INPUT@', '-o', '@OUTPUT@', '-I' + meson.source_root(), '-L' + libCrunchppPath]

	custom_target(
		test,
		command: command + commandExtra,
		input: [test + '.cxx'] + testObjs,
		output: test + testExt,
		depends: libCrunchpp,
		build_by_default: true
	)

	test(
		map['name'],
		crunchpp,
		args: test,
		workdir: meson.current_build_dir()
	)
endforeach
