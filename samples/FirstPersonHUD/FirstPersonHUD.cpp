/***********************************************************************
filename:   FirstPersonHUD.cpp
created:    11/8/2012
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
#include "FirstPersonHUD.h"

#include "CEGUI/CEGUI.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

using namespace CEGUI;


struct GamePlate
{
    GamePlate(HUDDemo* hudDemo);
    ~GamePlate();

    void update(float timeSinceLastUpdate);
    int getPoints();

    CEGUI::Window* d_window;
    bool d_isComingFromRight;
    bool d_isDestroyed;
};

GamePlate::GamePlate(HUDDemo* hudDemo)
    : d_isDestroyed(false)
{
    d_window = hudDemo->spawnPlate();

    int randumNumber = rand() % 2;
    d_isComingFromRight = (randumNumber == 0 ? false : true);

    if(d_isComingFromRight)
    {
        d_window->setHorizontalAlignment(HA_RIGHT);
    }
}

GamePlate::~GamePlate()
{
    CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
    winMgr.destroyWindow(d_window);
}

void GamePlate::update(float timeSinceLastUpdate)
{
    CEGUI::UVector2 positionOffset;
    
    if(d_isComingFromRight)
        positionOffset = CEGUI::UVector2(cegui_reldim(timeSinceLastUpdate * -0.05f), cegui_absdim(0.f));
    else
        positionOffset = CEGUI::UVector2(cegui_reldim(timeSinceLastUpdate * 0.05f), cegui_absdim(0.f));

    d_window->setPosition(d_window->getPosition() + positionOffset);

    const CEGUI::UVector2& position = d_window->getPosition();

    if(d_isComingFromRight)
    {
        if(position.d_x.d_scale < -1.5f)
            d_isDestroyed = true;
    }
    else
    {
        if(position.d_x.d_scale > 1.5f)
            d_isDestroyed = true;
    }
}

int GamePlate::getPoints()
{
    CEGUI::Window* window = d_window->getChild("ImageWindowObject");

    CEGUI::String objectImage = window->getProperty("Image");

    if(objectImage.compare(HUDDemo::s_imageNameBread) == 0)
        return 2;
    else if(objectImage.compare(HUDDemo::s_imageNamePoo) == 0)
        return -3;
    else if(objectImage.compare(HUDDemo::s_imageNameSteak) == 0)
        return -8;
    else if(objectImage.compare(HUDDemo::s_imageNamePrizza) == 0)
        return 3;
    else if(objectImage.compare(HUDDemo::s_imageNameVegPeople) == 0)
        return 1;
    else if(objectImage.compare(HUDDemo::s_imageNameVegFruits) == 0)
        return 88;

    return 0;
}

const CEGUI::String HUDDemo::s_imageNamePlate = "HUDDemo/Plate";
const CEGUI::String HUDDemo::s_imageNameBread = "HUDDemo/Bread";
const CEGUI::String HUDDemo::s_imageNamePoo = "HUDDemo/Poo";
const CEGUI::String HUDDemo::s_imageNamePrizza = "HUDDemo/Prizza";
const CEGUI::String HUDDemo::s_imageNameSteak = "HUDDemo/Steak";
const CEGUI::String HUDDemo::s_imageNameVegPeople = "HUDDemo/VegetablePeople";
const CEGUI::String HUDDemo::s_imageNameVegFruits = "HUDDemo/VegetablesAndFruits";

bool HUDDemo::initialise(CEGUI::GUIContext* guiContext)
{
    using namespace CEGUI;

    d_usedFiles = CEGUI::String(__FILE__);
    d_guiContext = guiContext;

    SchemeManager::getSingleton().createFromFile("HUDDemo.scheme");
    SchemeManager::getSingleton().createFromFile("Generic.scheme");
    SchemeManager::getSingleton().createFromFile("VanillaSkin.scheme");

    CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
    // Load the HUDDemo Layout
    d_root = winMgr.loadLayoutFromFile("HUDDemo.layout");
    d_guiContext->setRootWindow(d_root);

    setupMouseCursor();

    srand(static_cast<unsigned int >(time(0)));

    d_score = 0;

    return true;
}

/*************************************************************************
Cleans up resources allocated in the initialiseSample call.
*************************************************************************/
void HUDDemo::deinitialise()
{
    while(!d_gamePlates.empty())
    {
        GamePlate* curPlate = d_gamePlates.back();
        delete curPlate;

        d_gamePlates.pop_back();
    }
}


void HUDDemo::onEnteringSample()
{
    d_score = 0;
}

void HUDDemo::update(float timeSinceLastUpdate)
{
    static float timeSinceLastSpawn(0.0f);

    timeSinceLastSpawn += timeSinceLastUpdate;
 
    updateMouseCursor();

    
    if(timeSinceLastSpawn> 1.2f)
    {
        d_gamePlates.push_back(new GamePlate(this));
        timeSinceLastSpawn -= 1.2f;
    }

   updatePlates(timeSinceLastUpdate);
}

