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

#include <gltext/Text.h>

#include <string>

#include <hb-ft.h>  // HarfBuzz Freetype interface

#include "glload.hpp"   // OpenGL types & function pointers

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
     * @param[in] aPixelSize        Vertical size of the font in pixel
     * @param[in] aCacheSize        Minimum number of characters to allocate into the cache (use a square value).
     */
    FontImpl(const char* apPathFilename, unsigned int aPixelSize, unsigned int aCacheSize);
    /**
     * @brief Cleanup all Freetype and OpenGL ressources when the last reference is destroyed.
     */
    ~FontImpl();

    /**
     * @brief Pre-render and cache the glyphs representing the given characters, to speed-up future rendering.
     *
     * @see Font::Font() for detailed explanation
     *
     * @param[in] aCharacters   UTF-8 encoded string of characters to pre-render and add to the cache.
     */
    void cache(const std::string& aCharacters);

    /**
     * @brief Render the given string of characters (or use existing cached glyphs) and put it on a VAO.
     *
     * @see Font::Font() for detailed explanation
     *
     * @param[in] aCharacters   UTF-8 encoded string of characters to pre-render and add to the cache.
     * @param[in] aFontImplPtr  Shared pointer to this Private Implementation.
     *
     * @return Encapsulation of the constant text rendered with Freetype, ready to be drawn with OpenGL.
     */
    Text render(const std::string& aCharacters, const std::shared_ptr<const FontImpl>& aFontImplPtr);

    /**
     * @brief Draw the cache texture for debug purpose.
     *
     * @param[in] aX    X coordinate of where to start drawing the texture.
     * @param[in] aY    Y coordinate of where to start drawing the texture.
     * @param[in] aW    Width of the area to draw the texture.
     * @param[in] aH    Height of the area to draw the texture.
     */
    void drawCache(float aX, float aY, float aW, float aH);

private:
    /**
     * @brief Pre-render and cache the glyph representing the given unicode Unicode codepoint.
     *
     * @param[in] codepoint Unicode character codepoint.
     */
    unsigned int cache(FT_UInt codepoint);

private:
    std::string     mPathFilename;  ///< Path to the OpenType font file to open with Freetype.
    unsigned int    mCacheSize;     ///< Maximum number of characters to allocate into the cache.
    unsigned int    mCacheWidth;    ///< Horizontal size of the cache texture.
    unsigned int    mCacheHeigth;   ///< Vertical size  of the cache texture.
    unsigned int    mCacheNbGlyps;   ///< Maximum number of characters to allocate into the cache.
    unsigned int    mCacheFreeSlotX; ///< X coordinate of next free slot on the cache texture.
    unsigned int    mCacheFreeSlotY; ///< Y coordinate of next free slot on the cache texture.
    unsigned int    mPixelWidth;    ///< Maximal horizontal size of a character in pixel.
    unsigned int    mPixelHeight;   ///< Vertical size of a character in pixel.

    FT_Face         mFace;          ///< Handle to the typographic face object (given typeface/font, in a given style).
    hb_font_t*      mFont;          ///< Harfbuzz pointer to the freetype font, for text shaping

    GLuint mCacheTexture;
    // For cache debug draw
    GLuint mCacheVAO;
    GLuint mCacheVBO;
    GLuint mCacheIBO;
    // TODO move these into TextImpl class
    GLuint mTextVAO;
    GLuint mTextVBO;
    GLuint mTextIBO;
};

} // namespace gltext
