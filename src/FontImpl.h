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
#include <map>

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
     * @param[in] aOffsetX  X offset of where to start drawing the texture.
     * @param[in] aOffsetY  Y offset of where to start drawing the texture.
     * @param[in] aScaleX   Scale the width of the area to draw the texture.
     * @param[in] aScaleY   Scale the height of the area to draw the texture.
     *
     * @todo use glm::vec2
     * @todo add color
     */
    void drawCache(float aOffsetX, float aOffsetY, float aScaleX, float aScaleY);

private:
    /**
     * @brief Pre-render and cache the glyph representing the given unicode Unicode codepoint.
     *
     * @param[in] codepoint Unicode character codepoint.
     *
     * @return Index of the cached glyph
     */
    unsigned int cache(FT_UInt codepoint);

private:
    /// Association of codepoint/idx of the cached glyphs
    typedef std::map<FT_UInt, unsigned long>    GlyphIdxMap;

private:
    std::string     mPathFilename;      ///< Path to the OpenType font file to open with Freetype.
    unsigned int    mCacheWidth;        ///< Horizontal size of the cache texture.
    unsigned int    mCacheHeight;       ///< Vertical size  of the cache texture.
    unsigned int    mCacheLineHeight;   ///< Vertical size of the current line of character cache in pixel.
    unsigned int    mCacheFreeSlotX;    ///< X coordinate of next free slot on the cache texture.
    unsigned int    mCacheFreeSlotY;    ///< Y coordinate of next free slot on the cache texture.
    GlyphIdxMap     mCacheGlyphIdxMap;  ///< Association of codepoint/idx of the cached glyphs

    FT_Face         mFace;              ///< Handle to typographic face object (given typeface/font, in a given style).
    hb_font_t*      mFont;              ///< Harfbuzz pointer to the freetype font, for text shaping

    GLuint mCacheTexture;               ///< 2D Texture used to cache the rendered glyphs, shared between multiple Text
    // For cache debug draw
    GLuint mCacheVAO;                   ///< Vertex Array Object used only for debug draw of the cache
    GLuint mCacheVBO;                   ///< Vertex Buffer Object used only for debug draw of the cache
    GLuint mCacheIBO;                   ///< Index Buffer Object used only for debug draw of the cache
    // TODO move these into TextImpl class
    GLuint mTextVAO;                    ///< Vertex Array Object used to render a text
    GLuint mTextVBO;                    ///< Vertex Buffer Object used to render a text
    GLuint mTextIBO;                    ///< Index Buffer Object used to render a text
};

} // namespace gltext
