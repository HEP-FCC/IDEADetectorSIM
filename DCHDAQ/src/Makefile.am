datadir=/etc/wavedump/
bin_PROGRAMS=dchdaq
dchdaq_SOURCES=DCHDAQ.c WDconfig.c WDplot.c X742CorrectionRoutines.c fft.c keyb.c v1190.c
dchdaq_LDFLAGS = -L$CAENCOMM_LIBDIR -L$CAENVME_LIBDIR -L$CAENDGZ_LIBDIR
dchdaq_LDADD = -lCAENComm -lCAENDigitizer -lCAENVME -lm -lpthread
INCLUDES = -I../include
AM_CFLAGS=  -fPIC
dist_data_DATA=../Setup/WaveDumpConfig.txt ../Setup/WaveDumpConfig_X742.txt ../Setup/WaveDumpConfig_X740.txt