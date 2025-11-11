# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -I./include -O2 -std=c11

# Directories
SRCDIR = src
OBJDIR = obj
BINDIR = bin
RESDIR = results
SORTDIR = src/sorts

# Source files
MAIN_SRC = main.c
LIB_SOURCES = $(wildcard $(SRCDIR)/*.c)
SORT_SOURCES = $(wildcard $(SORTDIR)/*.c)

# Object files
MAIN_OBJ = $(OBJDIR)/main.o
LIB_OBJECTS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(LIB_SOURCES))
SORT_OBJECTS = $(patsubst $(SORTDIR)/%.c, $(OBJDIR)/sorts_%.o, $(SORT_SOURCES))

ALL_OBJECTS = $(MAIN_OBJ) $(LIB_OBJECTS) $(SORT_OBJECTS)

# Target executable
TARGET = $(BINDIR)/benchmark

# Default target
all: directories $(TARGET)

# Create necessary directories
directories:
	@mkdir -p $(OBJDIR) $(BINDIR) $(RESDIR) $(SORTDIR)

# Link object files to create executable
$(TARGET): $(ALL_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^
	@echo "Build successful! Executable: $(TARGET)"

# Compile main.c
$(OBJDIR)/main.o: $(MAIN_SRC)
	$(CC) $(CFLAGS) -c -o $@ $<

# Compile library source files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Compile sorting algorithm source files
$(OBJDIR)/sorts_%.o: $(SORTDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Clean build artifacts
clean:
	rm -rf $(OBJDIR) $(BINDIR)
	@echo "Cleaned build artifacts"

# Clean everything including results
cleanall: clean
	rm -rf $(RESDIR)
	@echo "Cleaned all files including results"

# Run the benchmark
run: all
	@echo "Running benchmark..."
	./$(TARGET)

# Install gnuplot (for macOS)
install-deps:
	@echo "Installing gnuplot..."
	brew install gnuplot

# Help message
help:
	@echo "Available targets:"
	@echo "  make          - Build the project"
	@echo "  make run      - Build and run benchmark"
	@echo "  make clean    - Remove build artifacts"
	@echo "  make cleanall - Remove build artifacts and results"
	@echo "  make help     - Show this help message"

.PHONY: all clean cleanall run directories install-deps help
