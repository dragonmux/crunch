ranlux = static_library(
	'ranlux',
	['ranlux32.cxx'],
	pic: true,
	include_directories: projectInclude,
	install: false,
	build_by_default: true
)

ranluxTests = ['testRanlux32']
libCrunchppPath = join_paths([meson.build_root(), libCrunchpp.outdir()])

objectMap = {
	'testRanlux32': {'test': ['ranlux32.cxx'], 'name': 'ranlux32'}
}

foreach test : ranluxTests
	map = objectMap.get(test)
	testObjs = [ranlux.extract_objects(map['test'])]
	command = [crunchMake, '-s', '@INPUT@', '-o', '@OUTPUT@', '-I' + meson.source_root(), '-L' + libCrunchppPath]
	if cxx.get_id() != 'msvc'
		command += ['-z', 'defs']
	endif
	if debug
		command += '--debug'
	endif

	custom_target(
		test,
		command: command,
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
