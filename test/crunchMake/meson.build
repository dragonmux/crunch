if cxx.get_id() == 'gcc'
	crunchMakeWrapper = find_program('crunchMakeGCC.py')
elif cxx.get_id() == 'clang'
	crunchMakeWrapper = find_program('crunchMakeClang.py')
elif isMSVC
	crunchMakeWrapper = find_program('crunchMakeMSVC.py')
else
	crunchMakeWrapper = disabler()
	warning('Your compiler is not strictly supported by crunch, skipping crunchMake tests')
endif

if crunchMakeWrapper.found()
	custom_target(
		'crunchMake-quiet',
		command: [
			crunchMakeWrapper,
			'-c', crunchMake,
			'-q',
			'-i', '@INPUT@',
			'-o', '@OUTPUT@',
			'--',
			f'-L@libCrunchppPath@',
			libCrunchppDep.get_variable('compile_args'),
			libCrunchppDep.get_variable('link_args'),
		] + commandExtra,
		input: 'dummyTest.cxx',
		output: 'dummyTest-quiet' + testExt,
		depends: libCrunchpp,
		build_by_default: true
	)

	custom_target(
		'crunchMake-noisy',
		command: [
			crunchMakeWrapper,
			'-c', crunchMake,
			'-i', '@INPUT@',
			'-o', '@OUTPUT@',
			'--',
			f'-L@libCrunchppPath@',
			libCrunchppDep.get_variable('compile_args'),
			libCrunchppDep.get_variable('link_args'),
		] + commandExtra,
		input: 'dummyTest.cxx',
		output: 'dummyTest-noisy' + testExt,
		depends: libCrunchpp,
		build_by_default: true
	)
endif
