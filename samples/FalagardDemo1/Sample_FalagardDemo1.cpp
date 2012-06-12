/***********************************************************************
    filename:   Sample_FalagardDemo1.cpp
    created:    19/6/2005
    author:     Paul D Turner
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2006 Paul D Turner & The CEGUI Development Team
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
#include "Sample_FalagardDemo1.h"
#include "CEGUI/CEGUI.h"

//////////////////////////////////////////////////////////////////////////
/*************************************************************************

    FalagardDemo1Sample class

*************************************************************************/
//////////////////////////////////////////////////////////////////////////




/*************************************************************************
   Constructor.
*************************************************************************/
FalagardDemo1Sample::FalagardDemo1Sample()
    : d_usedFiles(__FILE__)
{
}

/*************************************************************************
    Sample specific initialisation goes here.
*************************************************************************/
bool FalagardDemo1Sample::initialise(CEGUI::GUIContext* guiContext)
{
    using namespace CEGUI;

    Logger::getSingleton().setLoggingLevel(Informative);

    // Get window manager which we wil use for a few jobs here.
    WindowManager& winMgr = WindowManager::getSingleton();
    // Load the scheme to initialse the VanillaSkin which we use in this sample
    SchemeManager::getSingleton().createFromFile("VanillaSkin.scheme");
    // set default mouse image
    guiContext->getMouseCursor().setDefaultImage("Vanilla-Images/MouseArrow");

    // load an image to use as a background
    ImageManager::getSingleton().addFromImageFile("BackgroundImage", "GPN-2000-001437.png");

    // here we will use a StaticImage as the root, then we can use it to place a background image
    d_root = winMgr.createWindow("Vanilla/StaticImage");

    // Set the root window as root of our GUI Context
    guiContext->setRootWindow(d_root);

    // set area rectangle
    d_root->setArea(URect(cegui_reldim(0), cegui_reldim(0), cegui_reldim(1), cegui_reldim(1)));
    // disable frame and standard background
    d_root->setProperty("FrameEnabled", "false");
    d_root->setProperty("BackgroundEnabled", "false");
    // set the background image
    d_root->setProperty("Image", "BackgroundImage");

    FontManager::getSingleton().createFromFile("DejaVuSans-10.font");

    // load some demo windows and attach to the background 'root'
    d_root->addChild(winMgr.loadLayoutFromFile("VanillaWindows.layout"));
    
    // create an instance of the console class.
    d_console = new DemoConsole(d_root);

    // listen for key presses on the root window.
    d_root->subscribeEvent(Window::EventKeyDown, Event::Subscriber(&FalagardDemo1Sample::handleRootKeyDown, this));

    // activate the background window
    d_root->activate();

    // success!
    return true;
}


/*************************************************************************
    Cleans up resources allocated in the initialiseSample call.
*************************************************************************/
void FalagardDemo1Sample::deinitialise()
{
    delete d_console;
}


bool FalagardDemo1Sample::handleRootKeyDown(const CEGUI::EventArgs& args)
{
    using namespace CEGUI;

    const KeyEventArgs& keyArgs = static_cast<const KeyEventArgs&>(args);

    switch (keyArgs.scancode)
    {
    case Key::F12:
        d_console->toggleVisibility();
        break;

    default:
        return false;
    }

    return true;
}

/*************************************************************************
    Returns the path of the file used for this
*************************************************************************/
const CEGUI::String& FalagardDemo1Sample::getUsedFilesString()
{
    return d_usedFiles;
}


//////////////////////////////////////////////////////////////////////////
/*************************************************************************

    DemoConsole class

*************************************************************************/
//////////////////////////////////////////////////////////////////////////
// these must match the IDs assigned in the layout
const unsigned int DemoConsole::SubmitButtonID = 1;
const unsigned int DemoConsole::EntryBoxID     = 2;
const unsigned int DemoConsole::HistoryID      = 3;


DemoConsole::DemoConsole(CEGUI::Window* parent) :
d_root(CEGUI::WindowManager::getSingleton().loadLayoutFromFile("VanillaConsole.layout")),
    d_historyPos(0)
{
    using namespace CEGUI;

   /*
         if(parent)
                parent->addChild(d_root);*/
        


    // we will destroy the console box windows ourselves
    d_root->setDestroyedByParent(false);

    // Do events wire-up
    d_root->subscribeEvent(Window::EventKeyDown, Event::Subscriber(&DemoConsole::handleKeyDown, this));

    d_root->getChild(SubmitButtonID)->
        subscribeEvent(PushButton::EventClicked, Event::Subscriber(&DemoConsole::handleSubmit, this));

    d_root->getChild(EntryBoxID)->
        subscribeEvent(Editbox::EventTextAccepted, Event::Subscriber(&DemoConsole::handleSubmit, this));
}


DemoConsole::~DemoConsole()
{
    // destroy the windows that we loaded earlier
    CEGUI::WindowManager::getSingleton().destroyWindow(d_root);
}

void DemoConsole::toggleVisibility()
{
    d_root->isVisible() ? d_root->hide() : d_root->show();
}

bool DemoConsole::isVisible() const
{
    return d_root->isEffectiveVisible();
}

bool DemoConsole::handleSubmit(const CEGUI::EventArgs&)
{
    using namespace CEGUI;

    // get the text entry editbox
    Editbox* editbox = static_cast<Editbox*>(d_root->getChild(EntryBoxID));
    // get text out of the editbox
    String edit_text(editbox->getText());
    // if the string is not empty
    if (!edit_text.empty())
    {
        // add this entry to the command history buffer
        d_history.push_back(edit_text);
        // reset history position
        d_historyPos = d_history.size();
        // append newline to this entry
        edit_text += '\n';
        // get history window
        MultiLineEditbox* history = static_cast<MultiLineEditbox*>(d_root->getChild(HistoryID));
        // append new text to history output
        history->setText(history->getText() + edit_text);
        // scroll to bottom of history output
        history->setCaretIndex(static_cast<size_t>(-1));
        // erase text in text entry box.
        editbox->setText("");
    }

    // re-activate the text entry box
    editbox->activate();

    return true;
}

bool DemoConsole::handleKeyDown(const CEGUI::EventArgs& args)
{
    using namespace CEGUI;

    // get the text entry editbox
    Editbox* editbox = static_cast<Editbox*>(d_root->getChild(EntryBoxID));

    switch (static_cast<const KeyEventArgs&>(args).scancode)
    {
    case Key::ArrowUp:
        d_historyPos = ceguimax(d_historyPos - 1, -1);
        if (d_historyPos >= 0)
        {
            editbox->setText(d_history[d_historyPos]);
            editbox->setCaretIndex(static_cast<size_t>(-1));
        }
        else
        {
            editbox->setText("");
        }

        editbox->activate();
        break;

    case Key::ArrowDown:
        d_historyPos = ceguimin(d_historyPos + 1, static_cast<int>(d_history.size()));
        if (d_historyPos < static_cast<int>(d_history.size()))
        {
            editbox->setText(d_history[d_historyPos]);
            editbox->setCaretIndex(static_cast<size_t>(-1));
        }
        else
        {
            editbox->setText("");
        }

        editbox->activate();
        break;

    default:
        return false;
    }

    return true;
}

/*************************************************************************
    Define the module function that returns an instance of the sample
*************************************************************************/
SAMPLE_EXTERN_IMPL(FalagardDemo1Sample)