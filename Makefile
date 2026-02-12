# MironCraft Makefile
# Simple build system for macOS

# Compiler and flags
CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra
INCLUDES = -Imironcraft/include -Imironcraft/Dependencies/include -I/opt/homebrew/include
LDFLAGS = -Lmironcraft/Dependencies/lib -L/opt/homebrew/lib
LIBS = -lglfw -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo

# Directories
SRC_DIR = mironcraft/src
BUILD_DIR = build
BIN_DIR = bin

# Source files
SOURCES = $(SRC_DIR)/main.cpp \
          $(SRC_DIR)/stb.cpp \
          $(SRC_DIR)/Application.cpp \
          $(SRC_DIR)/Window.cpp \
          $(SRC_DIR)/Game.cpp \
          $(SRC_DIR)/VertexBuffer.cpp \
          $(SRC_DIR)/IndexBuffer.cpp \
          $(SRC_DIR)/VertexArray.cpp \
          $(SRC_DIR)/Shader.cpp \
          $(SRC_DIR)/Texture.cpp \
          $(SRC_DIR)/Block.cpp \
          $(SRC_DIR)/Tree.cpp \
          $(SRC_DIR)/Chunk.cpp \
          $(SRC_DIR)/Renderer.cpp \
          $(SRC_DIR)/Camera.cpp \
          $(SRC_DIR)/Common.cpp \
          mironcraft/glad.c

# Object files
OBJECTS = $(BUILD_DIR)/main.o \
          $(BUILD_DIR)/stb.o \
          $(BUILD_DIR)/Application.o \
          $(BUILD_DIR)/Window.o \
          $(BUILD_DIR)/Game.o \
          $(BUILD_DIR)/VertexBuffer.o \
          $(BUILD_DIR)/IndexBuffer.o \
          $(BUILD_DIR)/VertexArray.o \
          $(BUILD_DIR)/Shader.o \
          $(BUILD_DIR)/Texture.o \
          $(BUILD_DIR)/Block.o \
          $(BUILD_DIR)/Tree.o \
          $(BUILD_DIR)/Chunk.o \
          $(BUILD_DIR)/Renderer.o \
          $(BUILD_DIR)/Camera.o \
          $(BUILD_DIR)/Common.o \
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

# Pattern rule for compiling .cpp files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
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
