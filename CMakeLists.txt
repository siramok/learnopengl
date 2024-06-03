cmake_minimum_required(VERSION 3.10)

# Set the project name
project(learnopengl)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/src/*.c)
file(GLOB_RECURSE HEADERS ${PROJECT_SOURCE_DIR}/include/*.h ${PROJECT_SOURCE_DIR}/src/*.h)

# Add the executable
add_executable(learnopengl ${SOURCES} ${HEADERS})

# Find the GLFW package
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW3 REQUIRED glfw3)

# Find the OpenGL package
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

# Include GLFW
include_directories(${GLFW3_INCLUDE_DIRS})

# Link the GLFW and OpenGL libraries
target_link_libraries(learnopengl ${GLFW3_LIBRARIES} ${OPENGL_LIBRARIES})

# Define the source and destination directories for shaders
set(SHADERS_SRC_DIR "${PROJECT_SOURCE_DIR}/src/assets")
set(SHADERS_DEST_DIR "${CMAKE_BINARY_DIR}/assets")

# Add a custom target to copy the assets directory
add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${SHADERS_SRC_DIR} ${SHADERS_DEST_DIR}
    COMMENT "Copying shaders directory to build directory"
)

# Ensure the copy_shaders target is executed before building the executable
add_dependencies(learnopengl copy_assets)
