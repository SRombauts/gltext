/**
 * @file    Text.h
 * @brief   Encapsulate a (constant) a text rendered with Freetype, ready to draw with OpenGL.
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
 * @brief Encapsulate a (constant) text rendered with Freetype, ready to draw with OpenGL.
 *
 *  A Text instance is obtained from the Font::render() API.
 * It can then be shared (referenced-counted) with no cost.
 * It can be used from another thread, has it does not access the (monothreaded) Freetype library.
 */
class Text {
public:
    /**
     * @brief TOOD
     */
    explicit Text(const Font& aFont);

    /**
     * @brief Cleanup all Freetype and OpenGL ressources when the last reference is destroyed.
     */
    ~Text();

    // NOTE : see #Text class header about Copy & Assignment

private:
    /**
     * @brief Private Implementation of the rendered text.
     */
    std::shared_ptr<TextImpl>   mImplPtr;
};

} // namespace gltext
