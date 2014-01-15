/**
 * @file    FontPimpl.cpp
 * @brief   Private Implementation of the Freetype / HarfBuzz Font rendering.
 *
 * Copyright (c) 2014 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include "FontPimpl.h"
#include "Freetype.h"
#include "Exception.h"

#include <stdexcept>

namespace gltext {

FontPimpl::FontPimpl(const char* apFontFilename, unsigned int aPointSize, unsigned int aCacheSize) :
    mFontFilename(apFontFilename),
    mPointSize(aPointSize),
    mCacheSize(aCacheSize) {
    Freetype& freetype = Freetype::getInstance();
}
FontPimpl::~FontPimpl() {
}

} // namespace gltext
