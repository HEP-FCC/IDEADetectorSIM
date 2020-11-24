# $Id: GNUmakefile 68025 2013-03-13 13:43:46Z gcosmo $
# --------------------------------------------------------------
# GNUmakefile for examples module.  Gabriele Cosmo, 06/04/98.
# --------------------------------------------------------------

#name := g4GMC
name := $(notdir ${PWD})
G4TARGET := $(name)
G4EXLIB := true

#LOCALDIR := ${PWD}

ifndef G4INSTALL
  G4INSTALL = ../../../..
endif

SUBDIRS = EXCEP GEOMSRVC SVX PSHW CDCH PHCV DRFPIC
SUBSINCL :=$(foreach dir,$(SUBDIRS), -I${G4WORKDIR}/$(dir)/include)
SUBSFLAGS :=$(foreach dir,$(SUBDIRS), -l$(dir))

include $(G4INSTALL)/config/architecture.gmk

#ifdef G4INSTBASE
#  G4LIB :=${G4LIB}/../lib64
#else
#  G4LIB :=${G4LIB}/${G4SYSTEM}
#endif

ifdef GCC_FQ_DIR
  LGCCFLIB := -L${GCC_FQ_DIR}/lib64
endif

#ifndef PLATFORM
#  PLATFORM = slc4_ia32_gcc34
#endif

#ADD ROOT SETTINGS
#CPPFLAGS += -I$(ROOTSYS)/include/root
ROOTINC := -I$(shell $(ROOTSYS)/bin/root-config --incdir)
CPPFLAGS += ${ROOTINC}
ROOTFLAGS := $(shell $(ROOTSYS)/bin/root-config --libs) -lGui -lHtml -lMinuit
EXTRALIBS := ${ROOTFLAGS}

DICTIONARYPATH := ${G4WORKDIR}/tmp/${G4SYSTEM}/${G4TARGET}/dictionary
SRCFORDICT := GEOMSRVC/src/GMCG4TrackerHit.cc GEOMSRVC/src/GMCG4Particle.cc

#GCCXMLPATH := /usr/bin
#G4INCLUDE := /pro/geant4/geant4.10.p01-install/include/Geant4

#ADD GMC SETTINGS
GMCLIB := GMCAnalyzer
GMCINCLUDE := ${GMCDIR}/include
GMCLPATH := ${GMCDIR}/obj
ROMEINCLUDE := ${ROMESYS}/include
#MIDASLIB := -L${MIDASSYS}/linux/lib -lmidas-shared -L/usr/lib64 -lsqlite3 \
#	-L/usr/lib64/mysql -lmysqlclient

#ADD CONFIG READER SETTINGS
CRDBASEPATH := ${G4WORKDIR}/../ConfigReader
CRDFLAGS := -L${CRDBASEPATH}/lib -lSCrd
CPPFLAGS += -I${CRDBASEPATH}/include
EXTRALIBS += ${CRDFLAGS}

#ADD SUBDIR SETTINGS
CPPFLAGS += -I${G4WORKDIR}/include
CPPFLAGS += ${SUBSINCL}
EXTRALIBS += -L./lib ${SUBSFLAGS}

CPPFLGS := ${CPPFLAGS}
CPPFLAGS += -std=c++11
export CPPFLAGS
#export EXTRALIBS

CPPVERBOSE=1
export CPPVERBOSE

include $(G4INSTALL)/config/binmake.gmk

.PHONY: all
all: confread dictionary lib_sub lib cplib bin

dictionary:
ifndef ROOTSYS
	@echo ROOTSYS not defined!
endif
	mkdir -p ${DICTIONARYPATH}
	mkdir -p ${G4WORKDIR}/lib
	rootcint -f ${DICTIONARYPATH}/Dict.cc -c -p -Iinclude -I${G4INCLUDE} -I${CLHEP_INCLUDE_DIR} -O2 -g -Wall -W -fPIC -pipe -Woverloaded-virtual -m64 GEOMSRVC/include/GMCG4Classes.hh GEOMSRVC/src/LinkDef.hh

	$(CXX) -o ${G4WORKDIR}/lib/libGMCG4ClassesDict.so -fpic -shared \
	${DICTIONARYPATH}/Dict.cc ${SRCFORDICT} \
	-I. ${CPPFLGS} -I${G4INCLUDE} \
	-I${CLHEP_INCLUDE_DIR} \
	${ROOTFLAGS} -L${CLHEP_LIB_DIR} -L${G4LIB}/${G4SYSTEM} \
	${GCCFLIB} ${LDLIBS2} ${LOADLIBS} \
	-L${GMCLPATH} ${CRDFLAGS}

confread:
	$(MAKE) -C ${G4WORKDIR}/../ConfigReader

lib_sub:
	@for dir in $(SUBDIRS); do (cd $$dir && $(MAKE) lib); done;:

bin_sub:
	@for dir in $(SUBDIRS); do (cd $$dir && $(MAKE) bin); done;:

obj_sub:
	@for dir in $(SUBDIRS); do (cd $$dir && $(MAKE) obj); done;:

cplib:
	@cp tmp/${G4SYSTEM}/*/lib*.so lib/

readHits:
	mkdir -p ${G4WORKDIR}/bin/${G4SYSTEM}
	gcc -std=c++11 -o ${G4WORKDIR}/bin/${G4SYSTEM}/readHits readHits.cc \
	-Iinclude ${ROOTINC} -I${G4INCLUDE} -I${ROMEINCLUDE} \
	 -I${GMCINCLUDE} -I${CLHEP_INCLUDE_DIR} \
	-L${CLHEP_LIB_DIR} ${GCCFLIB} -L${G4LIB}/${G4SYSTEM} ${LDLIBS2} \
	${LDLIBS3} ${LOADLIBS} -L${GMCLPATH} -l${GMCLIB} ${MIDASLIB} \
	${EXTRALIBS}

clean_all: clean clean_sub clean_cfgr
	@$(RM) ${G4WORKDIR}/lib/lib*.so ${G4WORKDIR}/bin/${G4SYSTEM}/readHits hits.root

clean_cfgr:
	$(MAKE) -C ${CRDBASEPATH} clean

clean_sub:
	@for dir in $(SUBDIRS); do (cd $$dir && $(MAKE) clean); done;:

#clean_bin_sub:
#	@for dir in $(SUBDIRS); do (cd $$dir && $(MAKE) clean_bin); done;:


