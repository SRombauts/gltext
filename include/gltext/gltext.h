/**
 * @file    gltext.h
 * @brief   C++ Library to use freetype under OpenGL.
 *
 * Copyright (c) 2014 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

// TODO this shall not be visible to the client application => private class or Pimpl
#include <hb-ft.h>


namespace gltext {

/**
 * @brief Singleton to interface the Freetype library
 */
// TODO move into a dedicated private class
class Freetype {
public:
    Freetype() {
        FT_Init_FreeType(&mFreetypeLibrary);
    }
    ~Freetype() {
        FT_Done_FreeType(mFreetypeLibrary);
    }

    static Freetype& getInstance() {
        static Freetype _freetypeInstance;
        return _freetypeInstance;
    }
private:
    FT_Library  mFreetypeLibrary;   ///< Handle to the Freetype Library
};

/**
 * @brief Manage the Freetype rendering of a font, and cache the resulting glyphs
 */
class Font {
public:
    Font(const char* apFontFilename, unsigned int aPointSize, unsigned int aCacheSizeX = 256, int aCacheSizeY = 256);
    ~Font();

private:
    /// @{ Forbid copy and
    Font(const Font&);
    Font& operator=(const Font&);

};

} // namespace gltext
