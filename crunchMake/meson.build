crunchMakeSrc = ['crunchMake.cpp']

prefix = get_option('prefix')
libdir = join_paths(prefix, get_option('libdir'))

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

configure_file(
	input: 'crunchMake.h.in',
	output: 'crunchMake.h',
	configuration: crunchConfig
)

crunchMake = executable(
	'crunchMake',
	crunchMakeSrc,
	include_directories: [crunchppInc],
	link_with: [libCrunchpp],
	dependencies: [threading],
	cpp_args: [
		'-Dcrunch_PREFIX="' + prefix + '"',
		'-Dcrunch_LIBDIR="' + libdir + '"'
	],
	install: true
)
