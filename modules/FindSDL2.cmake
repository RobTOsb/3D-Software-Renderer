# Try to find SDL2 library
# If found, define SDL2_FOUND
# and set SDL2_INCLUDE_DIRS and SDL2_LIBRARIES
#
# This module respects the following environment variables:
#
# SDL2DIR           - hint for the base SDL2 directory
#                     (usually contains include/SDL2 and lib/)

if(WIN32)
    set(SDL2_SEARCH_DIRS
        "$ENV{SDL2DIR}"
        "C:/Program Files/SDL2"
        "C:/Program Files (x86)/SDL2"
        "C:/SDL2"
        "C:/SDL2-devel"
        "c:/vclib/SDL2-2.26.3"
        )
else()
    set(SDL2_SEARCH_DIRS
        "$ENV{SDL2DIR}"
        "/usr/local"
        "/usr"
        "/usr/local/include/SDL2"
        "/usr/include/SDL2"
        )
endif()

find_path(SDL2_INCLUDE_DIR SDL.h
    HINTS ${SDL2_SEARCH_DIRS}
    PATH_SUFFIXES SDL2
    )

find_library(SDL2_LIBRARY
    NAMES SDL2 SDL2main
    HINTS ${SDL2_SEARCH_DIRS}
    PATH_SUFFIXES lib64 lib
    )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SDL2
    DEFAULT_MSG
    SDL2_INCLUDE_DIR
    SDL2_LIBRARY
    )

mark_as_advanced(SDL2_INCLUDE_DIR SDL2_LIBRARY)
