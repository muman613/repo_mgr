###############################################################################
#	FILE		:	Makefile
#	PROJECT		:	reaper
#	DATE		:	May 30, 2013
#	LAST MOD	:	January 10, 2017
#	PROGRAMMER	:	Michael Uman
###############################################################################

TARGET_EXE=reaper
TARGET_TYPE=exe
ARCH?=$(shell arch)

CPP_SOURCES=reaper.cpp utils.cpp functions.cpp reaperEntry.cpp
C_SOURCES=sqlite3.c

CFLAGS+=$(SQLITE3_CFLAGS) $(ARCH_CFLAGS) $(DEBUG_CFLAGS)
LDFLAGS+=$(SQLITE3_LIBS) $(ARCH_LDFLAGS) -ldl -lpthread

include ../build/buildsys.mk

-include $(DEPS)

