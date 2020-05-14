pandoc = find_program('pandoc', required: false)

if pandoc.found()
	manSrcs = {
		'crunchMake.1.md': 'crunchMake.1',
		'crunch++.1.md': 'crunch++.1'
	}
	manuals = []
	foreach manSrc, manOut : manSrcs
		manuals += [configure_file(
			command: [pandoc, '-s', '-t', 'man', '-o', '@OUTPUT@', '@INPUT@'],
			input: manSrc,
			output: manOut,
			install: false
		)]
	endforeach
else
	warning('A suitable version of `pandoc` was not found; ' +
		'changes to man/*.md will not affect your build.')

	manuals = files(
		'crunchMake.1', 'crunch++.1'
	)
endif

install_man(manuals)
