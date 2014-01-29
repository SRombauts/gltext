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
#include <string>
#include <iostream>     // NOLINT TODO


/// Source of the vertex shader used to scale the glyphs vertices
static const char* _vertexShaderSource =
"#version 330\n"
"\n"
"// Attributes (input data streams ; 2D vertex position and texture coordinates)\n"
"layout(location = 0) in vec2 position;\n"
"layout(location = 1) in vec2 texCoord;\n"
"\n"
"// Output data stream (smoothed interpolated texture 2D coordinates)\n"
"smooth out vec2 smoothTexCoord;\n"
"\n"
"// Uniform variables\n"
"uniform vec2 scale;\n"
"uniform vec2 offset;\n"
"\n"
"void main() {\n"
"    // positions are scaled and offseted\n"
"    gl_Position = vec4((position + offset) * scale, 0.0f, 1.0f);\n"
"    smoothTexCoord = texCoord;\n"
"}\n";

/// Source of the fragment shader used to draw the glyphs using the cache texture
static const char* _fragmentShaderSource =
"#version 330\n"
"\n"
"smooth in vec2 smoothTexCoord;\n"
"\n"
"out vec4 outputColor;\n"
"\n"
"uniform sampler2D textureCache;\n"
"uniform vec3 color;\n"
"\n"
"void main() {\n"
"    // Texture gives only grayed ('black & white') intensity onto the 'GL_RED' color component\n"
"    float textureIntensity = texture(textureCache, smoothTexCoord).r;\n"
"    // Texture intensity is composed with pen color, and also drives the alpha component\n"
"    outputColor = vec4(color*textureIntensity, textureIntensity);\n"
"}\n";

/**
 * @brief Check for any previous OpenGL error. Use with the GL_CHECK() macro
 *
 * @param[in] apFile    File where the check is done
 * @param[in] aLine     Line where the check is called
 */
void checkOpenGlError(const char* apFile, int aLine) {
    GLenum error = glGetError();
    if (GL_NO_ERROR != error) {
        const char* pMsg = NULL;
        switch (error) {
            case GL_INVALID_ENUM:                   pMsg = "GL_INVALID_ENUM";                  break;
            case GL_INVALID_VALUE:                  pMsg = "GL_INVALID_VALUE";                 break;
            case GL_INVALID_OPERATION:              pMsg = "GL_INVALID_OPERATION";             break;
            case GL_OUT_OF_MEMORY:                  pMsg = "GL_OUT_OF_MEMORY";                 break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:  pMsg = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
            case GL_STACK_UNDERFLOW:                pMsg = "GL_STACK_UNDERFLOW";               break;
            case GL_STACK_OVERFLOW:                 pMsg = "GL_STACK_OVERFLOW";                break;
        }
        std::cerr << apFile << ":" << aLine << ": " << pMsg << std::endl;
    }
}
/// Macro checking for any previous OpenGL error.
#define GL_CHECK()  checkOpenGlError(__FILE__, __LINE__)


/**
 * @brief Compile the shaders and link the program
 */
class Program {
public:
    Program() :
        mTextureUnit(0) {

        std::cout << "Program::Program()\n";

        // Load OpenGL 3 function pointers
        glload::initGlPointers();

        // Compile shader and link program
        GLuint mVertexShader = compileShader(GL_VERTEX_SHADER, _vertexShaderSource);
        GLuint mFragmentShader = compileShader(GL_FRAGMENT_SHADER, _fragmentShaderSource);
        mProgram = linkProgram(mVertexShader, mFragmentShader);

        // Fetch Attribute (input data streams) and Uniform (variables) locations (ids)
        glUseProgram(mProgram);
        mVertexPositionAttrib = glGetAttribLocation(mProgram, "position");
        mVertexTextureCoordAttrib = glGetAttribLocation(mProgram, "texCoord");
        mScaleUnif = glGetUniformLocation(mProgram, "scale");
        mOffsetUnif = glGetUniformLocation(mProgram, "offset");
        mColorUnif = glGetUniformLocation(mProgram, "color");
        GLuint textureCacheUnif = glGetUniformLocation(mProgram, "textureCache");
        glUniform1i(textureCacheUnif, mTextureUnit);
        GL_CHECK();
    }

    ~Program() {
        // glDeleteProgram(mProgram);
    }

