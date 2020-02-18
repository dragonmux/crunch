libCrunchTests = ['testArgsParser', 'testCrunch', 'testBad']

libCrunchPath = join_paths([meson.build_root(), libCrunch.outdir()])

foreach test : libCrunchTests
	command = [crunchMake, '-s', '@INPUT@', '-o', '@OUTPUT@', '-I' + crunchSrcDir, '-L' + libCrunchPath]
	if debug
		command += ['--debug']
	else
		command += ['-DNO_ALLOC_TEST']
	endif

	inputs = [files(test + '.c')]
	if test == 'testCrunch'
		command += ['-I' + ranluxSrcDir]
		inputs += [ranlux]
	endif

	custom_target(
		'crunch-' + test,
		command: command,
		input: inputs,
		output: test + testExt,
		depends: libCrunch,
		build_by_default: true
	)
endforeach

test(
	'crunch',
	crunch,
	args: libCrunchTests + ['testMustNotExist'],
	workdir: meson.current_build_dir()
)

test(
	'crunch-empty',
	crunch,
	workdir: meson.current_build_dir(),
	should_fail: true
)
