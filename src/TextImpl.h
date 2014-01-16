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

#include <memory>   // for std::shared_ptr

namespace gltext {

class Font;
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
     * @param[in] aFont Reference to the Font instance from which this Text is build.
     *
     * @see Text::Text() for detailed explanation
     */
    explicit TextImpl(const Font& aFont);
    /**
     * @brief Cleanup
     */
    ~TextImpl();

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
};

} // namespace gltext
