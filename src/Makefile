# Compiler to use
CC=gcc

# Compiler flags
CFLAGS=-Isrc -Wall

# Linker flags
LDFLAGS=-Lbuild

# Libraries to link
LDLIBS=-lm

# Directories
SRCDIR=src
TESTDIR=tests
BUILDDIR=../build
BINDIR=../bin

# File extensions
SRCEXT=c

# Find all source files
SOURCES=$(shell find $(SRCDIR) -type f -name *.$(SRCEXT))

# Generate object files for all source files
OBJECTS=$(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))

# Find all test source files
TESTSOURCES=$(shell find $(TESTDIR) -type f -name *.$(SRCEXT))

# Generate object files for all test source files
TESTOBJECTS=$(patsubst $(TESTDIR)/%,$(BUILDDIR)/%,$(TESTSOURCES:.$(SRCEXT)=.o))

# Name of the executable
EXECUTABLE=program

# Default target
all: $(BINDIR)/$(EXECUTABLE)

# Link object files to generate the executable
$(BINDIR)/$(EXECUTABLE): $(OBJECTS) $(TESTOBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

# Compile source files to object files
$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

# Compile test source files to object files
$(BUILDDIR)/%.o: $(TESTDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

# Clean up generated files
clean:
	$(RM) -r $(BUILDDIR) $(BINDIR)

# Declare clean as a phony target
.PHONY: clean