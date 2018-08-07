crunchMakeSrc = ['crunchMake.cpp']

prefix = get_option('prefix')
libdir = join_paths(prefix, get_option('libdir'))

crunchConfig = configuration_data()
whichCC = run_command('python', '-c', 'import os; print(os.environ.get("CC", ""))')
whichCXX = run_command('python', '-c', 'import os; print(os.environ.get("CXX", ""))')

if whichCC.returncode() != 0 or whichCXX.returncode() != 0
	error('Failed to detect compilation environment')
else
	whichCC = whichCC.stdout().strip()
	whichCXX = whichCXX.stdout().strip()
endif

if whichCC != '' and whichCXX != ''
	if target_machine.cpu_family() == 'x86_64'
		whichCC += ' -fPIC -DPIC'
		whichCXX += ' -fPIC -DPIC'
	endif

	crunchConfig.set('CC', whichCC)
	crunchConfig.set('CXX', whichCXX)

	configure_file(
		input: 'crunchMake.h.in',
		output: 'crunchMake.h',
		configuration: crunchConfig
	)
endif

crunchMake = executable(
	'crunchMake',
	crunchMakeSrc,
	include_directories: [crunchppInc],
	link_with: [libCrunchpp],
	dependencies: [threading],
	cpp_args: [
		'-Dcrunch_PREFIX="' + prefix + '"',
		'-Dcrunch_LIBDIR="' + libdir + '"'
	]
)
