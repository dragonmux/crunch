crunchMakeSrc = ['crunchMake.cpp']

prefix = get_option('prefix')
libdir = join_paths(prefix, get_option('libdir'))

crunchMake = executable(
	'crunchMake',
	crunchMakeSrc,
	include_directories: [crunchppInc],
	link_with: [libCrunchpp],
	dependencies: [threading],
	cpp_args: [
		'-Dcrunch_GCC="$(GCC)"',
		'-Dcrunch_PREFIX="' + prefix + '"',
		'-Dcrunch_LIBDIR="' + libdir + '"'
	]
)
