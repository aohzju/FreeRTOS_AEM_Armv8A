#!/usr/bin/python

import subprocess

src_list = ['./']

subprocess.check_call(['rm', '-f', 'tags'])

for src in src_list:
	cmd = "ctags -R --append --c-kinds=+p %s*" % src
	subprocess.check_call(cmd, shell=True)

