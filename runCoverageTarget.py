#!/usr/bin/env python3
# SPDX-License-Identifier: LGPL-3.0-or-later
from subprocess import run, PIPE, DEVNULL
from sys import argv, exit

args = argv[1:]
for i in enumerate(args):
	i, arg = i
	if arg == '--':
		break
	args[i] = arg.replace('/', '\\')

result = run(args, stdout = PIPE, stderr = DEVNULL)
print(result.stdout.decode('utf-8'))
exit(result.returncode)