void HUDDemo::setupMouseCursor()
{
    CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();

    d_mouseCursorWnd = winMgr.createWindow("Generic/Image");
    d_mouseCursorWnd->setProperty("Image", "HUDDemo/Spoon");
    d_mouseCursorWnd->setAspectMode(CEGUI::AM_EXPAND);
    d_mouseCursorWnd->setAspectRatio(1.f);
    d_mouseCursorWnd->setSize(CEGUI::USize(cegui_absdim(0.0f), cegui_reldim(0.05f)));
    d_mouseCursorWnd->setAlwaysOnTop(true);
    d_mouseCursorWnd->setMousePassThroughEnabled(true);
    d_root->addChild(d_mouseCursorWnd);
}

void HUDDemo::updateMouseCursor()
{
    CEGUI::Vector2f position = d_guiContext->getMouseCursor().getPosition();

    // We want to position the image-window right top of the actual
    // cursor point so we add its height
    float absHeight = d_mouseCursorWnd->getPixelSize().d_height;
    position.d_y -= absHeight;

    d_mouseCursorWnd->setPosition(
        CEGUI::UVector2(
        cegui_absdim(position.d_x - 5.0f), cegui_absdim(position.d_y + 5.0f))
        );
}


CEGUI::Window* HUDDemo::spawnPlate()
{
    CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();

    CEGUI::Window* plateRoot = winMgr.createWindow("DefaultWindow");
    plateRoot->setSize(CEGUI::USize(cegui_absdim(0.0f), cegui_reldim(0.12f)));
    plateRoot->setAspectMode(CEGUI::AM_EXPAND);
    plateRoot->setAspectRatio(1.0f);
    plateRoot->subscribeEvent(CEGUI::Window::EventMouseClick, Event::Subscriber(&HUDDemo::handlePlateWindowClicked, this));
    d_root->addChild(plateRoot);

    CEGUI::Window* plateImgWnd = winMgr.createWindow("Generic/Image", "ImageWindowPlate");
    plateImgWnd->setProperty("Image", s_imageNamePlate);
    plateImgWnd->setSize(CEGUI::USize(cegui_reldim(1.0f), cegui_absdim(0.0f)));
    plateImgWnd->setAspectRatio(3.308f);
    plateImgWnd->setAspectMode(CEGUI::AM_EXPAND);
    plateImgWnd->setVerticalAlignment(CEGUI::VA_BOTTOM);
    plateImgWnd->setMousePassThroughEnabled(true);
    plateImgWnd->setPixelAligned(false);
    plateRoot->addChild(plateImgWnd);

    CEGUI::String image = getRandomGameImage();

    CEGUI::Window* plateTopping = winMgr.createWindow("Generic/Image", "ImageWindowObject");
    plateTopping->setProperty("Image", image);
    plateTopping->setSize(CEGUI::USize(cegui_reldim(0.88f), cegui_absdim(0.0f)));
    plateTopping->setAspectRatio(1.0f);
    plateTopping->setAspectMode(CEGUI::AM_EXPAND);
    plateTopping->setHorizontalAlignment(CEGUI::HA_CENTRE);
    plateTopping->setMousePassThroughEnabled(true);
    plateTopping->setPixelAligned(false);
    plateRoot->addChild(plateTopping);


    int randumNumber = rand() % 10000;
    float posY = randumNumber / 10000.0f;

    plateRoot->setPosition(CEGUI::UVector2(cegui_absdim(0.0f), cegui_reldim(0.8f * posY)));

    return plateRoot;
}

const CEGUI::String& HUDDemo::getRandomGameImage()
{
    int randumNumber = rand() % 100;

    if(randumNumber > 88)
        return s_imageNamePoo;
    else if(randumNumber > 72)
        return s_imageNamePrizza;
    else if(randumNumber > 55)
        return s_imageNameSteak;
    else if(randumNumber > 52)
        return s_imageNameVegFruits;
    else if(randumNumber > 25)
        return s_imageNameVegPeople;
    else if(randumNumber > 0)
        return s_imageNameBread;

    return s_imageNameBread;
}


void HUDDemo::updatePlates(float timeSinceLastUpdate)
{
    unsigned int vectorSize = d_gamePlates.size();

    for(unsigned int i = 0; i < vectorSize;)
    {
        GamePlate* currentPlate = d_gamePlates[i];

        currentPlate->update(timeSinceLastUpdate);
        if(currentPlate->d_isDestroyed)
        {
            delete currentPlate;
            d_gamePlates[i] = d_gamePlates.back();
            d_gamePlates.pop_back();
            vectorSize = d_gamePlates.size();
        }
        else
            ++i;
    }

}

void HUDDemo::updateScore(int change)
{
    d_score += change;

    CEGUI::Window* scoreWnd = d_root->getChild("ScoreCounter");
    scoreWnd->setText(CEGUI::PropertyHelper<int>::toString(d_score));
}

bool HUDDemo::handlePlateWindowClicked(const CEGUI::EventArgs& args)
{
    const CEGUI::WindowEventArgs& winArgs = static_cast<const CEGUI::WindowEventArgs&>(args);
    

    for(unsigned int i = 0; i < d_gamePlates.size(); ++i)
    {
        GamePlate* gamePlate = d_gamePlates[i];

        if(gamePlate->d_window == winArgs.window)
        {
            updateScore(gamePlate->getPoints());
            gamePlate->d_isDestroyed = true;
        }
    }

    return false;
}

/*************************************************************************
Define the module function that returns an instance of the sample
*************************************************************************/
extern "C" SAMPLE_EXPORT Sample& getSampleInstance()
{
    static HUDDemo sample;
    return sample;
}