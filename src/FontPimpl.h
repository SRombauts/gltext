/**
 * @file    FontPimpl.h
 * @brief   Private Implementation of the Freetype / HarfBuzz Font rendering.
 *
 * Copyright (c) 2014 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include <string>

namespace gltext {

/**
 * @brief Private Implementation of the Freetype / HarfBuzz Font rendering.
 *
 *  Using an "opaque pointer" to a private implementation (pimpl) is the way to protect the client application
 * from the inclusion of Freetype and HarfBuzz libraries.
 */
class FontPimpl {
public:
    FontPimpl(const char* apFontFilename, unsigned int aPointSize, unsigned int aCacheSize);
    ~FontPimpl();

private:
    std::string     mFontFilename;
    unsigned int    mPointSize;
    unsigned int    mCacheSize;
};

} // namespace gltext