    /**
     * @brief Compile a shader of the given type from the source code provided as a string
     *
     * @param[in] aShaderType   Type of shader to be compiled
     * @param[in] aShaderSource Shader source code to compile
     *
     * @return Id of the created shader object
     *
     * @throw a std::exception in case of error (std::runtime_error)
     */
    GLuint compileShader(GLenum aShaderType, const std::string& aShaderSource) const {
        const char *shaderSource = aShaderSource.c_str();

        std::cout << "Program::compileShader(" << aShaderType << ")\n";

        // Create a shader, give it the source code, and compile it
        GLuint shader = glCreateShader(aShaderType);
        glShaderSource(shader, 1, &shaderSource, NULL);
        glCompileShader(shader);

        // Check shader status, and get error message if a problem occured
        GLint status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (GL_FALSE == status) {
            GLint infoLogLength;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

            GLchar* pInfoLog = new GLchar[infoLogLength + 1];
            glGetShaderInfoLog(shader, infoLogLength, NULL, pInfoLog);
            std::string errorMsg = pInfoLog;
            delete[] pInfoLog;
            pInfoLog = NULL;

            throw std::runtime_error(errorMsg);
        }

        return shader;
    }

    /**
     * @brief Link the shaders into a program object
     *
     * @param[in] aVertexShader     Source text of the vertex shader
     * @param[in] aFragmentShader   Source text of the fragment shader
     *
     * @return Id of the created program object
     *
     * @throw a std::exception in case of error (std::runtime_error)
     */
    GLuint linkProgram(GLuint aVertexShader, GLuint aFragmentShader) const {
        // Create a program, attach shaders to it, and link the program
        GLuint program = glCreateProgram();
        glAttachShader(program, aVertexShader);
        glAttachShader(program, aFragmentShader);
        glLinkProgram(program);

        std::cout << "Program::linkProgram(" << aVertexShader << ", " << aFragmentShader << ")\n";

        // Check program status, and get error message if a problem occurred
        GLint status;
        glGetProgramiv(program, GL_LINK_STATUS, &status);
        if (GL_FALSE == status) {
            GLint infoLogLength;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

            GLchar* pInfoLog = new GLchar[infoLogLength + 1];
            glGetProgramInfoLog(program, infoLogLength, NULL, pInfoLog);
            std::string errorMsg = pInfoLog;
            delete[] pInfoLog;
            pInfoLog = NULL;

            throw std::runtime_error(errorMsg);
        }

        // Now, the intermediate compiled shader can be detached and deleted (the program contain them)
        glDetachShader(program, aVertexShader);
        glDeleteShader(aVertexShader);
        glDetachShader(program, aFragmentShader);
        glDeleteShader(aFragmentShader);

        return program;
    }

    /**
     * @brief Get instance of the singleton
     *
     * @return instance of the singleton
     */
    static Program& getInstance() {
        static Program loader;
        return loader;
    }

public:
    const GLuint mTextureUnit;          ///< id of the texture image unit (0)

    GLuint mProgram;                    ///< program linked of a vertex and a fragment shader
    GLuint mVertexPositionAttrib;       ///< vertex position
    GLuint mVertexTextureCoordAttrib;   ///< texture coordinate
    GLuint mScaleUnif;                  ///< uniform location of the "scale" variable
    GLuint mOffsetUnif;                 ///< uniform location of the "offset" variable
    GLuint mColorUnif;                  ///< uniform location of the "color" variable
};



namespace gltext {

// Ask Freetype to open a Font file and initialize it with the given size
FontImpl::FontImpl(const char* apPathFilename, unsigned int aPixelSize, unsigned int aCacheSize) :
    mPathFilename(apPathFilename) {
    Freetype& freetype = Freetype::getInstance();
    Program& program = Program::getInstance();
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
    unsigned long maxSlotWidth = static_cast<unsigned int>(
        ceil((mFace->max_advance_width * mFace->size->metrics.y_ppem) / static_cast<float>(mFace->units_per_EM)));
    unsigned long maxSlotHeight = static_cast<unsigned int>(
        ceil((mFace->height * mFace->size->metrics.y_ppem) / static_cast<float>(mFace->units_per_EM)));

    // TODO Calculate appropriate texture cache dimension from aCacheSize => use the Next Power Of Two (NPOT)
    mCacheWidth = 256;
    mCacheHeight = 256;
    mCacheLineHeight = 0;
    mCacheFreeSlotX = 0;
    mCacheFreeSlotY = 0;

    std::cout << "FontImpl::FontImpl(" << apPathFilename << ", " << aPixelSize << "): "
        << maxSlotWidth << "x" << maxSlotHeight
        << " (cache " << mCacheWidth << "x" << mCacheHeight << ")" << std::endl;

    // For cache debug draw
    // ^ y/t
    // |
    // 3 - 2
    // | / |
    // 0 - 1 -> x/s
    GlyphData glyphData;

    glyphData.vertices.bl.x = -1.0f;
    glyphData.vertices.bl.y = -1.0f;
    glyphData.vertices.bl.s = 0.0f;
    glyphData.vertices.bl.t = 1.0f;

    glyphData.vertices.br.x = 1.0f;
    glyphData.vertices.br.y = -1.0f;
    glyphData.vertices.br.s = 1.0f;
    glyphData.vertices.br.t = 1.0f;

    glyphData.vertices.tl.x = -1.0f;
    glyphData.vertices.tl.y = 1.0f;
    glyphData.vertices.tl.s = 0.0f;
    glyphData.vertices.tl.t = 0.0f;

    glyphData.vertices.tr.x = 1.0f;
    glyphData.vertices.tr.y = 1.0f;
    glyphData.vertices.tr.s = 1.0f;
    glyphData.vertices.tr.t = 0.0f;

    // TODO This should be filled automatically by an algorithme
    glyphData.indices.bl1 = 0;
    glyphData.indices.br1 = 1;
    glyphData.indices.tl1 = 2;
    glyphData.indices.br2 = 1;
    glyphData.indices.tl2 = 2;
    glyphData.indices.tr2 = 3;

    glGenVertexArrays(1, &mCacheVAO);
    glGenBuffers(1, &mCacheVBO);
    glGenBuffers(1, &mCacheIBO);
    glBindVertexArray(mCacheVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mCacheVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mCacheIBO);
    glBufferData(GL_ARRAY_BUFFER,         sizeof(glyphData.vertices), &(glyphData.vertices), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glyphData.indices),  &(glyphData.indices), GL_STATIC_DRAW);
    glEnableVertexAttribArray(program.mVertexPositionAttrib);
    glEnableVertexAttribArray(program.mVertexTextureCoordAttrib);
    glVertexAttribPointer(program.mVertexPositionAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(GlyphVertex), 0);
    glVertexAttribPointer(program.mVertexTextureCoordAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(GlyphVertex), reinterpret_cast<GLvoid*>(sizeof(GlyphVertex)/2)); // NOLINT
    GL_CHECK();

