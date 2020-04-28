crunchMakeSrc = ['crunchMake.cpp']
if target_machine.system() != 'windows'
	crunchMakeSrc += ['compilerUnixlike.cxx']
else
	crunchMakeSrc += ['compilerWindows.cxx']
endif

crunchConfig = configuration_data()
whichCC = cc.cmd_array()
whichCXX = cxx.cmd_array()

if target_machine.cpu_family() == 'x86_64'
	whichCC += '-fPIC -DPIC'
	whichCXX += '-fPIC -DPIC'
endif

whichCC = ' '.join(whichCC)
whichCXX = ' '.join(whichCXX)

crunchConfig.set('CC', whichCC)
crunchConfig.set('CXX', whichCXX)
crunchConfig.set('COMPILER', cxx.get_id())
crunchConfig.set10('IS_CLANG', cxx.get_id() == 'clang')

configure_file(
	input: 'crunchMake.h.in',
	output: 'crunchMake.h',
	configuration: crunchConfig
)

versionHeaderSrc = configure_file(
	configuration: config,
	input: 'version.hxx.in',
	output: 'version.hxx.in'
)

versionHeader = vcs_tag(
	input: versionHeaderSrc,
	output: 'version.hxx',
	replace_string: '@VERSION@',
	fallback: 'v@0@'.format(meson.project_version())
)

crunchMake = executable(
	'crunchMake',
	crunchMakeSrc + [versionHeader],
	include_directories: [crunchppInc, substrate],
	link_with: [libCrunchpp],
	dependencies: [threading],
	cpp_args: [
		'-Dcrunch_PREFIX="' + prefix + '"',
		'-Dcrunch_LIBDIR="' + libdir + '"'
	],
	override_options: [
		'cpp_std=c++14'
	],
	install: true,
	install_rpath: libdir
)
