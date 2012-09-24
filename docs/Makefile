# pgmp -- documentation makefile
#
# Use 'make env', then 'make html' to build the HTML documentation
#
# Copyright (C) 2011 Daniele Varrazzo

PYTHON := python$(PYTHON_VERSION)
PYTHON_VERSION ?= $(shell $(PYTHON) -c 'import sys; print ("%d.%d" % sys.version_info[:2])')
ROOT_DIR = $(shell pwd)
BUILD_DIR = $(ROOT_DIR)/build/lib.$(PYTHON_VERSION)
ENV_DIR = $(ROOT_DIR)/env/py-$(PYTHON_VERSION)
ENV_BIN = $(ENV_DIR)/bin
ENV_LIB = $(ENV_DIR)/lib
EASY_INSTALL = PYTHONPATH=$(ENV_LIB) $(ENV_BIN)/easy_install-$(PYTHON_VERSION) -d $(ENV_LIB) -s $(ENV_BIN)
EZ_SETUP = $(ENV_BIN)/ez_setup.py

SPHINXOPTS    =
SPHINXBUILD   = $(ENV_BIN)/sphinx-build
PAPER         =
BUILDDIR      = .

PLOTS = $(patsubst %.txt,%.png,$(wildcard ../sandbox/bench/*.txt))

.PHONY: env clean html upload

default: html

html: $(PLOTS)
	(cd $(ENV_DIR) && \
	for p in `ls $(ROOT_DIR)/patches/*.patch`; do \
		patch --verbose -p1 < $$p; \
	done)
	PYTHONPATH=$(ENV_LIB) $(SPHINXBUILD) -b html $(ALLSPHINXOPTS) \
		. $(BUILDDIR)/html
	(cd $(ENV_DIR) && \
	for p in `ls $(ROOT_DIR)/patches/*.patch`; do \
		patch --verbose -p1 -R < $$p; \
	done)

upload:
	tar cjf - -C html . \
	| ssh dvarrazzo@pgfoundry.org \
		tar xjvf - -C /home/pgfoundry.org/groups/pgmp/htdocs

../sandbox/bench/%.png: ../sandbox/bench/%.txt
	../sandbox/bench/plot_result.py $< -o $@

# The environment is currently required to build the documentation.
# It is not clean by 'make clean'

env: easy_install
	mkdir -p $(ENV_BIN)
	mkdir -p $(ENV_LIB)
	$(EASY_INSTALL) -U "Pygments>=1.5"
	$(EASY_INSTALL) -U "sphinx>=1.1"
	(cd $(ENV_DIR) && ln -s lib/Sphinx-*.egg/sphinx/ sphinx)

easy_install: ez_setup
	PYTHONPATH=$(ENV_LIB) $(PYTHON) $(EZ_SETUP) -d $(ENV_LIB) -s $(ENV_BIN) setuptools

ez_setup:
	mkdir -p $(ENV_BIN)
	mkdir -p $(ENV_LIB)
	wget -O $(EZ_SETUP) http://peak.telecommunity.com/dist/ez_setup.py

clean:
	$(RM) -r html

