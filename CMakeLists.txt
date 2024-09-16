cmake_minimum_required(VERSION 3.14)
project(Software-Renderer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# # Fetch SDL2
# FetchContent_Declare(
#     SDL2
#     GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
#     GIT_TAG release-2.26.5
# )
# FetchContent_GetProperties(SDL2)
# if(NOT SDL2_POPULATED)
#     FetchContent_Populate(SDL2)
#     # Add SDL2 to the build
#     add_subdirectory(${SDL2_SOURCE_DIR} ${SDL2_BINARY_DIR})
# endif()

# # Set SDL2_DIR so SDL2_image can find SDL2
# set(SDL2_DIR ${SDL2_BINARY_DIR})
# set(SDL2IMAGE_SDL2_DEPENDENCY_TYPE "cmake")

# # Fetch SDL2_image
# FetchContent_Declare(
#     SDL2_image
#     GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
#     GIT_TAG release-2.6.3
# )
# FetchContent_GetProperties(SDL2_image)
# if(NOT SDL2_image_POPULATED)
#     FetchContent_Populate(SDL2_image)
#     # Add SDL2_image to the build
#     add_subdirectory(${SDL2_image_SOURCE_DIR} ${SDL2_image_BINARY_DIR})
# endif()


FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 0.9.9.6
)
FetchContent_MakeAvailable(glm)

find_package(OpenMP)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/assets)

set(SOURCE_DIR "src")
set(INCLUDE_DIR "include")

set(IMP_FILES 
    ${SOURCE_DIR}/main.cpp
    ${SOURCE_DIR}/renderer.cpp
    ${SOURCE_DIR}/drawing.cpp
    ${SOURCE_DIR}/texture.cpp
    ${SOURCE_DIR}/model.cpp
    ${SOURCE_DIR}/camera.cpp
    ${SOURCE_DIR}/display.cpp
    ${SOURCE_DIR}/scene.cpp
    ${SOURCE_DIR}/input.cpp
    ${SOURCE_DIR}/engine.cpp
)

set(HEADER_FILES
    ${INCLUDE_DIR}/renderer.h
    ${INCLUDE_DIR}/drawing.h
    ${INCLUDE_DIR}/texture.h
    ${INCLUDE_DIR}/model.h
    ${INCLUDE_DIR}/camera.h
    ${INCLUDE_DIR}/display.h
    ${INCLUDE_DIR}/scene.h
    ${INCLUDE_DIR}/input.h
    ${INCLUDE_DIR}/engine.h
)

add_executable(Software-Renderer ${IMP_FILES} ${HEADER_FILES})

target_include_directories(Software-Renderer PUBLIC ${glm_SOURCE_DIR} ${INCLUDE_DIR})

target_link_libraries(Software-Renderer 
    PUBLIC 
        SDL2::SDL2 
        SDL2_image::SDL2_image 
        SDL2::SDL2main
        OpenMP::OpenMP_CXX
)

if (SDL2_STATIC)
    target_compile_definitions(Software-Renderer PRIVATE SDL_MAIN_HANDLED)
endif()

add_custom_command(
    TARGET Software-Renderer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:Software-Renderer>/assets
)

add_custom_target(CopyAssets ALL
    DEPENDS Software-Renderer
)
