/**
 * @file    FontImpl.h
 * @brief   Private Implementation of the Freetype / HarfBuzz Font rendering.
 *
 * Copyright (c) 2014 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include <string>

#include <hb-ft.h>  // HarfBuzz Freetype interface

namespace gltext {

/**
 * @brief Private Implementation of the Freetype / HarfBuzz Font rendering.
 *
 * @see the Font class for detailed explanation
 *
 *  Using an "opaque pointer" to a private implementation (pimpl) is the way to protect the client application
 * from the inclusion of Freetype and HarfBuzz libraries.
 */
class FontImpl {
public:
    /**
     * @brief Ask Freetype to open a Font file and initialize it with the given size
     *
     * @see Font::Font() for detailed explanation
     *
     * @param[in] apPathFilename    Path to the OpenType font file to open with Freetype.
     * @param[in] aPointSize        Vertical size of the font in pixel
     * @param[in] aCacheSize        Minimum number of characters to allocate into the cache (use a square value).
     */
    FontImpl(const char* apPathFilename, unsigned int aPointSize, unsigned int aCacheSize);
    /**
     * @brief Cleanup all Freetype and OpenGL ressources when the last reference is destroyed.
     */
    ~FontImpl();

private:
    std::string     mPathFilename;  ///< Path to the OpenType font file to open with Freetype.
    unsigned int    mPointSize;     ///< Vertical size of the font in pixel
    unsigned int    mCacheSize;     ///< Minimum number of characters to allocate into the cache (use a square value).

    FT_Face         mFace;          ///< Handle to the typographic face object (given typeface/font, in a given style).
};

} // namespace gltext
