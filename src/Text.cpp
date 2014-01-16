/**
 * @file    Text.cpp
 * @brief   A text rendered with freetype, ready to draw with OpenGL.
 *
 * Copyright (c) 2014 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include <gltext/Font.h>

#include "TextImpl.h"   // NOLINT TODO
#include "FontImpl.h"   // NOLINT TODO

namespace gltext {

// TODO
Text::Text(const Font& aFont) {
    mImplPtr.reset(new TextImpl(aFont));
}

// Cleanup all Freetype and OpenGL ressources when the last reference is destroyed.
Text::~Text() {
    // mImplPtr release its reference to the FontImpl instance
}

} // namespace gltext
