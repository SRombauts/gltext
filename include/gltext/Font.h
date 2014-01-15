/**
 * @file    Font.h
 * @brief   C++ Library to use freetype under OpenGL.
 *
 * Copyright (c) 2014 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include <memory>   // for std::shared_ptr

namespace gltext {

/**
 * @brief Private Implementation of the Freetype / HarfBuzz Font rendering
 */
class FontPimpl;

/**
 * @brief Manage the Freetype rendering of a font, and cache the resulting glyphs.
 *
 *  Default Copy Constructor and Assignment Operator only copy the shared pointeur,
 * which give a new reference to the Font instance, enabling easy sharing of a Font implementation accross application.
 */
class Font {
public:
    /**
     * @brief Ask Freetype to open a Font file and initialize it with the given size
     *
     *  An OpenGL texture is created, that will serve later to render glyph of the font on request, and as a cache.
     * This cache texture has a fixed size and will overflow if to many different characters are rendered.
     * This texture size is calculated based on the font size and the minimum number of characters of the cache.
     * It uses the the best "Power Of Two" texture size able to handle the requested cache size. Thus, if the resulting
     * texture have some extra space, the resulting cache size is expanded to reflect the real available space.
     * For accurate results, ask for a square cache size.
     *
     *  std::exception can be thrown in case of error during this process,
     * thus the new Font object will not be created, and any element will be cleaned accordingly.
     *
     * @param[in] apFontFilename    Path to a TrueType font file to open with Freetype.
     * @param[in] aPointSize        Vertical size of the font in pixel
     * @param[in] aCacheSize        Minimum number of characters to allocate into the cache (use a square value).
     */
    Font(const char* apFontFilename, unsigned int aPointSize = 16, unsigned int aCacheSize = 100);

    /**
     * @brief Cleanup all Freetype and OpenGL ressources when the last reference is destroyed.
     */
    ~Font();

    // NOTE : see #Font class header about Copy & Assignment

private:
    /**
     * @brief Private Implementation of the Freetype / HarfBuzz Font rendering
     *
     *  Using an "opaque pointer" to a private implementation (pimpl) is the way to protect the client application
     * from the inclusion of Freetype and HarfBuzz libraries.
     *  Using a reference-counted std::shared_ptr enable cheap sharing of a Font implementation;
     * copy only add a reference to the instance shared instance, and garanty that its ressources
     * will only be destroyed when the last Font reference is destroyed
     */
    std::shared_ptr<FontPimpl>  mImplPtr;
};

} // namespace gltext
