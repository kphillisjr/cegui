/***********************************************************************
filename:   GameMenu.h
created:    31/7/2012
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
#ifndef _Game_Menu_Demo_
#define _Game_Menu_Demo_

#include "SampleBase.h"

namespace CEGUI
{
    class GUIContext;
}


// Sample class
class GameMenuDemo : public Sample
{
public:
    // method to initialse the samples windows and events.
    virtual bool initialise(CEGUI::GUIContext* guiContext);

    void setupWindows();
    void setupAnimations();

    // method to perform any required cleanup operations.
    virtual void deinitialise();

    virtual void onEnteringSample();

protected:
    void update(float passedTime);

    void blendOutInteractiveElements();
    void blendInInteractiveElements();

    bool handleRootWindowUpdate(const CEGUI::EventArgs& args);
    bool handleLoginAcceptButtonClicked(const CEGUI::EventArgs& args);
    bool handleInnerPartStartClickAreaClick(const CEGUI::EventArgs& args);


    CEGUI::GUIContext* d_guiContext;
    CEGUI::Window* d_root;

    float d_timeSinceStart;

    bool d_interactiveElementsAreActivated;

    CEGUI::AnimationInstance* d_topBarAnimInst;
    CEGUI::AnimationInstance* d_botBarAnimInst;
    CEGUI::AnimationInstance* d_insideBlendInAnimInst;
    CEGUI::AnimationInstance* d_insideImage3RotateInInst;
    CEGUI::AnimationInstance* d_insideImage4RotateInInst;
    CEGUI::AnimationInstance* d_insideImageRingsContainerSizeInInst;

    CEGUI::AnimationInstance* d_buttonFadeInAnimInst1;
    CEGUI::AnimationInstance* d_buttonFadeInAnimInst2;
    CEGUI::AnimationInstance* d_buttonFadeInAnimInst3;
    CEGUI::AnimationInstance* d_buttonFadeInAnimInst4;
    CEGUI::AnimationInstance* d_buttonFadeInAnimInst5;

    CEGUI::AnimationInstance* d_loginContainerMoveInInst;
    CEGUI::AnimationInstance* d_naviButtonLeftMoveInInst;
    CEGUI::AnimationInstance* d_naviButtonRightMoveInInst;
    CEGUI::AnimationInstance* d_naviBotMoveInInst;
    CEGUI::AnimationInstance* d_startButtonBlendInAnimInst;
    CEGUI::AnimationInstance* d_navigationLabelBlendInAnimInst;

    CEGUI::AnimationInstance* d_leftArrowPartialBlendOutInst;
    CEGUI::AnimationInstance* d_rightArrowPartialBlendOutInst;
    CEGUI::AnimationInstance* d_naviIconPartialBlendOutInst;
    CEGUI::AnimationInstance* d_buttonLoadPartialBlendOutInst;
    CEGUI::AnimationInstance* d_buttonOptionsPartialBlendOutInst;
    CEGUI::AnimationInstance* d_buttonSavePartialBlendOutInst;
    CEGUI::AnimationInstance* d_buttonCharactersPartialBlendOutInst;

    CEGUI::AnimationInstance* d_leftArrowBlendInInst;
    CEGUI::AnimationInstance* d_rightArrowBlendInInst;
    CEGUI::AnimationInstance* d_naviIconBlendInInst;
    CEGUI::AnimationInstance* d_buttonLoadBlendInInst;
    CEGUI::AnimationInstance* d_buttonOptionsBlendInInst;
    CEGUI::AnimationInstance* d_buttonSaveBlendInInst;
    CEGUI::AnimationInstance* d_buttonCharactersBlendInInst;

    CEGUI::Window* d_topBarLabel;
    CEGUI::Window* d_botBarLabel;
    CEGUI::Window* d_loginContainer;
    CEGUI::Window* d_botNaviLeftArrow;
    CEGUI::Window* d_botNaviRightArrow;
    CEGUI::Window* d_botNaviCenter;
    CEGUI::Window* d_loginAcceptButton;
    CEGUI::Window* d_innerPartStartClickArea;
    CEGUI::Window* d_navigationDestinationLabel;

};

#endif
