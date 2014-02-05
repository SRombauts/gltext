/**
 * @file    Program.h
 * @brief   OpenGL shaders program.
 *
 * Copyright (c) 2014 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include <gltext/Text.h>

#include <string>
#include <map>
#include <vector>

#include <hb-ft.h>      // HarfBuzz Freetype interface

#include "glload.hpp"   // OpenGL types & function pointers

namespace gltext {

/**
 * @brief Check for any previous OpenGL error. Use with the GL_CHECK() macro
 *
 * @param[in] apFile    File where the check is done
 * @param[in] aLine     Line where the check is called
 *
 * @todo move in a dedicated file
 */
void checkOpenGlError(const char* apFile, int aLine);

/// Macro checking for any previous OpenGL error.
#define GL_CHECK()  checkOpenGlError(__FILE__, __LINE__)

extern const GLuint _TextureUnitIdx;   ///< Id of the texture image unit to use (0)


/**
 * @brief Compile the shaders and link the program
 */
class Program {
public:
    /// Constructor
    Program();
    /// Destructor
    ~Program();

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
    GLuint compileShader(GLenum aShaderType, const std::string& aShaderSource) const;

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
    GLuint linkProgram(GLuint aVertexShader, GLuint aFragmentShader) const;

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

} // namespace gltext
