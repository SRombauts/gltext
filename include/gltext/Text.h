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
class Font;

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
     * @param[in] aFont Reference to the Font instance from which this Text is build.
     */
    explicit Text(const Font& aFont);

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
     * @brief Ask OpenGL to draw the pre-rendered static text, using the current binded program.
     */
    void draw();

private:
    /**
     * @brief Private Implementation of the rendered text.
     */
    std::shared_ptr<TextImpl>   mImplPtr;
};

} // namespace gltext
