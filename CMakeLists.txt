cmake_minimum_required(VERSION 3.21)
project(Software-Renderer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_definitions(-DGLM_ENABLE_EXPERIMENTAL)

include(FetchContent)


FetchContent_Declare(
    SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-2.30.0
)

FetchContent_MakeAvailable(SDL2)


FetchContent_Declare(
    SDL2_image
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
    GIT_TAG release-2.8.0
)

set(BUILD_SHARED_LIBS OFF)
FetchContent_MakeAvailable(SDL2_image)


FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
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

add_executable(Software-Renderer ${IMP_FILES} ${INCLUDE_DIR})

target_include_directories(Software-Renderer PUBLIC ${glm_SOURCE_DIR} ${INCLUDE_DIR})

target_link_libraries(Software-Renderer PUBLIC SDL2::SDL2-static SDL2_image::SDL2_image-static SDL2::SDL2main)
add_custom_command(
    TARGET Software-Renderer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:Software-Renderer>/assets
)

add_custom_target(CopyAssets ALL
    DEPENDS Software-Renderer
)
