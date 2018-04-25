################################################################################
#	MODULE	:	tools_env.mk
#	PROJECT	:	Build Scripts
#	AUTHOR	:	Michael Uman
#	DATE	:	April 22, 2013
################################################################################

ARCH ?= $(shell uname -m)
DIST ?= $(shell lsb_release -cs)

CXX ?= g++			# c++ compiler
CC  ?= gcc			# c compiler
AR  ?= ar			# archive tool
TAR ?= tar			# tar tool

