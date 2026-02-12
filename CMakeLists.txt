cmake_minimum_required(VERSION 3.15)
project(MironCraft)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source files
set(SOURCES
    "mironcraft/src/main.cpp"
    "mironcraft/src/stb.cpp"
    "mironcraft/glad.c"
)

# Create executable
add_executable(MironCraft ${SOURCES})

# Include directories
target_include_directories(MironCraft PRIVATE
    "mironcraft/Dependencies/include"
)

# Link directories
target_link_directories(MironCraft PRIVATE
    "mironcraft/Dependencies/lib"
)

# Platform-specific linking
if(APPLE)
    # macOS frameworks and libraries
    target_link_libraries(MironCraft PRIVATE
        glfw3
        "-framework OpenGL"
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )
elseif(UNIX)
    # Linux libraries
    target_link_libraries(MironCraft PRIVATE
        glfw3
        GL
        X11
        pthread
        Xrandr
        Xi
        dl
    )
elseif(WIN32)
    # Windows libraries
    target_link_libraries(MironCraft PRIVATE
        glfw3
        opengl32
    )
endif()

# Set output directory
set_target_properties(MironCraft PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