    // TODO move these into TextImpl class
    size_t TextLength = aCacheSize;
    glGenVertexArrays(1, &mTextVAO);
    glGenBuffers(1, &mTextVBO);
    glGenBuffers(1, &mTextIBO);
    glBindVertexArray(mTextVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mTextVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mTextIBO);
    glBufferData(GL_ARRAY_BUFFER, TextLength * sizeof(GlyphVerticies), NULL, GL_DYNAMIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, TextLength * sizeof(GlyphIndices), NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(program.mVertexPositionAttrib);
    glEnableVertexAttribArray(program.mVertexTextureCoordAttrib);
    glVertexAttribPointer(program.mVertexPositionAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(GlyphVertex), 0);
    glVertexAttribPointer(program.mVertexTextureCoordAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(GlyphVertex), reinterpret_cast<GLvoid*>(sizeof(GlyphVertex)/2)); // NOLINT
    GL_CHECK();

    // Cache texture
    glActiveTexture(GL_TEXTURE0 + program.mTextureUnit);
    glGenTextures(1, &mCacheTexture);
    glBindTexture(GL_TEXTURE_2D, mCacheTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, mCacheWidth, mCacheHeight, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    GL_CHECK();
}

// Cleanup all Freetype and OpenGL ressources when the last reference is destroyed.
FontImpl::~FontImpl() {
    hb_font_destroy(mFont);
    glDeleteTextures(1, &mCacheTexture);
    glDeleteVertexArrays(1, &mCacheVAO);
    glDeleteBuffers(1, &mCacheVBO);
    glDeleteBuffers(1, &mCacheIBO);
    glDeleteVertexArrays(1, &mTextVAO);
    glDeleteBuffers(1, &mTextVBO);
    glDeleteBuffers(1, &mTextIBO);
}

// Pre-render and cache the glyphs representing the given characters, to speed-up future rendering.
void FontImpl::cache(const std::string& aCharacters) {
    std::cout << "FontImpl::cache(" << aCharacters << ")\n";

    // Put the provided UTF-8 encoded characters into a Harfbuzz buffer
    hb_buffer_t* buffer = hb_buffer_create();
    hb_buffer_set_direction(buffer, HB_DIRECTION_LTR);
    hb_buffer_add_utf8(buffer, aCharacters.c_str(), aCharacters.size(), 0, aCharacters.size());
    // Ask Harfbuzz to shape the UTF-8 buffer
    hb_shape(mFont, buffer, NULL, 0);

    // Get buffer properties
    unsigned len = hb_buffer_get_length(buffer);
    hb_glyph_info_t* glyphs = hb_buffer_get_glyph_infos(buffer, 0);
//  hb_glyph_position_t* positions = hb_buffer_get_glyph_positions(buffer, 0);
//  positions[0].x_advance;
//  positions[0].x_offset;

    Program& program = Program::getInstance();
    glActiveTexture(GL_TEXTURE0 + program.mTextureUnit);
    glBindTexture(GL_TEXTURE_2D, mCacheTexture);
    // Affects the unpacking of pixel data from memory. Specifies the alignment requirements
    // for the start of each pixel row in memory; 1 for byte-alignment (See also GL_UNPACK_ROW_LENGTH).
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    GL_CHECK();

    // Iterate over the glyphs of the text
    for (unsigned i = 0; i < len; ++i) {
        // Is the glyph corresponding to the codepoint already in the cache ?
        GlyphIdxMap::const_iterator iGlyph = mCacheGlyphIdxMap.find(glyphs[i].codepoint);
        if (mCacheGlyphIdxMap.end() == iGlyph) {
            // if not, render and add the glyph into the cache
            /* unsigned int idxInCache = */ cache(glyphs[i].codepoint);
        }
    }
}

// Pre-render and cache the glyph representing the given unicode Unicode codepoint.
unsigned int FontImpl::cache(FT_UInt codepoint) {
    const unsigned int  idxInCache = mCacheGlyphIdxMap.size();
    const FT_Bitmap& bitmap = mFace->glyph->bitmap;

    // Load and render the glyph into the glyph slot of a the face object
    FT_Error error;
    error = FT_Load_Glyph(mFace, codepoint, FT_LOAD_RENDER);
    if (error) {
        throw Exception("FT_Load_Glyph");
    }

    // TODO verification and calculation

    // Does the free slot is wide enough to hold the new glyph ?
    if (mCacheWidth <= mCacheFreeSlotX + bitmap.width) {
        // else start with the next line
        mCacheFreeSlotY += mCacheLineHeight;
        mCacheFreeSlotX = 0;
        mCacheLineHeight = 0;
    }
    // Does the free slot is high enough to hold the new glyph ?
    if (mCacheHeight <= mCacheFreeSlotY + bitmap.rows) {
        throw Exception("Cache overflow");
    }

    // The pitch is positive when the bitmap has a `down' flow, and negative when it has an `up' flow.
    // In all cases, the pitch is an offset to add to a bitmap pointer in order to go down one row.
    int pitch = bitmap.pitch;
    if (pitch < 0) {
        pitch = -pitch;
    }
    // GL_UNPACK_ROW_LENGTH defines the number of pixels in a row
    glPixelStorei(GL_UNPACK_ROW_LENGTH, pitch);

    std::cout << "FontImpl::cache(" << codepoint << "):"
        << " width=" << bitmap.width
        << " rows=" << bitmap.rows
        << " pitch=" << pitch << " (" << bitmap.pitch << ")"
        << " advance.x=" << (mFace->glyph->advance.x >> 6)
        << "\n";

    // Load the newly rendered glyph into the texture cache
    glTexSubImage2D(
        GL_TEXTURE_2D, 0,
        mCacheFreeSlotX, mCacheFreeSlotY, bitmap.width, bitmap.rows,
        GL_RED, GL_UNSIGNED_BYTE, bitmap.buffer);
    GL_CHECK();

    // Increase height of the current row if needed
    if (bitmap.rows > static_cast<int>(mCacheLineHeight)) {
        mCacheLineHeight = bitmap.rows;
    }
    // Optimize the usage of cache texture; only use the space taken by the glyph
    mCacheFreeSlotX += bitmap.width;
    if (mCacheWidth <= mCacheFreeSlotX) {
        mCacheFreeSlotY += mCacheLineHeight;
        mCacheFreeSlotX = 0;
        mCacheLineHeight = 0;
    }

    // Add the idx of the glyph into the map
    mCacheGlyphIdxMap[codepoint] = idxInCache;

    // TODO Add 2 vectors for vertices and indices

    return idxInCache;
}

// Render the given string of characters (or use existing cached glyphs) and put it on a VAO/VBO.
Text FontImpl::render(const std::string& aCharacters, const std::shared_ptr<const FontImpl>& aFontImplPtr) {
    return Text(aFontImplPtr);
}

// Draw the cache texture for debug purpose.
void FontImpl::drawCache(float aOffsetX, float aOffsetY, float aScaleX, float aScaleY) {
    static bool bFirst = true;
    if (bFirst) {
        std::cout << "FontImpl::drawCache()\n";
        bFirst = false;
    }

    Program& program = Program::getInstance();
    glUseProgram(program.mProgram);

    glUniform2f(program.mOffsetUnif, aOffsetX, aOffsetY);
    glUniform2f(program.mScaleUnif, aScaleX, aScaleY);
    glUniform3f(program.mColorUnif, 1.0f, 1.0f, 0.0f);

    glActiveTexture(GL_TEXTURE0 + program.mTextureUnit);
    glBindTexture(GL_TEXTURE_2D, mCacheTexture);
    // TODO Doc
    if (glBindSampler) {
        glBindSampler(0, 0);
    }
    glBindVertexArray(mCacheVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
}

} // namespace gltext
