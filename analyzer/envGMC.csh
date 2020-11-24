#!/bin/sh

#  $Id: envGMC.sh 268 2013-10-31 17:21:49Z tassielli $
#  $Author:  $
#  $Revision:  $

#gcc xml
#setenv GCCXMLPATH ${GCCXML_FQ_DIR}/bin

#rome
setenv ROMESYS /pro/rome
setenv PATH $ROMESYS/bin:${PATH}
setenv LIBROME yes

#root
setenv ROOTSYS /pro/root_v5_34_30
setenv PATH $ROOTSYS/bin:${PATH}
setenv LD_LIBRARY_PATH ${ROOTSYS}/lib:${LD_LIBRARY_PATH}

#midas
setenv MIDASSYS /pro/FIRB/midas
setenv MIDAS_DIR /pro/FIRB/midas
setenv MIDAS_EXPTAB $MIDAS_DIR/exptab
setenv MIDAS_EXPT_NAME FIRB_DAQ
setenv PATH ${MIDASSYS}/linux/bin:$PATH
setenv LD_LIBRARY_PATH ${MIDASSYS}/linux/lib:${LD_LIBRARY_PATH}

