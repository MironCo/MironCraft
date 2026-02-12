# MironCraft Makefile
# Simple build system for macOS

# Compiler and flags
CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra
INCLUDES = -Imironcraft/Dependencies/include -I/opt/homebrew/include
LDFLAGS = -Lmironcraft/Dependencies/lib -L/opt/homebrew/lib
LIBS = -lglfw -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo

# Directories
SRC_DIR = mironcraft/src
BUILD_DIR = build
BIN_DIR = bin

# Source files
SOURCES = $(SRC_DIR)/main.cpp \
          $(SRC_DIR)/stb.cpp \
          mironcraft/glad.c

# Object files
OBJECTS = $(BUILD_DIR)/main.o \
          $(BUILD_DIR)/stb.o \
          $(BUILD_DIR)/glad.o

# Output binary
TARGET = $(BIN_DIR)/MironCraft

# Default target
all: directories $(TARGET)

# Create necessary directories
directories:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BIN_DIR)

# Link the final executable
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) $(LIBS) -o $(TARGET)
	@echo "Build complete! Run with: ./$(TARGET)"

# Compile main.cpp
$(BUILD_DIR)/main.o: $(SRC_DIR)/main.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compile stb.cpp
$(BUILD_DIR)/stb.o: $(SRC_DIR)/stb.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compile glad.c
$(BUILD_DIR)/glad.o: mironcraft/glad.c
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "Clean complete!"

# Run the program
run: all
	./$(TARGET)

# Rebuild everything
rebuild: clean all

# Clean, build, and run
clean-run: clean all run

.PHONY: all clean run rebuild clean-run directories
