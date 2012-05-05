/***********************************************************************
    filename:   CEGUIOpenGLFBOTextureTarget.cpp
    created:    Wed, 8th Feb 2012
    author:     Lukas E Meindl (based on code by Paul D Turner)
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2012 Paul D Turner & The CEGUI Development Team
 *
 *   Permission is hereby granted, free of charge, to any person obtaining
 *   a copy of this software and associated documentation files (the
 *   "Software"), to deal in the Software without restriction, including
 *   without limitation the rights to use, copy, modify, merge, publish,
 *   distribute, sublicense, and/or sell copies of the Software, and to
 *   permit persons to whom the Software is furnished to do so, subject to
 *   the following conditions:
 *
 *   The above copyright notice and this permission notice shall be
 *   included in all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 *   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *   OTHER DEALINGS IN THE SOFTWARE.
 ***************************************************************************/
#include <GL/glew.h>

#include "CEGUI/RendererModules/OpenGL3/FBOTextureTarget.h"
#include "CEGUI/Exceptions.h"
#include "CEGUI/RenderQueue.h"
#include "CEGUI/GeometryBuffer.h"

#include "CEGUI/RendererModules/OpenGL3/Renderer.h"
#include "CEGUI/RendererModules/OpenGL3/Texture.h"

#include "CEGUI/Logger.h"

#include <sstream>
#include <iostream>

// Start of CEGUI namespace section
namespace CEGUI
{
//----------------------------------------------------------------------------//
const float OpenGL3FBOTextureTarget::DEFAULT_SIZE = 128.0f;

//----------------------------------------------------------------------------//
OpenGL3FBOTextureTarget::OpenGL3FBOTextureTarget(OpenGL3Renderer& owner) :
    OpenGL3TextureTarget(owner)
{
    initialiseRenderTexture();

    // setup area and cause the initial texture to be generated.
    declareRenderSize(Sizef(DEFAULT_SIZE, DEFAULT_SIZE));
}

//----------------------------------------------------------------------------//
OpenGL3FBOTextureTarget::~OpenGL3FBOTextureTarget()
{
    glDeleteFramebuffers(1, &d_frameBuffer);
}

//----------------------------------------------------------------------------//
void OpenGL3FBOTextureTarget::declareRenderSize(const Sizef& sz)
{
    // exit if current size is enough
    if ((d_area.getWidth() >= sz.d_width) && (d_area.getHeight() >=sz.d_height))
        return;

    setArea(Rectf(d_area.getPosition(), d_owner.getAdjustedTextureSize(sz)));
    resizeRenderTexture();
}

//----------------------------------------------------------------------------//
void OpenGL3FBOTextureTarget::activate()
{
    // switch to rendering to the texture
    glBindFramebuffer(GL_FRAMEBUFFER, d_frameBuffer);

    OpenGL3TextureTarget::activate();
}

//----------------------------------------------------------------------------//
void OpenGL3FBOTextureTarget::deactivate()
{
    OpenGL3TextureTarget::deactivate();

    // switch back to rendering to default buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//----------------------------------------------------------------------------//
void OpenGL3FBOTextureTarget::clear()
{
    // save old clear colour
    GLfloat old_col[4];
    glGetFloatv(GL_COLOR_CLEAR_VALUE, old_col);

    // switch to our FBO
    glBindFramebuffer(GL_FRAMEBUFFER, d_frameBuffer);
    // Clear it.
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);
    // switch back to rendering to view port
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // restore previous clear colour
    glClearColor(old_col[0], old_col[1], old_col[2], old_col[3]);
}

//----------------------------------------------------------------------------//
void OpenGL3FBOTextureTarget::initialiseRenderTexture()
{
    // save old texture binding
    GLuint old_tex;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, reinterpret_cast<GLint*>(&old_tex));

    // create FBO
    glGenFramebuffers(1, &d_frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, d_frameBuffer);

    // set up the texture the FBO will draw to
    glGenTextures(1, &d_texture);
    glBindTexture(GL_TEXTURE_2D, d_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
                 static_cast<GLsizei>(DEFAULT_SIZE),
                 static_cast<GLsizei>(DEFAULT_SIZE),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                              GL_TEXTURE_2D, d_texture, 0);

    //Check for framebuffer completeness
    checkFramebufferStatus();

    // switch from our frame buffer back to using default buffer.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // ensure the CEGUI::Texture is wrapping the gl texture and has correct size
    d_CEGUITexture->setOpenGLTexture(d_texture, d_area.getSize());

    // restore previous texture binding.
    glBindTexture(GL_TEXTURE_2D, old_tex);
}

//----------------------------------------------------------------------------//
void OpenGL3FBOTextureTarget::resizeRenderTexture()
{
    // save old texture binding
    GLuint old_tex;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, reinterpret_cast<GLint*>(&old_tex));

    const Sizef sz(d_area.getSize());

    // set the texture to the required size
    glBindTexture(GL_TEXTURE_2D, d_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
                 static_cast<GLsizei>(sz.d_width),
                 static_cast<GLsizei>(sz.d_height),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    clear();

    // ensure the CEGUI::Texture is wrapping the gl texture and has correct size
    d_CEGUITexture->setOpenGLTexture(d_texture, sz);

    // restore previous texture binding.
    glBindTexture(GL_TEXTURE_2D, old_tex);
}

//----------------------------------------------------------------------------//
void OpenGL3FBOTextureTarget::grabTexture()
{
    glDeleteFramebuffers(1, &d_frameBuffer);
    d_frameBuffer = 0;

    OpenGL3TextureTarget::grabTexture();
}

//----------------------------------------------------------------------------//
void OpenGL3FBOTextureTarget::restoreTexture()
{
    OpenGL3TextureTarget::restoreTexture();

    initialiseRenderTexture();
    resizeRenderTexture();
}

//----------------------------------------------------------------------------//
void OpenGL3FBOTextureTarget::checkFramebufferStatus()
{
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    // Check for completeness
    if(status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::stringstream stringStream;
        stringStream << "OpenGL3Renderer: Error  Framebuffer is not complete\n";

        switch(status)
        {
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            stringStream << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\n";
            break;
        case GL_FRAMEBUFFER_UNDEFINED:
            stringStream << "GL_FRAMEBUFFER_UNDEFINED \n";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            stringStream << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT\n";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER :
            stringStream << "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER \n";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            stringStream << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT\n";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            stringStream << "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE\n";
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            stringStream << "GL_FRAMEBUFFER_UNSUPPORTED\n";
            break;
        default:
            stringStream << "Undefined Framebuffer error\n";
            break;
        }

        if (CEGUI::Logger* logger = CEGUI::Logger::getSingletonPtr())
            logger->logEvent(stringStream.str());
        else
            std::cerr << stringStream.str() << std::endl;
    }
}

//----------------------------------------------------------------------------//



} // End of  CEGUI namespace section
