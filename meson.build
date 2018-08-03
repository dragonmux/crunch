project(
	'crunch',
	['c', 'cpp'],
	default_options: [
		'c_std=c99',
		'cpp_std=c++14',
		'warning_level=3'
	],
	version: '0.1.1'
)

cc = meson.get_compiler('c')
cxx = meson.get_compiler('cpp')
threading = dependency('threads')

coverage = get_option('b_coverage')
buildType = get_option('buildtype')

if ['gcc', 'clang'].contains(cc.get_id())
	add_global_arguments('-Wall', '-Wextra', '-pedantic', language: 'c')
	if coverage and buildType == 'debug'
		add_global_arguments('--coverage', language: 'c')
	endif
endif

if ['gcc', 'clang'].contains(cxx.get_id())
	add_global_arguments('-Wall', '-Wextra', '-pedantic', language: 'cpp')
	if coverage and buildType == 'debug'
		add_global_arguments('--coverage', language: 'cpp')
	endif
endif

subdir('crunch')
subdir('crunch++')
subdir('crunchMake')

#foreach test : libCrunchTests
#	run_command(crunchMake, test)
#endforeach

#test('crunch', crunch, args = libCrunchTests)
