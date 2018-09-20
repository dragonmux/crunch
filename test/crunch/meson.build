libCrunchTests = ['testArgsParser', 'testCrunch', 'testBad']

foreach test : libCrunchTests
	custom_target(
		test,
		command: [crunchMake, '@INPUT@', '-o', '@OUTPUT@', '-I' + crunchSrcDir],
		input: test + '.c',
		output: test + '.so',
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
