# Compiler to use
CC=gcc

# Compiler flags
CFLAGS=-Isrc -Wall -ggdb -O0

# Linker flags
LDFLAGS=-Lbuild

# Libraries to link
LDLIBS=-ldl

# Directories
SRCDIR=.
TESTSRCDIR=../
BUILDDIR=.
TESTBUILDDIR=../
BINDIR=.

# File extensions
SRCEXT=c


# Names for make run
EXECUTABLE=leakcount
SHIM_LIBRARY=memory_shim


# Find all source files
SOURCES=$(shell find $(SRCDIR) -type f -name "*.$(SRCEXT)" | grep -v "$(SHIM_LIBRARY).c")

# Generate object files for all source files
OBJECTS=$(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))


# Default target
all: clean $(BINDIR)/$(SHIM_LIBRARY).so $(BINDIR)/$(EXECUTABLE)
$(BINDIR)/$(SHIM_LIBRARY).so: $(SRCDIR)/$(SHIM_LIBRARY).c
	$(CC) $(CFLAGS) -shared -fPIC $(SRCDIR)/$(SHIM_LIBRARY).c -o $(BINDIR)/$(SHIM_LIBRARY).so -Wl,--no-as-needed -ldl

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