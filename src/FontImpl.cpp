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

/**
 * @brief Data of each glyph vertex
 */
struct GlyphVertex {
    GLfloat x;  ///< Vertex x coordinate
    GLfloat y;  ///< Vertex y coordinate
    GLfloat s;  ///< Texture s (x) coordinate
    GLfloat t;  ///< Texture t (y) coordinate
};

#define GLYPH_VERT_SIZE (sizeof(GlyphVertex))
#define GLYPH_IDX_SIZE (6*sizeof(GLushort))

static const char* _vertexShaderSource =
"#version 130\n"
"\n"
"layout(location = 0) in vec2 position;\n"
"layout(location = 1) in vec2 texCoord;\n"
"\n"
"smooth out vec2 smoothTexCoord;\n"
"\n"
"uniform ivec2 scale;\n"
"uniform ivec2 offset;\n"
"\n"
"void main() {\n"
"    smoothTexCoord = texCoord;\n"
"    gl_Position = vec4((position+vec2(offset))/vec2(scale) * 2.0 - 1.0, 0.0, 1.0);\n"
"}\n";

static const char* _fragmentShaderSource =
"#version 130\n"
"\n"
"smooth in vec2 smoothTexCoord;\n"
"\n"
"out vec2 outputColor;\n"
"\n"
"uniform sampler2D textureCache;\n"
"uniform vec3 color;\n"
"\n"
"void main() {\n"
"    float val = texture(textureCache, smoothTexCoord).r;\n"
"    outputColor = vec4(color*val, val);\n"
"}\n";


/**
 * @brief Compile the shaders and link the program
 */
class Program {
public:
    Program() {
        initGlPointers();

        GLuint mVertexShader = compileShader(GL_VERTEX_SHADER, _vertexShaderSource);
        GLuint mFragmentShader = compileShader(GL_FRAGMENT_SHADER, _fragmentShaderSource);
        mProgram = linkProgram(mVertexShader, mFragmentShader);

        glUseProgram(mProgram);
        glUniform1i(glGetUniformLocation(mProgram, "textureCache"), 0);
        mVertexPositionAttrib = glGetAttribLocation(mProgram, "position");
        mVertexTextureCoordAttrib = glGetAttribLocation(mProgram, "texCoord");
        mScaleUnif = glGetUniformLocation(mProgram, "scale");
        mOffsetUnif = glGetUniformLocation(mProgram, "offset");
        mColorUnif = glGetUniformLocation(mProgram, "color");
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
    mPixelHeight  = static_cast<unsigned int>(ceil((mFace->height * mFace->size->metrics.y_ppem) /
                                                   static_cast<float>(mFace->units_per_EM)));

    // TODO Calculate appropriate texture cache dimension
    mCacheWidth = mPixelWidth;
    mCacheHeigth = mPixelHeight;

    glGenVertexArrays(1, &mTextVAO);
    glGenBuffers(1, &mTextVBO);
    glGenBuffers(1, &mTextIBO);
    glBindVertexArray(mTextVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mTextVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mTextIBO);
    glBufferData(GL_ARRAY_BUFFER, GLYPH_VERT_SIZE*mCacheSize, NULL, GL_DYNAMIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLYPH_IDX_SIZE*mCacheSize, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(program.mVertexPositionAttrib);
    glEnableVertexAttribArray(program.mVertexTextureCoordAttrib);
    glVertexAttribPointer(program.mVertexPositionAttrib, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);
    glVertexAttribPointer(program.mVertexTextureCoordAttrib, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), reinterpret_cast<GLvoid*>(2*sizeof(float))); // NOLINT

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
    glDeleteBuffers(1, &mTextVBO);
    glDeleteBuffers(1, &mTextIBO);
    glDeleteVertexArrays(1, &mTextVAO);
}

// Pre-render and cache the glyphs representing the given characters, to speed-up future rendering.
void FontImpl::cache(const std::string& aCharacters) {
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

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mCacheTexture);
    glBindVertexArray(mTextVAO);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    // TODO
    for(unsigned i = 0; i < len; ++i) {
        // if (not in cache) {
        //    idx = cacheGlyph(glyphs[i].codepoint)
        // }
    }
 }

// Render the given string of characters (or use existing cached glyphs) and put it on a VAO/VBO.
Text FontImpl::render(const std::string& aCharacters, const std::shared_ptr<const FontImpl>& aFontImplPtr) {
    return Text(aFontImplPtr);
}

// Pre-render and cache the glyph representing the given unicode Unicode codepoint.
unsigned int FontImpl::cache(FT_UInt codepoint) {
    unsigned int idxGlyph = 0;
    // TODO
    return idxGlyph;
}

} // namespace gltext
