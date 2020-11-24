#!/bin/bash

#  $Id: envMidasDAQ.sh 268 2013-10-31 17:21:49Z tassielli $
#  $Author:  $
#  $Revision:  $

#source ../envDeps.sh

export MIDAS_DIR=/home/meg/FIRB/Repository/online/online_midas
export MIDAS_EXPTAB=$MIDAS_DIR/exptab
export MIDAS_EXPT_NAME=FIRB_DAQ

export MYDRIVER_DIR=$MIDAS_DIR/mydrivers

export CAENVME_DIR=$CAENVME
export CAENVME_INCDIR=$CAENVME_DIR/include
export CAENVME_LIBDIR=$CAENVME_DIR/lib/x64
export ETHERNET_LIBDIR=$MYDRIVER_DIR/ethernet

export HAVE_CAEN_BRD=1
