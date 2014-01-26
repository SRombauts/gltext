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

/**
 * @brief Data of each glyph vertex
 */
struct GlyphVertex {
    GLfloat x;  ///< Vertex x coordinate
    GLfloat y;  ///< Vertex y coordinate
    GLfloat s;  ///< Texture s (x) coordinate
    GLfloat t;  ///< Texture t (y) coordinate
};

#define GLYPH_VERT_SIZE (4*sizeof(GlyphVertex))
#define GLYPH_IDX_SIZE (6*sizeof(GLushort))

static const char* _vertexShaderSource =
"#version 130\n"
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

static const char* _fragmentShaderSource =
"#version 130\n"
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


void checkOpenGlError(const char* apFile, int aLine) {
    GLenum error = glGetError();
    while (GL_NO_ERROR != error) {
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
        error = glGetError();
    }
}
#define GL_CHECK() checkOpenGlError(__FILE__, __LINE__)


/**
 * @brief Compile the shaders and link the program
 */
class Program {
public:
    Program() :
        mTextureUnit(0) {

        // Load OpenGL 3 function pointers
        glload::initGlPointers();

        // Compile shader and link program
        GLuint mVertexShader = compileShader(GL_VERTEX_SHADER, _vertexShaderSource);
        GLuint mFragmentShader = compileShader(GL_FRAGMENT_SHADER, _fragmentShaderSource);
        mProgram = linkProgram(mVertexShader, mFragmentShader);

        // Fetch Attribute (input data streams) and Unitform (variables) locations (ids)
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

        // Check program status, and get error message if a problem occured
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
    mPathFilename(apPathFilename),
    mCacheSize(aCacheSize) {
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
    mPixelWidth = static_cast<unsigned int>(ceil((mFace->max_advance_width * mFace->size->metrics.y_ppem) /
                                                  static_cast<float>(mFace->units_per_EM)));
    mPixelHeight = static_cast<unsigned int>(ceil((mFace->height * mFace->size->metrics.y_ppem) /
                                                   static_cast<float>(mFace->units_per_EM)));

    std::cout << "FontImpl::FontImpl(" << apPathFilename << ", " << aPixelSize << "): "
        << mPixelWidth << "x" << mPixelHeight << std::endl;

    // TODO Calculate appropriate texture cache dimension
    mCacheWidth = 7 * mPixelWidth;
    mCacheHeigth = 7 * mPixelHeight;
    mCacheSize = 7 * 7;
    mCacheNbGlyps = 0;
    mCacheFreeSlotX = 0;
    mCacheFreeSlotY = 0;

    // For cache debug draw
    glGenVertexArrays(1, &mCacheVAO);
    glGenBuffers(1, &mCacheVBO);
    glGenBuffers(1, &mCacheIBO);
    glBindVertexArray(mCacheVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mCacheVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mCacheIBO);
    glBufferData(GL_ARRAY_BUFFER, GLYPH_VERT_SIZE, NULL, GL_DYNAMIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLYPH_IDX_SIZE, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(program.mVertexPositionAttrib);
    glEnableVertexAttribArray(program.mVertexTextureCoordAttrib);
    glVertexAttribPointer(program.mVertexPositionAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(GlyphVertex), 0);
    glVertexAttribPointer(program.mVertexTextureCoordAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(GlyphVertex), reinterpret_cast<GLvoid*>(sizeof(GlyphVertex)/2)); // NOLINT
    GL_CHECK();

    // TODO move these into TextImpl class
    size_t TextLength = mCacheSize;
    glGenVertexArrays(1, &mTextVAO);
    glGenBuffers(1, &mTextVBO);
    glGenBuffers(1, &mTextIBO);
    glBindVertexArray(mTextVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mTextVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mTextIBO);
    glBufferData(GL_ARRAY_BUFFER, TextLength * GLYPH_VERT_SIZE, NULL, GL_DYNAMIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, TextLength * GLYPH_IDX_SIZE, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(program.mVertexPositionAttrib);
    glEnableVertexAttribArray(program.mVertexTextureCoordAttrib);
    glVertexAttribPointer(program.mVertexPositionAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(GlyphVertex), 0);
    glVertexAttribPointer(program.mVertexTextureCoordAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(GlyphVertex), reinterpret_cast<GLvoid*>(sizeof(GlyphVertex)/2)); // NOLINT
    GL_CHECK();

    // Cache texture
    glActiveTexture(GL_TEXTURE0 + program.mTextureUnit);
    glGenTextures(1, &mCacheTexture);
    glBindTexture(GL_TEXTURE_2D, mCacheTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, mCacheWidth, mCacheHeigth, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
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

    Program& program = Program::getInstance();
    glActiveTexture(GL_TEXTURE0 + program.mTextureUnit);
    glBindTexture(GL_TEXTURE_2D, mCacheTexture);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    GL_CHECK();

    // TODO
    for (unsigned i = 0; i < len; ++i) {
        // TODO if (not in cache) {
        unsigned int idxInCache = cache(glyphs[i].codepoint);
        // }
    }
}

// Pre-render and cache the glyph representing the given unicode Unicode codepoint.
unsigned int FontImpl::cache(FT_UInt codepoint) {
    unsigned int idxInCache = 0;

    // Render the glyph with Freetype
    FT_Error error;
    error = FT_Load_Glyph(mFace, codepoint, FT_LOAD_RENDER);
    if (error) {
        throw Exception("FT_Load_Glyph");
    }

    // TODO verification and calculation
    int pitch = mFace->glyph->bitmap.pitch;
    if (pitch < 0) {
        pitch = -pitch;
    }
    glPixelStorei(GL_UNPACK_ROW_LENGTH, pitch);

    std::cout << "FontImpl::cache(" << codepoint << "):"
        << " width=" << mFace->glyph->bitmap.width
        << " rows=" << mFace->glyph->bitmap.rows
        << " pitch=" << pitch << " (" << mFace->glyph->bitmap.pitch << ")"
        << "\n";

    // Load the newly rendered glyph into the texture cache
    // TODO at the appropriate position
    glTexSubImage2D(
        GL_TEXTURE_2D, 0,
        mCacheFreeSlotX, mCacheFreeSlotY, mFace->glyph->bitmap.width, mFace->glyph->bitmap.rows,
        GL_RED, GL_UNSIGNED_BYTE, mFace->glyph->bitmap.buffer);

    // TODO calculation of verticies and indicies
    // TODO manage the cache
    mCacheNbGlyps++;
    mCacheFreeSlotX += mPixelWidth;
    if (mCacheWidth <= mCacheFreeSlotX) {
        mCacheFreeSlotX = 0;
        mCacheFreeSlotY += mPixelHeight;
    }

    GL_CHECK();

    return idxInCache;
}

// Render the given string of characters (or use existing cached glyphs) and put it on a VAO/VBO.
Text FontImpl::render(const std::string& aCharacters, const std::shared_ptr<const FontImpl>& aFontImplPtr) {
    return Text(aFontImplPtr);
}

// Draw the cache texture for debug purpose.
void FontImpl::drawCache(float aX, float aY, float aW, float aH) {
    static bool bFirst = true;
    if (bFirst) {
        std::cout << "FontImpl::drawCache()\n";
        bFirst = false;
    }
    // TODO
    // ^ y/t
    // |
    // 3 - 2
    // | / |
    // 0 - 1 -> x/s
    GlyphVertex corners[4];
    corners[0].x = aX;
    corners[0].y = aY;
    corners[0].s = 0.0f;
    corners[0].t = 1.0f;

    corners[1].x = aX + aW;
    corners[1].y = aY;
    corners[1].s = 1.0f;
    corners[1].t = 1.0f;

    corners[2].x = aX + aW;
    corners[2].y = aY + aH;
    corners[2].s = 1.0f;
    corners[2].t = 0.0f;

    corners[3].x = aX;
    corners[3].y = aY + aH;
    corners[3].s = 0.0f;
    corners[3].t = 0.0f;
    unsigned short indices[6] = {0, 1, 2,  2, 3, 0};

    Program& program = Program::getInstance();

    glActiveTexture(GL_TEXTURE0 + program.mTextureUnit);
    glBindTexture(GL_TEXTURE_2D, mCacheTexture);
    // TODO Doc
    if (glBindSampler) {
        glBindSampler(0, 0);
    }
    glUseProgram(program.mProgram);
    // TODO use real values
    glUniform2f(program.mScaleUnif, 2/640.0f, 2/480.0f);
    glUniform2f(program.mOffsetUnif, 0, 0);
    glUniform3f(program.mColorUnif, 1.0f, 1.0f, 0.0f);

    glBindVertexArray(mTextVAO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, GLYPH_VERT_SIZE, corners);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, GLYPH_IDX_SIZE, indices);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
}

} // namespace gltext
