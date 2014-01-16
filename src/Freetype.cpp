/**
 * @file    Freetype.cpp
 * @brief   Singleton to interface the Freetype library.
 *
 * Copyright (c) 2014 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include "Freetype.h"   // NOLINT TODO

#include <hb-ft.h>      // HarfBuzz Freetype interface
#include <stdexcept>
#include <string>

namespace gltext {

// Initialize the Freetype library.
Freetype::Freetype() {
    FT_Init_FreeType(&mLibrary);
}

// Free the Freetype library.
Freetype::~Freetype() {
    FT_Done_FreeType(mLibrary);
}

} // namespace gltext
