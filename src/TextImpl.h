/**
 * @file    TextImpl.h
 * @brief   Private Implementation of a static/constant text rendered with Freetype, ready to be drawn with OpenGL.
 *
 * Copyright (c) 2014 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include <memory>       // for std::shared_ptr

#include "glload.hpp"   // OpenGL types & function pointers

namespace gltext {

class FontImpl;

/**
 * @brief Private Implementation of a static/constant text rendered with Freetype, ready to be drawn with OpenGL.
 *
 * @see the Text class for detailed explanation
 *
 *  Using an "opaque pointer" to a private implementation (pimpl) is the way to protect the client application
 * from the inclusion of Freetype and HarfBuzz libraries.
 */
class TextImpl {
public:
    /**
     * @brief Encapsulate a text rendered
     *
     * @param[in] aFontImplPtr  Shared pointer to the Font implementation from which this Text is build.
     *
     * @see Text::Text() for detailed explanation
     */
    explicit TextImpl(const std::shared_ptr<const FontImpl>& aFontImplPtr);
    /**
     * @brief Cleanup
     */
    ~TextImpl();

    /**
     * @brief Initialize the 3D position where to start to draw the text.
     *
     *  Position must be expressed in a the appropriate Right Handed Coordinate system
     * (or depending on the current program used), where X is
     *  Text will be drawn at constant Y and Z coordinates, in forward X direction.
     *
     * @param[in] aX    X horizontal coordinate of where to start drawing the text.
     * @param[in] aY    Y vertical coordinate of where to start drawing the text.
     * @param[in] aZ    Z depth coordinate of where to start drawing the text.
     */
    void setPosition(float aX, float aY, float aZ);

    /**
     * @brief Ask OpenGL to draw the pre-rendered static text, using the current binded program, at current position.
     *
     * Text is drawn at constant Y and Z coordinates, in forward X direction, starting from setPosition().
     */
    void draw();

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
    const std::shared_ptr<const FontImpl> mFontImplPtr;

    size_t mTextSize;                   ///< Size of text (number of unicode codepoint, number of glyphs in GL buffers)

    GLuint mTextVAO;                    ///< Vertex Array Object used to render the text
    GLuint mTextVBO;                    ///< Vertex Buffer Object used to render the text
    GLuint mTextIBO;                    ///< Index Buffer Object used to render the text
};

} // namespace gltext
