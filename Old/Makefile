# AIMLAB VR Data Streamer - Makefile
# Supports Linux, macOS, and Windows (with MinGW)

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
LDFLAGS = -pthread

# Platform detection
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    PLATFORM = LINUX
endif
ifeq ($(UNAME_S),Darwin)
    PLATFORM = MACOS
endif
ifeq ($(OS),Windows_NT)
    PLATFORM = WINDOWS
    LDFLAGS += -lws2_32
    EXE_EXT = .exe
endif

# Target executable
TARGET = aimlab_network$(EXE_EXT)

# Source files
SOURCES = aimlab_network_cpp.cpp

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Build modes
.PHONY: all clean debug release run help

# Default target
all: release

# Release build (optimized)
release: CXXFLAGS += -DNDEBUG
release: $(TARGET)
	@echo "Release build complete: $(TARGET)"

# Debug build
debug: CXXFLAGS += -g -DDEBUG -O0
debug: $(TARGET)
	@echo "Debug build complete: $(TARGET)"

# Link executable
$(TARGET): $(OBJECTS)
	@echo "Linking $(TARGET)..."
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# Compile source files
%.o: %.cpp
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run the application
run: $(TARGET)
	@echo "Starting AIMLAB VR Data Streamer..."
	./$(TARGET)

# Run without discovery
run-no-discovery: $(TARGET)
	@echo "Starting AIMLAB VR Data Streamer (no discovery)..."
	./$(TARGET) --no-discovery

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	rm -f $(OBJECTS) $(TARGET)
	@echo "Clean complete"

# Help target
help:
	@echo "AIMLAB VR Data Streamer - Build System"
	@echo ""
	@echo "Available targets:"
	@echo "  make            - Build release version (default)"
	@echo "  make debug      - Build debug version with symbols"
	@echo "  make release    - Build optimized release version"
	@echo "  make run        - Build and run the application"
	@echo "  make clean      - Remove build artifacts"
	@echo "  make help       - Show this help message"
	@echo ""
	@echo "Platform detected: $(PLATFORM)"
	@echo "Compiler: $(CXX)"
	@echo "Target: $(TARGET)"

# Platform-specific notes
info:
	@echo "Build Information:"
	@echo "=================="
	@echo "Platform: $(PLATFORM)"
	@echo "Compiler: $(CXX)"
	@echo "C++ Standard: C++17"
	@echo "Target: $(TARGET)"
	@echo ""
	@echo "Compiler flags: $(CXXFLAGS)"
	@echo "Linker flags: $(LDFLAGS)"
