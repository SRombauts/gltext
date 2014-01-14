/**
 * @file    gltext.cpp
 * @brief   C++ Library to use freetype under OpenGL.
 *
 * Copyright (c) 2014 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include <gltext/gltext.h>
#include <stdexcept>
#include <string>

namespace gltext {

    /**
     * @brief gltext Exception derived from std::exception
     */
    class Exception : public std::runtime_error {
    public:
        Exception(const std::string& aWhat) : std::runtime_error(aWhat) {}
    };

    Font::Font(const char* apFontFilename, unsigned int aPointSize, unsigned int aCacheSizeX /* = 256 */, int aCacheSizeY /* = 256 */) {
        Freetype& freetype = Freetype::getInstance();
    }
    Font::~Font() {
    }


} // namespace gltext
