/**
 * @file    TextImpl.cpp
 * @brief   Private Implementation of the Freetype / HarfBuzz Text rendering.
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

namespace gltext {

// Ask Freetype to open a Text file and initialize it with the given size
TextImpl::TextImpl(const Font& aFont) :
    mFontImplPtr(aFont.getImplPtr()) {
}

// Cleanup all Freetype and OpenGL ressources when the last reference is destroyed.
TextImpl::~TextImpl() {
}

} // namespace gltext
