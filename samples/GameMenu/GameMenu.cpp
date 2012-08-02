/***********************************************************************
filename:   GameMenu.cpp
created:    31/7/2012
author:     Lukas E Meindl
*************************************************************************/
/***************************************************************************
*   Copyright (C) 2004 - 2012 Paul D Turner & Thce CEGUI Development Team
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
#include "GameMenu.h"

#include "CEGUI/CEGUI.h"

#include <cmath>


using namespace CEGUI;


bool GameMenuDemo::initialise(CEGUI::GUIContext* guiContext)
{
    using namespace CEGUI;

    d_usedFiles = CEGUI::String(__FILE__);
    d_guiContext = guiContext;

    // load scheme and set up defaults
    SchemeManager::getSingleton().createFromFile("GameMenu.scheme");
    d_guiContext->getMouseCursor().setDefaultImage("GameMenuImages/MouseCursor");

    // load font and setup default if not loaded via scheme
    Font& defaultFont = FontManager::getSingleton().createFromFile("Jura-14.font");
    // Set default font for the gui context
    d_guiContext->setDefaultFont(&defaultFont);

    CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();

    d_root = winMgr.loadLayoutFromFile("GameMenu.layout");
    d_guiContext->setRootWindow(d_root);

    setupAnimations();

    return true;
}

/*************************************************************************
Cleans up resources allocated in the initialiseSample call.
*************************************************************************/
void GameMenuDemo::deinitialise()
{
}

void GameMenuDemo::setupAnimations()
{
    AnimationManager& animMgr = AnimationManager::getSingleton();
    animMgr.loadAnimationsFromXML("GameMenu.anims");

    CEGUI::Animation* startButtAnim = animMgr.getAnimation("StartButtonPulsating");
    CEGUI::AnimationInstance* startButtAnimInstance = CEGUI::AnimationManager::getSingleton().instantiateAnimation(startButtAnim);
    CEGUI::Window* startButtWindow = d_root->getChild("InnerPartContainer/InsideStartClickArea/StartButtonImage");
    startButtAnimInstance->setTargetWindow(startButtWindow);
    startButtAnimInstance->start();

    CEGUI::Animation* insideImg1Anim = animMgr.getAnimation("InsideImage1Pulsating");
    CEGUI::AnimationInstance* insideImg1AnimInst = CEGUI::AnimationManager::getSingleton().instantiateAnimation(insideImg1Anim);
    CEGUI::Window* insideImg1 = d_root->getChild("InnerPartContainer/InsideImage1");
    insideImg1AnimInst->setTargetWindow(insideImg1);
    insideImg1AnimInst->start();

    CEGUI::Animation* topBarAnim = animMgr.getAnimation("TopBarMoveInAnimation");
    d_topBarAnimInst = CEGUI::AnimationManager::getSingleton().instantiateAnimation(topBarAnim);
    CEGUI::Window* topBarWindow = d_root->getChild("TopBar");
    d_topBarAnimInst->setTargetWindow(topBarWindow);
    d_topBarAnimInst->start();

    CEGUI::Animation* botBarAnim = animMgr.getAnimation("BotBarMoveInAnimation");
    d_botBarAnimInst = CEGUI::AnimationManager::getSingleton().instantiateAnimation(botBarAnim);
    CEGUI::Window* botBarWindow = d_root->getChild("BotBar");
    d_botBarAnimInst->setTargetWindow(botBarWindow);
    d_botBarAnimInst->start();

    CEGUI::Animation* insideBlendInAnim = animMgr.getAnimation("InsideBlendIn");
    d_insideBlendInAnimInst = CEGUI::AnimationManager::getSingleton().instantiateAnimation(insideBlendInAnim);
    CEGUI::Window* innerPartContainer = d_root->getChild("InnerPartContainer");
    d_insideBlendInAnimInst->setTargetWindow(innerPartContainer);
    d_insideBlendInAnimInst->start();

    
    CEGUI::Animation* insideImage3RotateIn = animMgr.getAnimation("InsideImage3RotateIn");
    d_insideImage3RotateInInst = CEGUI::AnimationManager::getSingleton().instantiateAnimation(insideImage3RotateIn);
    CEGUI::Window* insideImage3 = d_root->getChild("InnerPartContainer/OuterRingsContainer/InsideImage3");
    d_insideImage3RotateInInst->setTargetWindow(insideImage3);
    d_insideImage3RotateInInst->start();

    CEGUI::Animation* insideImage4RotateIn = animMgr.getAnimation("InsideImage4RotateIn");
    d_insideImage4RotateInInst = CEGUI::AnimationManager::getSingleton().instantiateAnimation(insideImage4RotateIn);
    CEGUI::Window* insideImage4 = d_root->getChild("InnerPartContainer/OuterRingsContainer/InsideImage4");
    d_insideImage4RotateInInst->setTargetWindow(insideImage4);
    d_insideImage4RotateInInst->start();

    CEGUI::Animation* insideImageRingsContainerSizeIn = animMgr.getAnimation("RingsContainerSizeIn");
    d_insideImageRingsContainerSizeInInst = CEGUI::AnimationManager::getSingleton().instantiateAnimation(insideImageRingsContainerSizeIn);
    CEGUI::Window* insideImageContainer = d_root->getChild("InnerPartContainer/OuterRingsContainer");
    d_insideImageRingsContainerSizeInInst->setTargetWindow(insideImageContainer);
    d_insideImageRingsContainerSizeInInst->start();
}


void GameMenuDemo::onEnteringSample()
{
    d_topBarAnimInst->start();
    d_botBarAnimInst->start();
    d_insideBlendInAnimInst->start();
    d_insideImage3RotateInInst->start();
    d_insideImage4RotateInInst->start();
    d_insideImageRingsContainerSizeInInst->start();
}

/*************************************************************************
Define the module function that returns an instance of the sample
*************************************************************************/
extern "C" SAMPLE_EXPORT Sample& getSampleInstance()
{
    static GameMenuDemo sample;
    return sample;
}