# Copyright (c) 2014 Sebastien Rombauts (sebastien.rombauts@gmail.com)
#
# Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
# or copy at http://opensource.org/licenses/MIT) 

cmake_minimum_required(VERSION 2.6)
project(gltext)

# Define useful variables to handle OS/Compiler differences
if (MSVC)
    set(CPPLINT_ARG_OUTPUT      "--output=vs7")
    set(CPPCHECK_ARG_TEMPLATE   "--template=vs")
    set(DEV_NULL                "NUL")
    # disable Visual Studio usual warnings
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
elseif (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CPPLINT_ARG_OUTPUT      "--output=eclipse")
    set(CPPCHECK_ARG_TEMPLATE   "--template=gcc")
    set(DEV_NULL                "/dev/null")
    message(STATUS "C++11 activated for std::shared_ptr")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")  # -std=c++11
endif ()
## TODO SRO : tests
set(CPPLINT_ARG_VERBOSE "--verbose=3")
set(CPPLINT_ARG_LINELENGTH "--linelength=120")


## Core source code ##

set(GLTEXT_API
    include/gltext/Font.h
    include/gltext/Text.h
)
set(GLTEXT_SOURCES
    src/Font.cpp
    src/FontImpl.cpp src/FontImpl.h
    src/Text.cpp
    src/TextImpl.cpp src/TextImpl.h
    src/Freetype.cpp src/Freetype.h
    src/Exception.h
    # Temporary: replace with glload or glew
    src/glload.cpp src/glload.hpp
)
source_group(src FILES ${GLTEXT_SOURCES})
source_group(api FILES ${GLTEXT_API} README.md LICENSE.txt)
include_directories(include)

set(HARFBUZZ_SOURCES
    harfbuzz/src/hb-atomic-private.hh
    harfbuzz/src/hb-blob.cc
    harfbuzz/src/hb-blob.h
    harfbuzz/src/hb-buffer.cc
    harfbuzz/src/hb-buffer-deserialize-json.hh
    harfbuzz/src/hb-buffer-deserialize-text.hh
    harfbuzz/src/hb-buffer.h
    harfbuzz/src/hb-buffer-private.hh
    harfbuzz/src/hb-buffer-serialize.cc
    harfbuzz/src/hb-cache-private.hh
    harfbuzz/src/hb-common.cc
    harfbuzz/src/hb-common.h
#    harfbuzz/src/hb-coretext.cc
#    harfbuzz/src/hb-coretext.h
    harfbuzz/src/hb-deprecated.h
    harfbuzz/src/hb-face.cc
    harfbuzz/src/hb-face.h
    harfbuzz/src/hb-face-private.hh
    harfbuzz/src/hb-fallback-shape.cc
    harfbuzz/src/hb-font.cc
    harfbuzz/src/hb-font.h
    harfbuzz/src/hb-font-private.hh
    harfbuzz/src/hb-ft.cc
    harfbuzz/src/hb-ft.h
#    harfbuzz/src/hb-glib.cc
#    harfbuzz/src/hb-glib.h
#    harfbuzz/src/hb-gobject.h
#    harfbuzz/src/hb-gobject-structs.cc
#    harfbuzz/src/hb-gobject-structs.h
#    harfbuzz/src/hb-graphite2.cc
#    harfbuzz/src/hb-graphite2.h
    harfbuzz/src/hb.h
#    harfbuzz/src/hb-icu.cc
#    harfbuzz/src/hb-icu.h
    harfbuzz/src/hb-mutex-private.hh
    harfbuzz/src/hb-object-private.hh
    harfbuzz/src/hb-open-file-private.hh
    harfbuzz/src/hb-open-type-private.hh
    harfbuzz/src/hb-ot.h
    harfbuzz/src/hb-ot-head-table.hh
    harfbuzz/src/hb-ot-hhea-table.hh
    harfbuzz/src/hb-ot-hmtx-table.hh
    harfbuzz/src/hb-ot-layout.cc
    harfbuzz/src/hb-ot-layout-common-private.hh
    harfbuzz/src/hb-ot-layout-gdef-table.hh
    harfbuzz/src/hb-ot-layout-gpos-table.hh
    harfbuzz/src/hb-ot-layout-gsubgpos-private.hh
    harfbuzz/src/hb-ot-layout-gsub-table.hh
    harfbuzz/src/hb-ot-layout.h
    harfbuzz/src/hb-ot-layout-jstf-table.hh
    harfbuzz/src/hb-ot-layout-private.hh
    harfbuzz/src/hb-ot-map.cc
    harfbuzz/src/hb-ot-map-private.hh
    harfbuzz/src/hb-ot-maxp-table.hh
    harfbuzz/src/hb-ot-name-table.hh
    harfbuzz/src/hb-ot-shape.cc
    harfbuzz/src/hb-ot-shape-complex-arabic.cc
    harfbuzz/src/hb-ot-shape-complex-arabic-fallback.hh
    harfbuzz/src/hb-ot-shape-complex-arabic-table.hh
    harfbuzz/src/hb-ot-shape-complex-default.cc
    harfbuzz/src/hb-ot-shape-complex-indic.cc
    harfbuzz/src/hb-ot-shape-complex-indic-machine.hh
    harfbuzz/src/hb-ot-shape-complex-indic-private.hh
    harfbuzz/src/hb-ot-shape-complex-indic-table.cc
    harfbuzz/src/hb-ot-shape-complex-myanmar.cc
    harfbuzz/src/hb-ot-shape-complex-myanmar-machine.hh
    harfbuzz/src/hb-ot-shape-complex-private.hh
    harfbuzz/src/hb-ot-shape-complex-sea.cc
    harfbuzz/src/hb-ot-shape-complex-sea-machine.hh
    harfbuzz/src/hb-ot-shape-complex-thai.cc
    harfbuzz/src/hb-ot-shape-fallback.cc
    harfbuzz/src/hb-ot-shape-fallback-private.hh
    harfbuzz/src/hb-ot-shape-normalize.cc
    harfbuzz/src/hb-ot-shape-normalize-private.hh
    harfbuzz/src/hb-ot-shape-private.hh
    harfbuzz/src/hb-ot-tag.cc
    harfbuzz/src/hb-ot-tag.h
    harfbuzz/src/hb-private.hh
    harfbuzz/src/hb-set.cc
    harfbuzz/src/hb-set.h
    harfbuzz/src/hb-set-private.hh
    harfbuzz/src/hb-shape.cc
    harfbuzz/src/hb-shape.h
    harfbuzz/src/hb-shape-plan.cc
    harfbuzz/src/hb-shape-plan.h
    harfbuzz/src/hb-shape-plan-private.hh
    harfbuzz/src/hb-shaper.cc
    harfbuzz/src/hb-shaper-impl-private.hh
    harfbuzz/src/hb-shaper-list.hh
    harfbuzz/src/hb-shaper-private.hh
    harfbuzz/src/hb-tt-font.cc
    harfbuzz/src/hb-ucdn.cc
    harfbuzz/src/hb-unicode.cc
    harfbuzz/src/hb-unicode.h
    harfbuzz/src/hb-unicode-private.hh
#    harfbuzz/src/hb-uniscribe.cc
#    harfbuzz/src/hb-uniscribe.h
    harfbuzz/src/hb-utf-private.hh
    harfbuzz/src/hb-version.h
    harfbuzz/src/hb-warning.cc
)
include_directories(harfbuzz/src/)

