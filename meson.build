project(
	'crunch',
	['c', 'cpp'],
	default_options: [
		'c_std=c99',
		'cpp_std=c++11',
		'warning_level=3',
		'b_pch=false'
	],
	version: '0.2.0'
)

cc = meson.get_compiler('c')
cxx = meson.get_compiler('cpp')
threading = dependency('threads')

coverage = get_option('b_coverage')
buildType = get_option('buildtype')

if ['gcc', 'clang'].contains(cc.get_id())
	add_project_arguments('-Wall', '-Wextra', '-pedantic', language: 'c')
	if coverage and buildType == 'debug'
		add_project_arguments('--coverage', language: 'c')
	endif
endif

if ['gcc', 'clang'].contains(cxx.get_id())
	add_project_arguments('-Wall', '-Wextra', '-pedantic', language: 'cpp')
	if coverage and buildType == 'debug'
		add_project_arguments('--coverage', language: 'cpp')
	endif
endif

if target_machine.system() == 'windows'
	add_project_arguments('-D__crunch_lib__', language: ['c', 'cpp'])
endif

subdir('crunch')
subdir('crunch++')
subdir('crunchMake')
subdir('test')
