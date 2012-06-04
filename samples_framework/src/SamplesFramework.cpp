/***********************************************************************
filename:   SampleFramework.cpp
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
#include "SamplesFramework.h"

#include "Samples_xmlHandler.h"
#include "Sample.h"
#include "SampleData.h"

#include "CEGUI/CEGUI.h"
#include "CEGUI/Logger.h"
#include "CEGUI/DynamicModule.h"
#include "CEGUI/Version.h"



#include <string>

using namespace CEGUI;

#define S_(X) #X
#define STRINGIZE(X) S_(X)

//platform-dependant DLL delay-loading includes
#if (defined( __WIN32__ ) || defined( _WIN32 )) 
    #include "windows.h"
#endif

typedef Sample& (*getSampleInstance)();
#define GetSampleInstanceFuncName "getSampleInstance"


// Name of the xsd schema file used to validate animation XML files.
const String SamplesFramework::XMLSchemaName("Animation.xsd");

// String that holds the default resource group for loading samples
String SamplesFramework::s_defaultResourceGroup("");


int main(int /*argc*/, char* /*argv*/[])
{
    // Basic start-up for the sample browser application.
    // Will remain in run() until quitting

    SamplesFramework sampleFramework;
    return sampleFramework.run();
}


SamplesFramework::SamplesFramework()
{
}

SamplesFramework::~SamplesFramework()
{
}



bool SamplesFramework::initialiseSample()
{
    using namespace CEGUI;

    initialiseFrameworkLayout();


    loadSamplesDataFromXML("samples.samps", s_defaultResourceGroup);

    loadSamples();

   

    // return true ton continue that the samples framework knows that initialisation was a
    // success, and that it should now run the sample.
    return true;
}

void SamplesFramework::cleanupSample()
{
}

void SamplesFramework::initialiseFrameworkLayout()
{
    // CEGUI relies on various systems being set-up, so this is what we do
    // here first.
    //
    // The first thing to do is load a CEGUI 'scheme' this is basically a file
    // that groups all the required resources and definitions for a particular
    // skin so they can be loaded / initialised easily
    //
    // So, we use the SchemeManager singleton to load in a scheme that loads the
    // imagery and registers widgets for the TaharezLook skin.  This scheme also
    // loads in a font that gets used as the system default.
    SchemeManager::getSingleton().createFromFile("SampleBrowser.scheme");

    // The next thing we do is to set a default mouse cursor image.  This is
    // not strictly essential, although it is nice to always have a visible
    // cursor if a window or widget does not explicitly set one of its own.
    //
    // The TaharezLook Imageset contains an Image named "MouseArrow" which is
    // the ideal thing to have as a defult mouse cursor image.
    System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage("SampleBrowserSkin/MouseArrow");

    // Now the system is initialised, we can actually create some UI elements, for
    // this first example, a full-screen 'root' window is set as the active GUI
    // sheet, and then a simple frame window will be created and attached to it.

    // All windows and widgets are created via the WindowManager singleton.
    WindowManager& winMgr = WindowManager::getSingleton();

    // Here we create a "DefaultWindow".  This is a native type, that is, it does
    // not have to be loaded via a scheme, it is always available.  One common use
    // for the DefaultWindow is as a generic container for other windows.  Its
    // size defaults to 1.0f x 1.0f using the Relative metrics mode, which means
    // when it is set as the root GUI sheet window, it will cover the entire display.
    // The DefaultWindow does not perform any rendering of its own, so is invisible.
    //
    // Create a DefaultWindow called 'Root'.
    m_root = (DefaultWindow*)winMgr.createWindow("DefaultWindow", "Root");

    // set the GUI root window (also known as the GUI "sheet"), so the gui we set up
    // will be visible.
    System::getSingleton().getDefaultGUIContext().setRootWindow(m_root);

    // A FrameWindow is a window with a frame and a titlebar which may be moved around
    // and resized.
    //
    // Create a FrameWindow in the TaharezLook style, and name it 'Demo Window'
    FrameWindow* wnd = (FrameWindow*)winMgr.createWindow("SampleBrowserSkin/SampleWindow", "Demo Window");

    // Here we attach the newly created FrameWindow to the previously created
    // DefaultWindow which we will be using as the root of the displayed gui.
    m_root->addChild(wnd);

    // Windows are in Relative metrics mode by default.  This means that we can
    // specify sizes and positions without having to know the exact pixel size
    // of the elements in advance.  The relative metrics mode co-ordinates are
    // relative to the parent of the window where the co-ordinates are being set.
    // This means that if 0.5f is specified as the width for a window, that window
    // will be half as its parent window.
    //
    // Here we set the FrameWindow so that it is half the size of the display,
    // and centered within the display.
    wnd->setPosition(UVector2(cegui_reldim(0.25f), cegui_reldim( 0.25f)));
    wnd->setSize(USize(cegui_reldim(0.5f), cegui_reldim( 0.5f)));
}

void SamplesFramework::loadSamples()
{
    int size = m_samples.size();
    for(int i = 0; i < size; ++i)
    {
        SampleData& const sampleData = m_samples[i];

        getSampleInstanceFromDLL(sampleData);

        Sample* sample = sampleData.d_sample;
        sampleData.d_sample->initialise();
    }
  
    //sample.deinitialise();
}

void SamplesFramework::getSampleInstanceFromDLL(SampleData& sampleData)
{
    // Append version
    static CEGUI::String versionSuffix( "-" STRINGIZE(CEGUI_VERSION_MAJOR) "." STRINGIZE(CEGUI_VERSION_MINOR));

    CEGUI::DynamicModule* sampleModule = new DynamicModule(sampleData.d_name + versionSuffix);
    getSampleInstance functionPointerGetSample = (getSampleInstance)sampleModule->getSymbolAddress(CEGUI::String(GetSampleInstanceFuncName));

    if(functionPointerGetSample == 0)
    {
        CEGUI::String errorMessage = "The sample creation function is not defined in the dynamic library of " + sampleData.d_name;
        CEGUI_THROW(InvalidRequestException(errorMessage.c_str()));
    }

    sampleData.d_sample =  &(functionPointerGetSample());
}

//----------------------------------------------------------------------------//
void SamplesFramework::loadSamplesDataFromXML(const String& filename,
                                             const String& resourceGroup)
{
    if (filename.empty())
        CEGUI_THROW(InvalidRequestException(
            "SamplesFramework::loadSamplesDataFromXML: "
            "filename supplied for file loading must be valid."));

    Samples_xmlHandler handler(this);

    // do parse (which uses handler to create actual data)
    CEGUI_TRY
    {
        System::getSingleton().getXMLParser()->
            parseXMLFile(handler, filename, XMLSchemaName,
                         resourceGroup.empty() ? resourceGroup : s_defaultResourceGroup);
    }
    CEGUI_CATCH(...)
    {
        Logger::getSingleton().logEvent(
            "SamplesFramework::loadSamplesDataFromXML: "
            "loading of sample data from file '" + filename + "' has failed.",
            Errors);

        CEGUI_RETHROW;
    }
}


void SamplesFramework::addSampleData(const SampleData& sampleData)
{
    m_samples.push_back(sampleData);
}

/*!
\brief
Sets the default resource group to be used when loading samples xml
data

\param resourceGroup
String describing the default resource group identifier to be used.
*/
void SamplesFramework::setDefaultResourceGroup(const String& resourceGroup)
{
    s_defaultResourceGroup = resourceGroup;
}
