CC = g++
OBJFLAGS = -fpic -c
HEADERS = -I inc
ROOTCONFIG = `root-config --cflags`
ROOTGLIBS = `root-config --glibs`
SOURCES = $(shell find src/. -name "*.cpp")
LIST_OF_HEADERS = $(shell find inc/. -name "*.h") 
LINKDEF_HEADER = linkdef_inc/LinkDef.h
OBJFILES = WaveData.o RunHeader.o WaveDataDict.o
OUTPUT_LIBRARY = lib/libWaveData.so
DICT_SOURCE = WaveDataDict.cpp

all: $(OUTPUT_LIBRARY)

$(OUTPUT_LIBRARY): $(DICT_SOURCE)
	@echo "Creating *.o files..."
	$(CC) $(ROOTCONFIG) $(HEADERS) $(OBJFLAGS) $(SOURCES) $(DICT_SOURCE) $(ROOTGLIBS)
	@echo "Creating *.o files: done."
	@echo "Creating *.so files..."
	$(CC) -fPIC $(ROOTCONFIG) $(HEADERS) $(OBJFILES) -shared -o $(OUTPUT_LIBRARY) $(ROOTGLIBS)
	@echo "Creating *.so files: done."
	@echo "Removing *.o files..."
	$(RM) $(OBJFILES)
	@echo "Removing *.o files: done."

$(DICT_SOURCE):
	@echo "Generating dictionary..."
	@rootcint $(DICT_SOURCE) -c -D__cplusplus $(LIST_OF_HEADERS) $(LINKDEF_HEADER)
	@echo "Generating dictionary: done."

clean:
	@echo "Cleaning..."
	$(RM) $(OUTPUT_LIBRARY) $(DICT_SOURCE) *.h *.pcm
	@echo "Cleaning: done."
