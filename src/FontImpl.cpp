/**
 * @file    FontImpl.cpp
 * @brief   Private Implementation of the Freetype / HarfBuzz Font rendering.
 *
 * Copyright (c) 2014 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include "FontImpl.h"   // NOLINT TODO
#include "Freetype.h"   // NOLINT TODO
#include "Exception.h"  // NOLINT TODO

#include <stdexcept>
#include <cassert>
#include <cmath>


/**
 * @brief Data of each gyph vertex
 */
struct GlyphVertex {
    GLfloat x;  ///< Vertex x coordinate
    GLfloat y;  ///< Vertex y coordinate
    GLfloat s;  ///< Texture s coordinate x
    GLfloat t;  ///< Texture t coordinate y
};

#define GLYPH_VERT_SIZE (sizeof(GlyphVertex))
#define GLYPH_IDX_SIZE (6*sizeof(GLushort))

static const char* shader_vert =
"#version 130\n"
"\n"
"in vec2 vert;\n"
"in vec2 texCoord;\n"
"smooth out vec2 smoothTexCoord;\n"
"\n"
"uniform ivec2 scale;\n"
"uniform ivec2 position;\n"
"\n"
"void main() {\n"
"    smoothTexCoord = texCoord;\n"
"    gl_Position = vec4((vert+vec2(position))/vec2(scale) * 2.0 - 1.0, 0.0, 1.0);\n"
"}\n";

static const char* shader_frag =
"#version 130\n"
"\n"
"smooth in vec2 smoothTexCoord;\n"
"out vec2 outputColor;\n"
"\n"
"uniform sampler2D tex;\n"
"uniform vec3 color;\n"
"\n"
"void main() {\n"
"    float val = texture(tex, smoothTexCoord).r;\n"
"    outputColor = vec4(color*val, val);\n"
"}\n";


// TODO Put this in a Program class
// TODO rename these variables
GLuint fs;
GLuint vs;
GLuint prog;
GLuint v; // vertex position
GLuint t; // texture coordinate
GLuint scale_loc;
GLuint pos_loc;
GLuint col_loc;


class Loader {
public:
    Loader() {
        initGlPointers();
        fs = glCreateShader(GL_FRAGMENT_SHADER);
        vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(fs, 1, &shader_frag, 0);
        glShaderSource(vs, 1, &shader_vert, 0);
        glCompileShader(fs);
        glCompileShader(vs);
        prog = glCreateProgram();
        glAttachShader(prog, fs);
        glAttachShader(prog, vs);
        glLinkProgram(prog);
        glUseProgram(prog);
        glUniform1i(glGetUniformLocation(prog, "tex"), 0);
        v = glGetAttribLocation(prog, "vert");
        t = glGetAttribLocation(prog, "texCoord");
        scale_loc = glGetUniformLocation(prog, "scale");
        pos_loc = glGetUniformLocation(prog, "position");
        col_loc = glGetUniformLocation(prog, "color");
    }

    static Loader& getInstance() {
        static Loader loader;
        return loader;
    }
};



namespace gltext {

// Ask Freetype to open a Font file and initialize it with the given size
FontImpl::FontImpl(const char* apPathFilename, unsigned int aPixelSize, unsigned int aCacheSize) :
    mPathFilename(apPathFilename),
    mCacheSize(aCacheSize) {
    Freetype& freetype = Freetype::getInstance();
    Loader& loader = Loader::getInstance();
    // Load the font from file
    FT_Error error = FT_New_Face(freetype.getLibrary(), mPathFilename.c_str(), 0, &mFace);
    if (error) {
        throw Exception("FT_New_Face error");
    }
    // Set the vertical pixel size
    error = FT_Set_Pixel_Sizes(mFace, 0, aPixelSize);
    if (error) {
        FT_Done_Face(mFace);
        throw Exception("FT_Set_Pixel_Sizes error");
    }
    // Open the font with harfbuzz for text shaping
    mFont = hb_ft_font_create(mFace, 0);

    // Calculate actual font size
    mPixelWidth = static_cast<unsigned int>(ceil((mFace->max_advance_width * mFace->size->metrics.y_ppem) /
                                                   static_cast<float>(mFace->units_per_EM)));
    mPixelHeight  = static_cast<unsigned int>(ceil((mFace->height * mFace->size->metrics.y_ppem) /
                                                   static_cast<float>(mFace->units_per_EM)));

    // TODO Calculate appropriate texture cache dimension
    mCacheWidth = mPixelWidth;
    mCacheHeigth = mPixelHeight;

    glGenVertexArrays(1, &mTextVAO);
    glGenBuffers(1, &mCacheVBO);
    glGenBuffers(1, &mTextIBO);
    glBindVertexArray(mTextVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mCacheVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mTextIBO);
    glBufferData(GL_ARRAY_BUFFER, GLYPH_VERT_SIZE*mCacheSize, NULL, GL_DYNAMIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLYPH_IDX_SIZE*mCacheSize, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(v);
    glEnableVertexAttribArray(t);
    glVertexAttribPointer(v, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);
    glVertexAttribPointer(t, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), reinterpret_cast<GLvoid*>(2*sizeof(float)));

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &mCacheTexture);
    glBindTexture(GL_TEXTURE_2D, mCacheTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, mCacheWidth, mCacheHeigth, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

// Cleanup all Freetype and OpenGL ressources when the last reference is destroyed.
FontImpl::~FontImpl() {
    hb_font_destroy(mFont);
    glDeleteTextures(1, &mCacheTexture);
    glDeleteBuffers(1, &mCacheVBO);
    glDeleteBuffers(1, &mTextIBO);
    glDeleteVertexArrays(1, &mTextVAO);
}

// Pre-render and cache the glyphs representing the given characters, to speed-up future rendering.
void FontImpl::cache(const char* apCharacters) {
}

// Render the given string of characters (or use existing cached glyphs) and put it on a VAO/VBO.
Text FontImpl::render(const char* apCharacters, const std::shared_ptr<const FontImpl>& aFontImplPtr) {
    return Text(aFontImplPtr);
}

// Pre-render and cache the glyph representing the given unicode Unicode codepoint.
unsigned int FontImpl::cache(FT_UInt codepoint) {
    unsigned int idxGlyph = 0;
    // TODO
    return idxGlyph;
}

} // namespace gltext
