/**
 * @file    TextImpl.cpp
 * @brief   Private Implementation of a static/constant text rendered with Freetype, ready to be drawn with OpenGL.
 *
 * Copyright (c) 2014 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include "TextImpl.h"   // NOLINT TODO
#include "FontImpl.h"   // NOLINT TODO
#include "Exception.h"  // NOLINT TODO
#include "Program.h"    // NOLINT TODO

#include <stdexcept>
#include <cassert>

namespace gltext {

// Encapsulation.
TextImpl::TextImpl(const std::shared_ptr<const FontImpl>&   aFontImplPtr,
                   size_t                                   aTextLength,
                   size_t                                   aTextVAO,
                   size_t                                   aTextVBO,
                   size_t                                   aTextIBO) :
    mFontImplPtr(aFontImplPtr),
    mTextLength(aTextLength),
    mTextVAO(aTextVAO),
    mTextVBO(aTextVBO),
    mTextIBO(aTextIBO) {
}

// Private Implementation of a static/constant text rendered with Freetype, ready to be drawn with OpenGL..
TextImpl::~TextImpl() {
    // releases mFontImplPtr
    glDeleteVertexArrays(1, &mTextVAO);
    glDeleteBuffers(1, &mTextVBO);
    glDeleteBuffers(1, &mTextIBO);
}

// Initialize the 3D position where to start to draw the text.
void TextImpl::setPosition(float aX, float aY, float aZ) {
    assert(mFontImplPtr);
}

// Ask OpenGL to draw the pre-rendered static text, using the current binded program, at current position.
void TextImpl::draw() {
    assert(mFontImplPtr);

    Program& program = Program::getInstance();
    glUseProgram(program.mProgram);

    // TODO remove this, shall be down outside of this method
    glUniform2f(program.mOffsetUnif, -200.0f, -200.0f);
    glUniform2f(program.mScaleUnif, 1/256.0f, 1/256.0f);
    glUniform3f(program.mColorUnif, 1.0f, 1.0f, 0.0f);

    glActiveTexture(GL_TEXTURE0 + _TextureUnitIdx);
    glBindTexture(GL_TEXTURE_2D, mFontImplPtr->mCacheTexture);
    // Bind to sampler name zero == the currently bound texture's sampler state becomes active (no dedicated sampler)
    glBindSampler(_TextureUnitIdx, 0);

    // Draw the rendered text
    glBindVertexArray(mTextVAO);
    glDrawElements(GL_TRIANGLES, mTextLength * 6, GL_UNSIGNED_SHORT, 0);
}

} // namespace gltext
