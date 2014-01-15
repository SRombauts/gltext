/**
 * @file    Font.cpp
 * @brief   C++ Library to use freetype under OpenGL.
 *
 * Copyright (c) 2014 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include <gltext/Font.h>

#include "FontPimpl.h"

namespace gltext {

// Ask Freetype to open a Font file and initialize it with the given size
Font::Font(const char* apFontFilename, unsigned int aPointSize /* = 16 */, unsigned int aCacheSize /* = 100 */) {
}

// Cleanup all Freetype and OpenGL ressources when the last reference is destroyed.
Font::~Font() {
}

} // namespace gltext
