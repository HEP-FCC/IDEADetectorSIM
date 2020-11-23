CC = g++
HEADERS = -I inc -I $(WAVEDATA_DIR)/inc -I $(WAVEDATA_DIR)/linkdef_inc
ROOTCONFIG = `root-config --cflags`
ROOTGLIBS = `root-config --glibs`
SOURCES = $(shell find src/. -name "*.cpp")
LIBS = -L $(WAVEDATA_DIR)/lib -lWaveData
OUTPUT_PROGRAM = test.out

all: $(OUTPUT_PROGRAM)

$(OUTPUT_PROGRAM):
	$(CC) $(ROOTCONFIG) $(HEADERS) $(SOURCES) -o $(OUTPUT_PROGRAM) $(ROOTGLIBS) $(LIBS)

clean:
	$(RM) $(OUTPUT_PROGRAM)
