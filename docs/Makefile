# pgmp -- documentation makefile
#
# Use 'make env', then 'make html' to build the HTML documentation
#
# Copyright (C) 2011-2015 Daniele Varrazzo

ENV_DIR = $(shell pwd)/env
ENV_BIN = $(ENV_DIR)/bin
ENV_LIB = $(ENV_DIR)/lib

SPHINXOPTS    =
SPHINXBUILD   = $(ENV_BIN)/sphinx-build
PAPER         =
BUILDDIR      = .

PLOTS = $(patsubst ../sandbox/bench/%.txt,html/img/%.png,$(wildcard ../sandbox/bench/*.txt))

.PHONY: env clean html

default: html

html: $(PLOTS)
	PYTHONPATH=$(ENV_LIB) $(SPHINXBUILD) -b html $(ALLSPHINXOPTS) \
		. $(BUILDDIR)/html

html/img/%.png: ../sandbox/bench/%.txt
	mkdir -p html/img
	$(ENV_BIN)/python ../sandbox/bench/plot_result.py $< -o $@

# The environment is currently required to build the documentation.
# It is not clean by 'make clean'

env:
	virtualenv $(ENV_DIR)
	$(ENV_BIN)/pip install -r requirements.txt

clean:
	# Do not delete .gitignore and .git directories
	$(RM) -r html/*

