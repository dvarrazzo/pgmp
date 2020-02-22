#!/usr/bin/env python
"""Plot a result from benchmark.py

Copyright (C) 2011-2020 - Daniele Varrazzo
"""
import sys
from collections import defaultdict

import matplotlib

matplotlib.use('AGG')  # noqa

import matplotlib.pyplot as plt

tests = defaultdict(list)
nsamples = cls_name = None

# Read from a file or stdin if no file provided
f = len(sys.argv) > 1 and open(sys.argv[1]) or sys.stdin

labels = {}

for line in f:
    if line.isspace():
        continue

    if ':' in line:
        tokens = line.split(':', 1)
        labels[tokens[0].strip()] = tokens[1].strip()
        continue

    tokens = line.split()

    # Parse the class of the test
    if cls_name is None:
        cls_name = tokens[0]
    assert cls_name == tokens[0], (cls_name, tokens)

    # Parse the number of samples
    if nsamples is None:
        nsamples = tokens[2]
    assert nsamples == tokens[2], (nsamples, tokens)

    tests[tokens[1]].append((int(tokens[3]), float(tokens[4])))

fig = plt.figure()
ax = fig.add_subplot(111)
for label, data in sorted(tests.items()):
    data = list(zip(*data))  # transpose
    p = ax.plot(data[0], data[1], 'o-', label=label)

title = labels.get('title', '')
if int(nsamples):
    title += " (n=%s)" % nsamples
ax.set_title(title)

if 'xlabel' in labels:
    ax.set_xlabel(labels['xlabel'])
if 'ylabel' in labels:
    ax.set_ylabel(labels['ylabel'])
ax.legend(loc=2)

if '-o' in sys.argv:
    plt.savefig(sys.argv[sys.argv.index('-o') + 1], dpi=72)
else:
    plt.show()
