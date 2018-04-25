###############################################################################
#	FILE		:	Makefile
#	PROJECT		:	reaper
#	DATE		:	May 30, 2013
#	LAST MOD	:	March 19, 2014
#	PROGRAMMER	:	Michael Uman
###############################################################################

TARGET_EXE=reaper
TARGET_TYPE=exe
CPP_SOURCES=CheckoutThread.cpp functions.cpp ReaperDialog.cpp reaperEntry.cpp reaper_guiApp.cpp reaperGUI.cpp utils.cpp
C_SOURCES=sqlite3.c

WX_CFLAGS=$(shell wx-config --cflags)
WX_LDFLAGS=$(shell wx-config --libs)

#include ../build/sqlite3_env.mk

#ARCH_CFLAGS=-m32
#ARCH_LDFLAGS=-m32
#DEBUG_CFLAGS=-g3 -D_DEBUG

CFLAGS+=--std=c++11 $(SQLITE3_CFLAGS) $(WX_CFLAGS) $(DEBUG_CFLAGS)
LDFLAGS+=$(SQLITE3_LIBS) $(WX_LDFLAGS) -ldl -lpthread

include ../../../build/Build/buildsys.mk

-include $(DEPS)


