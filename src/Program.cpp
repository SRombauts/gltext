/**
 * @file    Program.cpp
 * @brief   OpenGL shaders program.
 *
 * Copyright (c) 2014 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include "Program.h"   // NOLINT TODO

#include <stdexcept>
#include <cassert>
#include <cmath>
#include <string>
#include <iostream>     // NOLINT TODO


namespace gltext {


const GLuint _TextureUnitId = 0;   ///< Id of the texture image unit to use (0)


// Check for any previous OpenGL error. Use with the GL_CHECK() macro
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


Program::Program() {
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
    glUniform1i(textureCacheUnif, _TextureUnitId);
    GL_CHECK();
}

Program::~Program() {
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
GLuint Program::compileShader(GLenum aShaderType, const std::string& aShaderSource) const {
    const char *shaderSource = aShaderSource.c_str();

    std::cout << "Program::compileShader(" << aShaderType << ")\n";

    // Create a shader, give it the source code, and compile it
    GLuint shader = glCreateShader(aShaderType);
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);

    // Check shader status, and get error message if a problem occurred
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
GLuint Program::linkProgram(GLuint aVertexShader, GLuint aFragmentShader) const {
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

} // namespace gltext
