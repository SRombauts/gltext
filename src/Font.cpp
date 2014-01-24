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

#include <cassert>
#include <string>

namespace gltext {

// Ask Freetype to open a Font file and initialize it with the given size
Font::Font(const char* apPathFilename, unsigned int aPixelSize /* = 16 */, unsigned int aCacheSize /* = 100 */) {
    mImplPtr.reset(new FontImpl(apPathFilename, aPixelSize, aCacheSize));
}

// Cleanup all Freetype and OpenGL ressources when the last reference is destroyed.
Font::~Font() {
    // mImplPtr release its reference to the FontImpl instance
}

// Pre-render and cache the glyphs representing the given characters, to speed-up future rendering.
void Font::cache(const std::string& aCharacters) {
    assert(mImplPtr);

    mImplPtr->cache(aCharacters);
}

// Render the given string of characters (or use existing cached glyphs) and put it on a VAO/VBO.
Text Font::render(const std::string& aCharacters) {
    assert(mImplPtr);

    return mImplPtr->render(aCharacters, mImplPtr);
}

// Draw the cache texture for debug purpose.
void Font::drawCache(float aX, float aY, float aW, float aH) {
    assert(mImplPtr);

    mImplPtr->drawCache(aX, aY, aW, aH);
}

} // namespace gltext
