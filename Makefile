# Compiler to use
CC=gcc

# Compiler flags
CFLAGS=-Isrc -Wall -ggdb -O0

# Linker flags
LDFLAGS=-Lbuild

# Libraries to link
LDLIBS=-lm

# Directories
SRCDIR=src
TESTSRCDIR=test_src
BUILDDIR=build
TESTBUILDDIR=test_build
BINDIR=bin

# File extensions
SRCEXT=c

# Find all source files
SOURCES=$(shell find $(SRCDIR) -type f -name "*.$(SRCEXT)")

# Generate object files for all source files
OBJECTS=$(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))

# Find all test source files
TESTSOURCES=$(shell find $(TESTSRCDIR) -type f -name "*.$(SRCEXT)")

# Generate object files for all test source files
TESTOBJECTS=$(patsubst $(TESTSRCDIR)/%,$(TESTBUILDDIR)/%,$(TESTSOURCES:.$(SRCEXT)=.o))

# Name of the executable
EXECUTABLE=leakcount
TEST_EXECUTABLE=program_to_test
TEST_EXECUTABLE_ARGS=arg1 arg2
SHIM_LIBRARY=memory_shim


# Default target
all: clean $(BINDIR)/$(EXECUTABLE) $(BINDIR)/$(SHIM_LIBRARY).so $(BINDIR)/$(TEST_EXECUTABLE)

$(BINDIR)/$(SHIM_LIBRARY).so: $(SRCDIR)/$(SHIM_LIBRARY).c
	$(CC) $(CFLAGS) -shared -fPIC $(SRCDIR)/$(SHIM_LIBRARY).c -o $(BINDIR)/$(SHIM_LIBRARY).so -ldl

# Link object files to generate the executable
$(BINDIR)/$(EXECUTABLE): $(OBJECTS)
	mkdir -p $(BINDIR)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

# Compile source files to object files
$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BINDIR)/$(TEST_EXECUTABLE): $(TESTOBJECTS)
	mkdir -p $(BINDIR)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

 # Compile test source files to object files
$(TESTBUILDDIR)/%.o: $(TESTSRCDIR)/%.$(SRCEXT)
	mkdir -p $(TESTBUILDDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

# Clean up generated files
clean:
	$(RM) -r $(BUILDDIR) $(TESTBUILDDIR) $(BINDIR)

run:
	$(BINDIR)/$(EXECUTABLE) $(BINDIR)/$(TEST_EXECUTABLE) $(TEST_EXECUTABLE_ARGS)

# Declare clean as a phony target
.PHONY: clean run