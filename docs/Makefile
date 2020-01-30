# pgmp -- documentation makefile
#
# Use 'make env' once, then 'make html' to build the HTML documentation.
# You can use PYTHON=python3.6 to use a different Python version to build.
#
# Copyright (C) 2011-2020 Daniele Varrazzo

ENV_DIR = $(shell pwd)/env
ENV_BIN = $(ENV_DIR)/bin
ENV_LIB = $(ENV_DIR)/lib

SPHINXOPTS    =
SPHINXBUILD   = $(ENV_BIN)/sphinx-build
PAPER         =
BUILDDIR      = .

PYTHON ?= python
PLOTS = $(patsubst ../bench/%.txt,html/img/%.png,$(sort $(wildcard ../bench/*.txt)))

.PHONY: env html upload clean

default: html

html: $(PLOTS) html/.gitignore html/.nojekill
	PYTHONPATH=$(ENV_LIB) $(SPHINXBUILD) -b html $(ALLSPHINXOPTS) \
		. $(BUILDDIR)/html

html/img/%.png: ../bench/%.txt
	mkdir -p html/img
	$(ENV_BIN)/python ../bench/plot_result.py $< -o $@

# The environment is currently required to build the documentation.
# It is not clean by 'make clean'

env:
	virtualenv -p $(PYTHON) $(ENV_DIR)
	$(ENV_BIN)/pip install -r requirements.txt
	# Clone the github pages repository to update
	# Fail silently if this is not a git repos (upload won't work of course)
	git status 2>/dev/null \
		&& git clone -b gh-pages \
			$$(git remote show -n origin | awk '/URL/ {print $$NF; exit}') \
			html \
		|| true

html/.gitignore: html-gitignore
	cp $< $@

html/.nojekill:
	touch $@

upload:
	cd html && test -d .git \
		&& git add --all \
		&& git commit -m "website update on $$(date)" \
		&& git push

clean:
	# Do not delete .gitignore and .git directories
	$(RM) -r html/*
