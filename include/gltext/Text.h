/**
 * @file    Text.h
 * @brief   Encapsulate a static/constant text rendered with Freetype, ready to be drawn with OpenGL.
 *
 * Copyright (c) 2014 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include <memory>   // for std::shared_ptr

namespace gltext {

/**
 * @brief Private Implementation of the rendered text.
 */
class TextImpl;
class FontImpl;

/**
 * @brief Encapsulate a static/constant text rendered with Freetype, ready to be drawn with OpenGL.
 *
 *  A Text instance is obtained from the Font::render() API.
 * It can be used from another thread, has it does not access the (monothreaded) Freetype library.
 *
 *  Default Copy Constructor and Assignment Operator only copy the shared pointeur,
 * which give a new reference to the Text instance, enabling easy sharing of a Text implementation across application.
 */
class Text {
public:
    /**
     * @brief Encapsulate the rendered text returned by Font::render(), ready to be drawn with OpenGL.
     *
     *  An OpenGL Vertex Array Object (VAO) is created and initialized with states needed to draw the text.
     * An OpenGL Index Buffer Object (IBO) is created to index the glyphs to be rendered.
     *
     * @param[in] aFontImplPtr  Shared pointer to the Font implementation from which this Text is build.
     */
    explicit Text(const std::shared_ptr<const FontImpl>& aFontImplPtr);

    /**
     * @brief Cleanup all Freetype and OpenGL ressources when the last reference is destroyed.
     *
     *  When the last reference to a Text object is destroyed, the TextImpl releases all OpenGL ressources
     * associated to it (the VAO and IBO).
     *  When the last reference to its upstream Font object is destroyed, the FontImpl also releases
     * the Texture and all Freetype ressources owned by the Font.
     */
    ~Text();

    // NOTE : see #Text class header about Copy & Assignment

    /**
     * @brief Initialize the 3D position where to start to draw the text.
     *
     *  Position must be expressed in a the appropriate (Right or Left) Handed coordinate system,
     * depending on the current program used.
     *  Text will be drawn at constant Y and Z coordinates, in forward X direction.
     *
     * @param[in] aX    X coordinate of where to start drawing the text.
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
     * @brief Private Implementation of the rendered text.
     */
    std::shared_ptr<TextImpl>   mImplPtr;
};

} // namespace gltext
