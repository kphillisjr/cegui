/***********************************************************************
    filename:   SampleFramework.h
    created:    24/5/2012
    author:     Lukas E Meindl
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
#ifndef _Samples_Framework_h_
#define _Samples_Framework_h_

#include "SamplesFrameworkBase.h"

#include <vector>

// forward declarations
namespace CEGUI
{
    class DefaultWindow;
}

struct DLLSample;

/*!
\brief
    This is 
*/
class SamplesFramework : public SamplesFrameworkBase
{
public:
    SamplesFramework();
    virtual ~SamplesFramework();


    bool initialiseSample();
    void cleanupSample();

    bool initialiseCEGUI();




protected:
    void initialiseFrameworkLayout();
    void loadSamples();

    CEGUI::DefaultWindow* m_root;

    std::vector<DLLSample> m_dllSamples;
};

#endif

