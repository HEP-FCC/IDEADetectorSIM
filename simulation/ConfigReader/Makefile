
CC = gcc # C compiler
CXX = g++ # C compiler

CXXFLAGS = -fPIC -Iinclude -I${CLHEP_INCLUDE_DIR}
CFLAGS = -shared -fPIC -Wall -Wextra -O2 -g # C flags
LDFLAGS = -L${CLHEP_LIB_DIR} -lCLHEP-Vector-${CLHEP_VERSION} # linking flags


RM = rm -f  # rm command
TARGET_LIB = lib/libSCrd.so # target lib

SRCS = $(shell echo src/*.cc) # source files
#HEADERS = $(shell echo include/*.hh)
OBJS = $(SRCS:.cc=.o)

.PHONY: all
all: ${TARGET_LIB}

$(TARGET_LIB): $(OBJS)
	mkdir -p lib
	$(CC) $(CFLAGS) ${LDFLAGS} -o $(TARGET_LIB) $(OBJS)

%.o: %.cc
	$(CXX) $(CXXFLAGS) -o $@ -c $<

example: example.o
	$(CC) -o example example.o ${LDFLAGS} -L./ -lSCrd -L/usrlib64 -lstdc++

.PHONY: clean
clean:
	-${RM} ${TARGET_LIB} ${OBJS} example.o example