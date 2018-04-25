###############################################################################
#	FILE		:	Makefile
#	PROJECT		:	crepo
#	DATE		:	May 5, 2016
#	LAST MOD	:	January 10, 2017
#	PROGRAMMER	:	Michael Uman
###############################################################################

TARGET_EXE=crepo
TARGET_TYPE=exe
CPP_SOURCES=main.cpp functions.cpp repoObject.cpp utils.cpp actions.cpp
ARCH?=$(shell arch)

include ../build/libxml2_env.mk
include ../build/libpopt_env.mk

CFLAGS=-std=c++11 $(LIBXML_CFLAGS) $(LIBPOPT_CFLAGS) $(ARCH_CFLAGS)
LDFLAGS=$(LIBXML_LIBS) $(LIBPOPT_LIBS) $(ARCH_LDFLAGS)


include ../build/buildsys.mk


-include $(DEPS)