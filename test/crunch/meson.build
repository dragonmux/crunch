libCrunchTests = ['testArgsParser', 'testCrunch', 'testBad']

libCrunchPath = join_paths([meson.build_root(), libCrunch.outdir()])

foreach test : libCrunchTests
	custom_target(
		'crunch-' + test,
		command: [crunchMake, '-s', '@INPUT@', '-o', '@OUTPUT@', '-I' + crunchSrcDir, '-L' + libCrunchPath],
		input: test + '.c',
		output: test + '.so',
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
	'empty',
	crunch,
	workdir: meson.current_build_dir(),
	should_fail: true
)
