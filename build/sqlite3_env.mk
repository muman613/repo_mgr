################################################################################
#	MODULE		:	Makefile
#	PROJECT		:	Reaper
#	AUTHOR 		:	Michael Uman
#	DATE		:	April 17, 2013
################################################################################

SQLITE3_CFLAGS=$(shell pkg-config sqlite3 --cflags)
SQLITE3_LIBS=$(shell pkg-config sqlite3 --libs)
