/**
 * @file    Freetype.h
 * @brief   Singleton to interface the Freetype library.
 *
 * Copyright (c) 2014 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include <hb-ft.h>  // HarfBuzz Freetype interface

namespace gltext {

/**
 * @brief Singleton to interface the Freetype library
 *
 *  Using a singleton is simple but has drawbacks; it must not be used from multiple threads simultaneously.
 * So we have to do all Font loading, rendering and caching in one thread, and use only the Text result
 * in other threads.
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
     * @brief Access the Freetype library.
     */
    inline const FT_Library& getLibrary() const;

    /**
     * @brief Access instance of the singleton to the Freetype library
     *
     * @return The unique static instance of this interface
     */
    static inline Freetype& getInstance() {
        /// Static instance of the singleton
        static Freetype _freetypeInstance;
        return _freetypeInstance;
    }

private:
    FT_Library  mLibrary;   ///< Handle to the Freetype Library
};

// Access the handle to the Freetype library.
inline const FT_Library& Freetype::getLibrary() const {
    return mLibrary;
}

} // namespace gltext
