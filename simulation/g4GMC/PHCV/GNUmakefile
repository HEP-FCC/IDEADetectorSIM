# $Id: GNUmakefile 68058 2013-03-13 14:47:43Z gcosmo $
# --------------------------------------------------------------
# GNUmakefile for examples module.  Gabriele Cosmo, 06/04/98.
# --------------------------------------------------------------

name := $(notdir ${PWD})
G4TARGET := $(name)
G4EXLIB := true

#ifndef G4INSTALL
#  G4INSTALL = ../../../..
#endif

#CPPFLAGS += -I../include

#ADD CONFIG READER SETTINGS
#CRDBASEPATH := ${G4WORKDIR}/../ConfigReader
#CRDFLAGS := -L${CRDBASEPATH}/lib -lSCrd
#CPPFLAGS += -I${CRDBASEPATH}/include
#EXTRALIBS += ${CRDFLAGS}

#CPPFLAGS += -std=c++11

#ORGCPPFLAGS := ${CPPFLAGS}
#CPPFLAGS := -I${G4WORKDIR}/${G4TARGET}/include
#CPPFLAGS += ${ORGCPPFLAGS}

#CPPVERBOSE := 1

.PHONY: all
all: obj
#lib bin

include $(G4INSTALL)/config/architecture.gmk

include $(G4INSTALL)/config/binmake.gmk
