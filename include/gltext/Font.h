/**
 * @file    Font.h
 * @brief   C++ Library to use freetype under OpenGL.
 *
 * Copyright (c) 2014 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include <memory>   // for std::shared_ptr

#include <gltext/Text.h>

namespace gltext {

/**
 * @brief Private Implementation of the Freetype / HarfBuzz Font rendering
 */
class FontImpl;

/**
 * @brief Manage the Freetype rendering of a font, and cache the resulting glyphs.
 *
 *  The Font public interface must never be used from multiple threads simultaneously.
 * All Font loading, rendering and caching shall be done in only one thread (a background loading thread for instance),
 * and then the Text result can be used in other threads if needed (the main rendering thread).
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
     * @param[in] apPathFilename    Path to the OpenType font file to open with Freetype.
     * @param[in] aPointSize        Vertical size of the font in pixel
     * @param[in] aCacheSize        Minimum number of characters to allocate into the cache (use a square value).
     */
    Font(const char* apPathFilename, unsigned int aPointSize = 16, unsigned int aCacheSize = 100);

    /**
     * @brief Cleanup all Freetype and OpenGL ressources when the last reference is destroyed.
     */
    ~Font();

    // NOTE : see #Font class header about Copy & Assignment

    // TODO SRombauts
    void cache(const char* apCharacters);
    Text render(const char* apCharacters);

    inline const std::shared_ptr<const FontImpl> getImplPtr() const;

private:
    /**
     * @brief Private Implementation of the Freetype / HarfBuzz Font rendering
     *
     *  Using an "opaque pointer" to a private implementation (PIMPL idiom) is the way to protect the client application
     * from the inclusion of Freetype and HarfBuzz libraries.
     *  Using a reference-counted std::shared_ptr enable cheap sharing of a Font implementation;
     * copy only add a reference to the instance shared instance, and guaranty that its ressources
     * will only be destroyed when the last Font reference is destroyed
     */
    std::shared_ptr<FontImpl>   mImplPtr;
};

inline const std::shared_ptr<const FontImpl> Font::getImplPtr() const {
    return mImplPtr;
}

} // namespace gltext