set(HARFBUZZ_UCDN_SOURCES
    harfbuzz/src/hb-ucdn/ucdn.c
    harfbuzz/src/hb-ucdn/ucdn.h
    harfbuzz/src/hb-ucdn/unicodedata_db.h
)
include_directories(harfbuzz/src/hb-ucdn/)

source_group("harbuzz" FILES ${HARFBUZZ_SOURCES} ${HARFBUZZ_UCDN_SOURCES})

find_package(OpenGL REQUIRED)
find_package(Freetype REQUIRED)

include_directories(${FREETYPE_INCLUDE_DIRS})

add_definitions(-DHAVE_OT=1)
add_definitions(-DHAVE_UCDN=1)

add_library(gltext ${GLTEXT_SOURCES} ${GLTEXT_API} ${HARFBUZZ_SOURCES} ${HARFBUZZ_UCDN_SOURCES})

option(GLTEXT_BUILD_HARFBUZZ_CMDLINE_TEST "Build the small harfbuzz command line tool." OFF)
if (GLTEXT_BUILD_HARFBUZZ_CMDLINE_TEST)
    add_executable(test harfbuzz/src/main.cc)
    target_link_libraries(test ${FREETYPE_LIBRARY} ${OPENGL_gl_LIBRARY} gltext)
endif ()


# Optional additional targets:

option(GLTEXT_RUN_CPPLINT "Run cpplint.py tool for Google C++ StyleGuide." ON)
if (GLTEXT_RUN_CPPLINT)
    # add a cpplint target to the "all" target
    add_custom_target(gltext_cpplint
     ALL
     COMMAND python cpplint.py ${CPPLINT_ARG_OUTPUT} ${CPPLINT_ARG_VERBOSE} ${CPPLINT_ARG_LINELENGTH} ${GLTEXT_SOURCES} ${GLTEXT_API}
     WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    )
else()
    message(STATUS "GLTEXT_RUN_CPPLINT OFF")
endif()

option(GLTEXT_RUN_CPPCHECK "Run cppcheck C++ static analysis tool." ON)
if (GLTEXT_RUN_CPPCHECK)
    # add a cppcheck target to the "all" target
    add_custom_target(gltext_cppcheck
     ALL
     COMMAND cppcheck -j 4 cppcheck --enable=style --quiet ${CPPCHECK_ARG_TEMPLATE} ${PROJECT_SOURCE_DIR}/src
    )
else()
    message(STATUS "GLTEXT_RUN_CPPCHECK OFF")
endif()

option(GLTEXT_RUN_DOXYGEN "Run Doxygen C++ documentation tool." ON)
if (GLTEXT_RUN_DOXYGEN)
    # add a Doxygen target to the "all" target
    add_custom_target(gltext_doxygen
     ALL
     COMMAND doxygen Doxyfile > ${DEV_NULL}
     WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    )
else()
    message(STATUS "GLTEXT_RUN_DOXYGEN OFF")
endif()
