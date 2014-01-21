/**
 * @file    TextImpl.cpp
 * @brief   Private Implementation of a static/constant text rendered with Freetype, ready to be drawn with OpenGL.
 *
 * Copyright (c) 2014 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include <gltext/Font.h>

#include "TextImpl.h"   // NOLINT TODO
#include "Exception.h"  // NOLINT TODO

#include <stdexcept>
#include <cassert>

namespace gltext {

// Encapsulation.
TextImpl::TextImpl(const Font& aFont) :
    mFontImplPtr(aFont.getImplPtr()) {
}

// Private Implementation of a static/constant text rendered with Freetype, ready to be drawn with OpenGL..
TextImpl::~TextImpl() {
    // releases mFontImplPtr
}

// Initialize the 3D position where to start to draw the text.
void TextImpl::setPosition(float aX, float aY, float aZ) {
    assert(mFontImplPtr);
}

// Ask OpenGL to draw the pre-rendered static text, using the current binded program, at current position.
void TextImpl::draw() {
    assert(mFontImplPtr);
}

} // namespace gltext
