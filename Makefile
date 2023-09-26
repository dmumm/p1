# Compiler to use
CC=gcc

# Compiler flags
CFLAGS=-Isrc -Wall -ggdb -O0

# Linker flags
LDFLAGS=-Lbuild

# Libraries to link
LDLIBS=

# Directories
SRCDIR=.
TESTSRCDIR=../
BUILDDIR=.
TESTBUILDDIR=../
BINDIR=.

# File extensions
SRCEXT=c

# Find all source files
SOURCES=$(shell find $(SRCDIR) -type f -name "*.$(SRCEXT)")

# Generate object files for all source files
OBJECTS=$(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))

# Names for make run
EXECUTABLE=leakcount
SHIM_LIBRARY=memory_shim


# Default target
all: clean $(BINDIR)/$(EXECUTABLE) $(BINDIR)/$(SHIM_LIBRARY).so $(BINDIR)/$(TEST_EXECUTABLE)

$(BINDIR)/$(SHIM_LIBRARY).so: $(SRCDIR)/$(SHIM_LIBRARY).c
	$(CC) $(CFLAGS) -shared -fPIC $(SRCDIR)/$(SHIM_LIBRARY).c -o $(BINDIR)/$(SHIM_LIBRARY).so -ldl

# Link object files to generate the executable
$(BINDIR)/$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

# Compile source files to object files
$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BINDIR)/$(TEST_EXECUTABLE): $(TESTOBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

 # Compile test source files to object files
$(TESTBUILDDIR)/%.o: $(TESTSRCDIR)/%.$(SRCEXT)
	$(CC) $(CFLAGS) -c -o $@ $<

# Clean up generated files
clean:
	$(RM) -r *.o
	$(RM) -r ../*.o
	$(RM) -r *.so
	$(RM) -r $(EXECUTABLE)


# Run with specified arguments
run:
	$(BINDIR)/$(EXECUTABLE) $(BINDIR)/$(TEST_EXECUTABLE) $(TEST_EXECUTABLE_ARGS)

# Declare clean as a phony target
.PHONY: clean run