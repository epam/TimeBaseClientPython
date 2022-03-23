# Name of the app executable to build

OS?=LINUX

DXAPI_BIN=./dxapi/bin
PYTHONAPI_INTERFACE=dxapi.py
PYHTONAPI_LIB=_dxapi.so

PYTHON_VERSION?=36
ifeq ($(PYTHON_VERSION),37)
	PYTHON=python3.7
	PYTHON_VERSION_FULL=3.7
	PYTHON_LIB_SUFFIX=3.7m
else ifeq ($(PYTHON_VERSION),38)
	PYTHON=python3.8
	PYTHON_VERSION_FULL=3.8
	PYTHON_LIB_SUFFIX=3.8
else ifeq ($(PYTHON_VERSION),39)
	PYTHON=python3.9
	PYTHON_VERSION_FULL=3.9
	PYTHON_LIB_SUFFIX=3.9
else ifeq ($(PYTHON_VERSION),310)
	PYTHON=python3.10
	PYTHON_VERSION_FULL=3.10
	PYTHON_LIB_SUFFIX=3.10
else
	PYTHON=python3.6
	PYTHON_VERSION_FULL=3.6
	PYTHON_LIB_SUFFIX=3.6m
endif

ifeq ($(OS),MACOS)
    DFP_BIN=./dfp/lib/osx/64
    DFP_LIB=DecimalNative
    RPATH_PARAM=
    THIRD_PARTY_LIBS=
    PYTHON_INCLUDES=/Library/Frameworks/Python.framework/Versions/$(PYTHON_VERSION_FULL)/Headers
    PYTHON_LIBS=-L/Library/Frameworks/Python.framework/Versions/$(PYTHON_VERSION_FULL)/lib -lpython$(PYTHON_LIB_SUFFIX)
    BIN_SUBFOLDER=darwin
else
    DFP_BIN=./dfp/lib/linux/64
    DFP_LIB=DecimalNative
    RPATH_PARAM=-Wl,-rpath,'$$ORIGIN'
    PYTHON_INCLUDES=/usr/include/$(PYTHON)
    PYTHON_LIBS=
    BIN_SUBFOLDER=linux
endif

BUILD_TARGETS=$(BINDIR)/$(PYHTONAPI_LIB)
CLEAN_TARGETS=clean-$(PYHTONAPI_LIB) clean-$(PYTHONAPI_INTERFACE)

# wrapper
WRAPPER_OBJ=dxapi_wrap

# Names of C/C++ source files to build, without path/extension
OBJ_LIB=common python_common tick_cursor tick_loader message_codec $(WRAPPER_OBJ)

# C/C++ source code files and internal includes are here
SRCDIR=src

# Source files inside $(SRCDIR)
SRCDIRS=codecs swig swig/wrappers

# Include directories
INCLUDES= $(PYTHON_INCLUDES) ./dxapi/include/native ./dxapi/include/native/dxapi $(SRCDIR)

OBJ=$(OBJ_LIB)

# Library files
# LIBS=lib1 lib2
LIBS=

# Common preprocessor flags
COMMONPFLAGS=

PLATFORM=x64

#build Release by default
DEBUG?=0

ifeq ($(DEBUG),1)
	# C preprocessor flags to define
	PFLAGS=$(COMMONPFLAGS) _DEBUG DEBUG
	# C/C++ compiler flags
	CFLAGS=-O0 -g
    CONFIGURATION=debug
    DXAPI_LIB=dxapi-$(PLATFORM)d
else
	# Note that NDEBUG will disable assertions
	PFLAGS=$(COMMONPFLAGS) NDEBUG
	CFLAGS=-O3
    CONFIGURATION=release
    DXAPI_LIB=dxapi-$(PLATFORM)
endif

PLATFORM=x64

# Example: rebuild with clang in Debug mode
# CC=clang;make clean;make DEBUG=1

#==============================================================================

# Binary files are put here
BINDIR=bin/$(CONFIGURATION)/$(BIN_SUBFOLDER)/$(PLATFORM)/py$(PYTHON_VERSION)
INIT_PY_DIR=bin/$(CONFIGURATION)

# Temporary object files are put here
OBJDIR=obj/$(PLATFORM)/$(CONFIGURATION)

WRAPDIR=$(SRCDIR)/swig/wrappers
WRAPPER_OBJDIR=$(OBJDIR)


#==============================================================================

# Default C/C++ compiler
CC?=clang

#==============================================================================

# Choose C++ compiler from C compiler
ifeq ($(CC),gcc)
	CXX=g++
endif
ifeq ($(CC),clang)
	CXX=clang++
endif

EMPTY:=
SPACE:= $(EMPTY) $(EMPTY)

CWARNINGS=-Wall -Wno-multichar -Wno-unused-function -Wno-unused-value
CINCLUDES=-I. $(INCLUDES:%=-I%)

# Some more advanced perf. flags
# FL = -m64 -msse2 -fno-exceptions -fno-unwind-tables

CCFLAGS=-fPIC $(CFLAGS) $(CINCLUDES) $(CWARNINGS) $(PFLAGS:%=-D %)
CXXFLAGS=-std=c++11 $(CCFLAGS)

DEFLIBS=
#stdc++ c m pthread

# Popular libs: rt-realtime extensions, pcap-packet capture, pthread-Posix threads
LDFLAGS=$(patsubst %, -l%, $(DEFLIBS) $(LIBS))


OUTDIRS=$(WRAPDIR) $(OBJDIR) $(BINDIR)
VPATH=$(SRCDIR):$(SRCDIRS:%=$(SRCDIR)/%)


#==============================================================================

build: dirs $(BUILD_TARGETS)

clean: $(CLEAN_TARGETS)
	-rm $(OBJDIR)/*.o
	-rm $(WRAPDIR)/$(WRAPPER_OBJ).cxx

dirs: $(OUTDIRS)

$(OUTDIRS):
	mkdir -p $@
    
# python wrapper
$(WRAPPER_OBJDIR)/$(WRAPPER_OBJ).o: dxapi.i
	swig -c++ -python -I./dxapi/include/native/dxapi -o $(WRAPDIR)/$(WRAPPER_OBJ).cxx -outdir $(INIT_PY_DIR) src/swig/dxapi.i
	cp $(INIT_PY_DIR)/dxapi.py $(INIT_PY_DIR)/__init__.py
	$(CXX) -c $(CXXFLAGS) -o $@ $(WRAPDIR)/$(WRAPPER_OBJ).cxx

# c++ files
$(OBJDIR)/%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $< 
    
$(OBJDIR)/%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

.PHONY: build clean dirs

#==============================================================================
# build lib

DXAPI_PYTHON_LIB_PATH=$(BINDIR)/$(PYHTONAPI_LIB)
DXAPI_PYTHON_LIB_OBJ_PATHS=$(OBJ_LIB:%=$(OBJDIR)/%.o)

# linking
$(BINDIR)/$(PYHTONAPI_LIB): $(DXAPI_PYTHON_LIB_OBJ_PATHS)
	$(CXX) -shared $(DXAPI_PYTHON_LIB_OBJ_PATHS) $(LDFLAGS) -L$(DXAPI_BIN)/ -L$(DFP_BIN)/ -l$(DXAPI_LIB) -l$(DFP_LIB) $(RPATH_PARAM) $(PYTHON_LIBS) $(THIRD_PARTY_LIBS) -o $@

$(CLEAN_TARGETS):
	-rm $(@:clean-%=$(BINDIR)/%)

.PHONY: $(CLEAN_TARGETS)
