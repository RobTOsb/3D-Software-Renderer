cmake_minimum_required(VERSION 3.2)
project(Software-Renderer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# Find the SDL2 library
find_package(SDL2 REQUIRED)
# Find the SDL2_image library
find_package(SDL2_image REQUIRED)

# Find OpenMP
find_package(OpenMP)

# Compiles without omp if not found
if(OpenMP_CXX_FOUND)
    # Add OpenMP flags to the compiler and linker
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_CXX_FLAGS}")
else()
    message(WARNING "OpenMP not found, the project will be built without OpenMP support.")
endif()

# It doesn't find the include dir automatically so I have to find it manually
find_path(SDL2_IMAGE_INCLUDE_DIR SDL_image.h)
set(SDL2_IMAGE_INCLUDE_DIRS ${SDL2_IMAGE_INCLUDE_DIR})

# Find the SDL2_image library
find_library(SDL2_IMAGE_LIBRARY SDL2_image)

# Set the SDL2_image libraries variable
set(SDL2_IMAGE_LIBRARIES ${SDL2_IMAGE_LIBRARY})

# include asset directory
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/assets)

# Add the SDL2 include directory to the include path
include_directories(${SDL2_INCLUDE_DIRS})
# Add the SDL2_image include directory to the include path
include_directories(${SDL2_IMAGE_INCLUDE_DIRS})

# Set the source directory
set(SOURCE_DIR "src")


# Get a list of all the .cpp and .h files in the source directory and its subdirectories
file(GLOB_RECURSE SOURCE_FILES "${SOURCE_DIR}/*.cpp")

# It doesn't find all my header files for some reason so I put this here
file(GLOB_RECURSE HEADER_FILES include/*.h include/*.hpp)

# Add the include directory to the include path
include_directories(include)

# Create the executable
add_executable(Software-Renderer ${SOURCE_FILES} ${HEADER_FILES} ${LIB_FILES})

# Link the SDL2 and SDL2_image libraries
target_link_libraries(Software-Renderer ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})
