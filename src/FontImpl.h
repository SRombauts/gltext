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
#include <vector>

#include <hb-ft.h>      // HarfBuzz Freetype interface

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
    // TODO : replace by a getter for mCacheTexture ?
    friend class TextImpl;

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
    FontImpl(const char* apPathFilename, size_t aPixelSize, size_t aCacheSize);
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
    size_t cache(FT_UInt codepoint);

private:
    /// Data of one of four glyph vertex
    struct GlyphVertex {
        GLfloat x;  ///< Vertex x coordinate
        GLfloat y;  ///< Vertex y coordinate
        GLfloat s;  ///< Texture s (x) coordinate
        GLfloat t;  ///< Texture t (y) coordinate
    };

    /// Vertex and texture coordinates of the 4 corners of a glyph (that is, a quad, or 2 triangles)
    struct GlyphVerticies {
        GlyphVertex bl; ///< Vertex data of the Bottom Left corner
        GlyphVertex br; ///< Vertex data of the Bottom Right corner
        GlyphVertex tl; ///< Vertex data of the Top Left corner
        GlyphVertex tr; ///< Vertex data of the Top Right corner
    };

    /// Corresponding 6 indices used to described the 2 triangles that compose a glyph
    struct GlyphIndices {
        GLushort bl1;   ///< Index 0 of the vertex in the Bottom Left corner (first triangle)
        GLushort br1;   ///< Index 1 of the vertex in the Bottom Right corner (first triangle)
        GLushort tl1;   ///< Index 2 of the vertex in the Top Left corner (first triangle)
        GLushort br2;   ///< Index 1 of the vertex in the Bottom Right corner (second triangle)
        GLushort tr2;   ///< Index 2 of the vertex in the Top Right corner (second triangle)
        GLushort tl2;   ///< Index 3 of the vertex in the Top Left corner (second triangle)
    };

    /// Association of codepoint/idx of the cached glyphs
    typedef std::map<FT_UInt, size_t>   GlyphIdxMap;
    /// Vector of cached vertex and texture coordinates for each glyph
    typedef std::vector<GlyphVerticies> GlyphVertVector;
    /// Vector of cached indices for each glyph
    typedef std::vector<GlyphIndices>   GlyphIdxVector;

private:
    std::string     mPathFilename;      ///< Path to the OpenType font file to open with Freetype.
    size_t          mCacheWidth;        ///< Horizontal size of the cache texture.
    size_t          mCacheHeight;       ///< Vertical size  of the cache texture.
    size_t          mCacheLineHeight;   ///< Vertical size of the current line of character cache in pixel.
    size_t          mCacheFreeSlotX;    ///< X coordinate of next free slot on the cache texture.
    size_t          mCacheFreeSlotY;    ///< Y coordinate of next free slot on the cache texture.
    GlyphIdxMap     mCacheGlyphIdxMap;  ///< Association of codepoint/idx of the cached glyphs
    GlyphVertVector mCacheGlyphVertList; ///< List of cached data (vertex and texture coordinates, and indices)

    FT_Face         mFace;              ///< Handle to typographic face object (given typeface/font, in a given style).
    hb_font_t*      mFont;              ///< Harfbuzz pointer to the freetype font, for text shaping

    GLuint mCacheTexture;               ///< 2D Texture used to cache the rendered glyphs, shared between multiple Text
    // For cache debug draw
    GLuint mCacheVAO;                   ///< Vertex Array Object used only for debug draw of the cache
    GLuint mCacheVBO;                   ///< Vertex Buffer Object used only for debug draw of the cache
    GLuint mCacheIBO;                   ///< Index Buffer Object used only for debug draw of the cache
};

} // namespace gltext
