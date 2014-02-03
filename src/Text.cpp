/**
 * @file    Text.cpp
 * @brief   Encapsulate a static/constant text rendered with Freetype, ready to be drawn with OpenGL.
 *
 * Copyright (c) 2014 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include <gltext/Text.h>

#include "TextImpl.h"   // NOLINT TODO
#include "FontImpl.h"   // NOLINT TODO

#include <cassert>

namespace gltext {

// Encapsulate the rendered text returned by Font::render(), ready to be drawn with OpenGL.
Text::Text(const std::shared_ptr<const FontImpl>&   aFontImplPtr,
           size_t                                   aTextLength,
           size_t                                   aTextVAO,
           size_t                                   aTextVBO,
           size_t                                   aTextIBO) {
    mImplPtr.reset(new TextImpl(aFontImplPtr, aTextLength, aTextVAO, aTextVBO, aTextIBO));
}

// Cleanup all Freetype and OpenGL ressources when the last reference is destroyed.
Text::~Text() {
    // mImplPtr release its reference to the TextImpl instance
}


// Initialize the 3D position where to start to draw the text.
void Text::setPosition(float aX, float aY, float aZ) {
    assert(mImplPtr);

    mImplPtr->setPosition(aX, aY, aZ);
}

// Ask OpenGL to draw the pre-rendered static text, using the current binded program.
void Text::draw() {
    assert(mImplPtr);

    mImplPtr->draw();
}

} // namespace gltext
