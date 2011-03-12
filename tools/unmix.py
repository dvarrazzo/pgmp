#!/usr/bin/env python
"""Convert a file containing a mix of python code and content into content.

Write the input file to stdout. Python code included between blocks
``!! PYON`` and ``!! PYOFF`` is executed and the output emitted.
"""

# Copyright (c) 2011, Daniele Varrazzo <daniele.varrazzo@gmail.com>
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
# * Redistributions of source code must retain the above copyright notice,
#   this list of conditions and the following disclaimer.
# * Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
# * The name of Daniele Varrazzo may not be used to endorse or promote
#   products derived from this software without specific prior written
#   permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

import sys

def convert(f):
    mode = 'out'    # can be 'out' or 'py'
    script = []
    env = {}
    while 1:
        line =  f.readline()
        if not line: break

        if line.startswith("!#"):
            # a comment
            continue

        if not line.startswith("!!"):
            # a regular line
            if mode == 'out':
                sys.stdout.write(line)
            elif mode == 'py':
                script.append(line)
            else:
                raise ValueError("unexpected mode: %s" % mode)
            continue

        # state change
        if 'PYON' in line and mode == 'out':
            del script[:]
            mode = 'py'
        elif 'PYOFF' in line and mode == 'py':
            exec ''.join(script) in env
            mode = 'out'
        else:
            raise ValueError("bad line in mode %s: %s"
                % (mode, line.rstrip()))


if __name__ == '__main__':
    if len(sys.argv) > 2:
        print >>sys.stderr, "usage: %s [FILE]" % sys.argv[1]
        sys.exit(2)

    f = len(sys.argv) == 2 and open(sys.argv[1]) or sys.stdin
    convert(f)
    sys.exit(0)

