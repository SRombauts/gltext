/**
 * @file    FontImpl.cpp
 * @brief   Private Implementation of the Freetype / HarfBuzz Font rendering.
 *
 * Copyright (c) 2014 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include "FontImpl.h"   // NOLINT TODO
#include "Freetype.h"   // NOLINT TODO
#include "Exception.h"  // NOLINT TODO

#include <stdexcept>

namespace gltext {

// Ask Freetype to open a Font file and initialize it with the given size
FontImpl::FontImpl(const char* apPathFilename, unsigned int aPointSize, unsigned int aCacheSize) :
    mPathFilename(apPathFilename),
    mPointSize(aPointSize),
    mCacheSize(aCacheSize) {
    Freetype& freetype = Freetype::getInstance();
    FT_Error error = FT_New_Face(freetype.getLibrary(), mPathFilename.c_str(), 0, &mFace);
    if (error) {
        throw Exception("FT_New_Face error");
    }
}

// Cleanup all Freetype and OpenGL ressources when the last reference is destroyed.
FontImpl::~FontImpl() {
}

} // namespace gltext
