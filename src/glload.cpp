/**
 * @file    glload.cpp
 * @brief   usefull extract from the gltext library of Branan Purvine-Riley
 *
 * Copyright 2011 Branan Purvine-Riley
 *
 *  This is part of gltext, a text-rendering library.
 *
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the
 * above copyright notice and the following two paragraphs appear in
 * all copies of this software.
 *
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN
 * IF THE COPYRIGHT HOLDER HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 *
 * THE COPYRIGHT HOLDER SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE COPYRIGHT HOLDER HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */

#include "glload.hpp"

#ifdef _WIN32
static void* glPointer(const char* funcname) {
    return reinterpret_cast<void*>(wglGetProcAddress(funcname));
}
#else
static void* glPointer(const char* funcname) {
    return reinterpret_cast<void*>(glXGetProcAddress(reinterpret_cast<const GLubyte*>(funcname)));
}
#endif

// These need to be included after the windows stuff
#include "GL3/gl3.h"

#ifdef _WIN32
PFNGLACTIVETEXTUREPROC glActiveTexture;
#endif
PFNGLBINDSAMPLERPROC glBindSampler;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLDELETEBUFFERSPROC glDeleteBuffers;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLBUFFERSUBDATAPROC glBufferSubData;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
PFNGLCREATESHADERPROC glCreateShader;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLDELETESHADERPROC glDeleteShader;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLUNIFORM1IPROC glUniform1i;
PFNGLUNIFORM2IPROC glUniform2i;
PFNGLUNIFORM2FPROC glUniform2f;
PFNGLUNIFORM3FPROC glUniform3f;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLGETPROGRAMIVPROC glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
PFNGLDETACHSHADERPROC glDetachShader;

namespace glload {

/**
 * @brief Initialize OpenGL pointers
 */
void initGlPointers() {
#ifdef _WIN32
    glActiveTexture = (PFNGLACTIVETEXTUREPROC)glPointer("glActiveTexture");
#endif
    glBindSampler = (PFNGLBINDSAMPLERPROC)glPointer("glBindSampler");
    glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)glPointer("glGenVertexArrays");
    glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)glPointer("glBindVertexArray");
    glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)glPointer("glDeleteVertexArrays");
    glGenBuffers = (PFNGLGENBUFFERSPROC)glPointer("glGenBuffers");
    glBindBuffer = (PFNGLBINDBUFFERPROC)glPointer("glBindBuffer");
    glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)glPointer("glDeleteBuffers");
    glBufferData = (PFNGLBUFFERDATAPROC)glPointer("glBufferData");
    glBufferSubData = (PFNGLBUFFERSUBDATAPROC)glPointer("glBufferSubData");
    glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)glPointer("glVertexAttribPointer");
    glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)glPointer("glEnableVertexAttribArray");
    glCreateShader = (PFNGLCREATESHADERPROC)glPointer("glCreateShader");
    glShaderSource = (PFNGLSHADERSOURCEPROC)glPointer("glShaderSource");
    glCompileShader = (PFNGLCOMPILESHADERPROC)glPointer("glCompileShader");
    glDeleteShader = (PFNGLDELETESHADERPROC)glPointer("glDeleteShader");
    glCreateProgram = (PFNGLCREATEPROGRAMPROC)glPointer("glCreateProgram");
    glAttachShader = (PFNGLATTACHSHADERPROC)glPointer("glAttachShader");
    glLinkProgram = (PFNGLLINKPROGRAMPROC)glPointer("glLinkProgram");
    glUseProgram = (PFNGLUSEPROGRAMPROC)glPointer("glUseProgram");
    glUniform1i = (PFNGLUNIFORM1IPROC)glPointer("glUniform1i");
    glUniform2i = (PFNGLUNIFORM2IPROC)glPointer("glUniform2i");
    glUniform2f = (PFNGLUNIFORM2FPROC)glPointer("glUniform2f");
    glUniform3f = (PFNGLUNIFORM3FPROC)glPointer("glUniform3f");
    glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)glPointer("glGetUniformLocation");
    glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)glPointer("glGetAttribLocation");
    glGetShaderiv = (PFNGLGETSHADERIVPROC)glPointer("glGetShaderiv");
    glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)glPointer("glGetShaderInfoLog");
    glGetProgramiv = (PFNGLGETPROGRAMIVPROC)glPointer("glGetProgramiv");
    glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)glPointer("glGetProgramInfoLog");
    glDetachShader = (PFNGLDETACHSHADERPROC)glPointer("glDetachShader");
}

} // namespace glload
