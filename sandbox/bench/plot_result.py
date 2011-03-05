"""Plot a result from benchmark.py

Copyright (C) 2011 - Daniele Varrazzo
"""
import sys
from collections import defaultdict

import matplotlib.pyplot as plt

import benchmark

tests = defaultdict(list)
nsamples = cls_name = None

# Read from a file or stdin if no file provided
f = len(sys.argv) > 1 and open(sys.argv[1]) or sys.stdin

for line in f:
    tokens = line.split()

    # Parse the class of the test
    if cls_name is None:
        cls_name = tokens[0]
        test = getattr(benchmark, cls_name)

    assert cls_name == tokens[0], (cls_name, tokens)

    # Parse the number of samples
    if nsamples is None: nsamples = tokens[2]
    assert nsamples == tokens[2], (nsamples, tokens)

    tests[tokens[1]].append(
        (int(tokens[3]), float(tokens[4])))

fig = plt.figure()
ax = fig.add_subplot(111)
for label, data in sorted(tests.items()):
    data = zip(*data)  # transpose
    p = ax.plot(data[0], data[1], 'o-', label=label)

ax.set_title("%s (n=%s)" % (test.title, nsamples))
ax.set_xlabel(test.xlabel)
ax.set_ylabel(test.ylabel)
ax.legend(loc=2)

plt.show()

