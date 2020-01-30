#!/usr/bin/env python
"""Generate "ALTER EXTENSION" statements to package a list of SQL definitions.

The script doesn't try to be a robust parser: it relies on the input file
being regular enough.

The script is also incomplete, but it complains loudly if it meets elements
it doesn't know how to deal with.
"""

# Copyright (c) 2011-2020, Daniele Varrazzo <daniele.varrazzo@gmail.com>
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

from __future__ import print_function

import re
import sys

re_stmt = re.compile(
    r'CREATE\s+(?:OR\s+REPLACE\s+)?(\w+)\b([^;]+);',
    re.MULTILINE | re.IGNORECASE)

def process_file(f, opt):
    data = f.read()
    # Clean up parts we don't care about and that make parsing more complex
    data = strip_comments(data)
    data = strip_strings(data)

    for m in re_stmt.finditer(data):
        try:
            f = globals()['process_' + m.group(1).lower()]
        except:
            # TODO: all the missing statements
            raise KeyError("can't process statement 'CREATE %s'" %
                (m.group(1).upper(),))

        f(m.group(2), opt.extname)

def process_aggregate(body, extname):
    # TODO: parse the "old syntax"
    name = _find_name(body)
    args = _find_args(body)
    print("ALTER EXTENSION %s ADD AGGREGATE %s %s;" % (extname, name, args))

def process_cast(body, extname):
    args = _find_args(body)
    print("ALTER EXTENSION %s ADD CAST %s;" % (extname, args))

def process_function(body, extname):
    name = _find_name(body)
    args = _find_args(body)
    print("ALTER EXTENSION %s ADD FUNCTION %s %s;" % (extname, name, args))

def process_operator(body, extname):
    if body.lstrip().lower().startswith('class'):
        return process_operator_class(body, extname)

    m = re.match(r'^\s*([^\s\(]+)\s*\(', body)
    if m is None:
        raise ValueError("can't find operator:\n%s" % body)

    op = m.group(1)
    m = re.search(r'LEFTARG\s*=\s*([^,\)]+)', body, re.IGNORECASE)
    larg = m and m.group(1).strip()

    m = re.search(r'RIGHTARG\s*=\s*([^,\)]+)', body, re.IGNORECASE)
    rarg = m and m.group(1).strip()

    if not (larg or rarg):
        raise ValueError("can't find operator arguments:\n%s" % body)

    print("ALTER EXTENSION %s ADD OPERATOR %s (%s, %s);" % (
        extname, op, larg or 'NONE', rarg or 'NONE'))

def process_operator_class(body, extname):
    m = re.match(r'^\s*CLASS\s*(\w+)\b.*?USING\s+(\w+)\b',
        body, re.IGNORECASE | re.DOTALL)

    if m is None:
        raise ValueError("can't parse operator class:\n%s" % body)

    print("ALTER EXTENSION %s ADD OPERATOR CLASS %s USING %s;" % (
        extname, m.group(1), m.group(2)))

def process_type(body, extname):
    name = _find_name(body)
    print("ALTER EXTENSION %s ADD TYPE %s;" % (
        extname, name))

re_name = re.compile(r'^\s*(\w+)\b')

def _find_name(body):
    m = re_name.match(body)
    if m is None:
        raise ValueError("can't find name:\n%s" % body)

    return m.group(1)

def _find_args(body):
    # find the closing brace of the arguments list
    # count the braces to avoid getting fooled by type modifiers
    # e.g. varchar(10)
    count = 0
    for i, c in enumerate(body):
        if c == '(':
            count += 1
        elif c == ')':
            count -= 1
            if count == 0:
                break
    else:
        raise ValueError("failed to parse arguments list:\n%s")

    astart = body.index('(')
    aend = i + 1

    return ' '.join(body[astart:aend].split())


re_comment_single = re.compile(r'--.*?$', re.MULTILINE)
re_comment_multi = re.compile(r'/\*.*?\*/', re.DOTALL)

def strip_comments(s):
    """Remove SQL comments from a string.
    
    TODO: doesn't handle nested comments.
    """
    s = re_comment_single.sub("''", s)
    s = re_comment_multi.sub("''", s)
    return s


re_string_quote = re.compile(r"'(''|[^'])*'")
re_string_dollar = re.compile(r'\$([^$]*)\$.*?\$\1\$')

def strip_strings(s):
    """Replace all the SQL literal strings with the empty string."""
    s = re_string_quote.sub('', s)
    s = re_string_dollar.sub('', s)
    return s


def main():
    opt = parse_options()
    print("-- This file was automatically generated")
    print("-- by the script '%s'" % __file__)
    print("-- from input files:", ", ".join(opt.filenames))
    print()
    for fn in opt.filenames:
        f = fn == '-' and sys.stdin or open(fn)
        process_file(f, opt)

def parse_options():
    from optparse import OptionParser
    parser = OptionParser()
    parser.add_option('--extname')
    opt, args = parser.parse_args()
    if not opt.extname:
        parser.error("extension name must be specified")
    opt.filenames = args or ['-']
    return opt

if __name__ == '__main__':
    main()

