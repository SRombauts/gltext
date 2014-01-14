project(gltext)
cmake_minimum_required(VERSION 2.8)


set(GLTEXT_SOURCES
    src/gltext.cpp
    include/gltext/gltext.h
)
source_group("gltext" FILES ${GLTEXT_SOURCES} README.md LICENSE.txt)
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

add_library(gltext ${GLTEXT_SOURCES} ${HARFBUZZ_SOURCES} ${HARFBUZZ_UCDN_SOURCES})

option(GLTEXT_BUILD_HARFBUZZ_CMDLINE_TEST "Build the small harfbuzz command line tool." OFF)
if (GLTEXT_BUILD_HARFBUZZ_CMDLINE_TEST)
    add_executable(test harfbuzz/src/main.cc)
    target_link_libraries(test ${FREETYPE_LIBRARY} ${OPENGL_gl_LIBRARY} gltext)
endif ()