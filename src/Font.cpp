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

#include "FontImpl.h"   // NOLINT TODO

namespace gltext {

// Ask Freetype to open a Font file and initialize it with the given size
Font::Font(const char* apPathFilename, unsigned int aPointSize /* = 16 */, unsigned int aCacheSize /* = 100 */) {
    mImplPtr.reset(new FontImpl(apPathFilename, aPointSize, aCacheSize));
}

// Cleanup all Freetype and OpenGL ressources when the last reference is destroyed.
Font::~Font() {
    // mImplPtr release its reference to the FontImpl instance
}

// TODO SRombauts
void Font::cache(const char* apCharacters) {
}

// TODO SRombauts
Text Font::render(const char* apCharacters) {
    return Text(*this);
}

} // namespace gltext
