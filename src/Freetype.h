/**
 * @file    Freetype.h
 * @brief   Singleton to interface the Freetype library.
 *
 * Copyright (c) 2014 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include <hb-ft.h>  // HarfBuzz Freetype interface

namespace gltext {

/**
 * @brief Singleton to interface the Freetype library
 */
class Freetype {
public:
    /**
     * @brief Initialize the Freetype library.
     */
    Freetype();

    /**
     * @brief Free the Freetype library.
     */
    ~Freetype();

    /**
     * @brief Access instance of the singleton to the Freetype library
     *
     * @return The unique static instance of this interface
     */
    static inline Freetype& getInstance() {
        static Freetype _freetypeInstance; ///< Static instance of the singleton
        return _freetypeInstance;
    }

private:
    FT_Library  mFreetypeLibrary;   ///< Handle to the Freetype Library
};

} // namespace gltext
