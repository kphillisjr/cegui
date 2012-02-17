/***********************************************************************
	filename: 	CEGUISystem.cpp
	created:	20/2/2004
	author:		Paul D Turner

	purpose:	Implementation of main system object
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
#ifdef HAVE_CONFIG_H
#   include "config.h"
#endif

#include "CEGUI/System.h"
#include "CEGUI/Clipboard.h"
#include "CEGUI/DefaultLogger.h"
#include "CEGUI/ImageManager.h"
#include "CEGUI/FontManager.h"
#include "CEGUI/WindowFactoryManager.h"
#include "CEGUI/WindowManager.h"
#include "CEGUI/SchemeManager.h"
#include "CEGUI/RenderEffectManager.h"
#include "CEGUI/AnimationManager.h"
#include "CEGUI/MouseCursor.h"
#include "CEGUI/Window.h"
#include "CEGUI/Exceptions.h"
#include "CEGUI/ScriptModule.h"
#include "CEGUI/Config_xmlHandler.h"
#include "CEGUI/DataContainer.h"
#include "CEGUI/ResourceProvider.h"
#include "CEGUI/GlobalEventSet.h"
#include "CEGUI/falagard/WidgetLookManager.h"
#include "CEGUI/PropertyHelper.h"
#include "CEGUI/WindowRendererManager.h"
#include "CEGUI/DynamicModule.h"
#include "CEGUI/XMLParser.h"
#include "CEGUI/GUIContext.h"
#include "CEGUI/RenderingWindow.h"
#include "CEGUI/RenderingContext.h"
#include "CEGUI/DefaultResourceProvider.h"
#include "CEGUI/ImageCodec.h"
#include "CEGUI/widgets/All.h"
#ifdef CEGUI_HAS_PCRE_REGEX
#   include "CEGUI/PCRERegexMatcher.h"
#endif
#include <ctime>
#include <clocale>

// declare create / destroy functions used for XMLParser and ImageCodec
// modules as extern when static linking
#if defined(CEGUI_STATIC)
extern "C"
{
// XML Parser
CEGUI::XMLParser* createParser(void);
void destroyParser(CEGUI::XMLParser* parser);
// Image codec
CEGUI::ImageCodec* createImageCodec(void);
void destroyImageCodec(CEGUI::ImageCodec* imageCodec);
}
#endif

#define S_(X) #X
#define STRINGIZE(X) S_(X)

// Start of CEGUI namespace section
namespace CEGUI
{
const String System::EventNamespace("System");

/*************************************************************************
	Static Data Definitions
*************************************************************************/
// singleton instance pointer
template<> System* Singleton<System>::ms_Singleton	= 0;

// event names
const String System::EventGUISheetChanged( "GUISheetChanged" );
const String System::EventSingleClickTimeoutChanged( "SingleClickTimeoutChanged" );
const String System::EventMultiClickTimeoutChanged( "MultiClickTimeoutChanged" );
const String System::EventMultiClickAreaSizeChanged( "MultiClickAreaSizeChanged" );
const String System::EventDefaultFontChanged( "DefaultFontChanged" );
const String System::EventMouseMoveScalingChanged( "MouseMoveScalingChanged" );
const String System::EventDisplaySizeChanged( "DisplaySizeChanged" );
const String System::EventRenderedStringParserChanged("RenderedStringParserChanged");

// Holds name of default XMLParser
String System::d_defaultXMLParserName(STRINGIZE(CEGUI_DEFAULT_XMLPARSER));
// Holds name of default ImageCodec
String System::d_defaultImageCodecName(STRINGIZE(CEGUI_DEFAULT_IMAGE_CODEC));


/*************************************************************************
    Constructor
*************************************************************************/
System::System(Renderer& renderer,
               ResourceProvider* resourceProvider,
               XMLParser* xmlParser,
               ImageCodec* imageCodec,
               ScriptModule* scriptModule,
               const String& configFile,
               const String& logFile)

: d_renderer(&renderer),
  d_resourceProvider(resourceProvider),
  d_ourResourceProvider(false),
  d_defaultFont(0),
  d_defaultGUIContext(CEGUI_NEW_AO GUIContext(d_renderer->getDefaultRenderTarget())),
  d_clipboard(CEGUI_NEW_AO Clipboard()),
  d_scriptModule(scriptModule),
  d_xmlParser(xmlParser),
  d_ourXmlParser(false),
  d_parserModule(0),
  d_defaultTooltip(0),
  d_weOwnTooltip(false),
  d_imageCodec(imageCodec),
  d_ourImageCodec(false),
  d_imageCodecModule(0),
  d_ourLogger(Logger::getSingletonPtr() == 0),
  d_customRenderedStringParser(0)
{
    // Start out by fixing the numeric locale to C (we depend on this behaviour)
    // consider a UVector2 as a property {{0.5,0},{0.5,0}} could become {{0,5,0},{0,5,0}}
    setlocale(LC_NUMERIC, "C");

    // Instantiate logger first (we have no file at this point, but entries will
    // be cached until we do)
    //
    // NOTE: If the user already created a logger prior to calling this
    // constructor, we mark it as so and leave the logger untouched. This allows
    // the user to fully customize the logger as he sees fit without fear of
    // seeing its configuration overwritten by this.
#ifdef CEGUI_HAS_DEFAULT_LOGGER
    if (d_ourLogger)
        CEGUI_NEW_AO DefaultLogger();
#endif

    Logger& logger(Logger::getSingleton());

    // create default resource provider, unless one was already provided
    if (!d_resourceProvider)
    {
        d_resourceProvider = CEGUI_NEW_AO DefaultResourceProvider();
        d_ourResourceProvider = true;
    }

    // handle initialisation and setup of the XML parser
    setupXMLParser();

    // now XML is available, read the configuration file (if any)
    Config_xmlHandler config;
    if (!configFile.empty())
    {
        CEGUI_TRY
        {
            d_xmlParser->parseXMLFile(config, configFile,
                                      config.CEGUIConfigSchemaName,
                                      "");
        }
        CEGUI_CATCH(...)
        {
            // cleanup XML stuff
            d_xmlParser->cleanup();
            CEGUI_DELETE_AO d_xmlParser;
            CEGUI_RETHROW;
        }
    }

    // Initialise logger if the user didn't create a logger beforehand
    if (d_ourLogger)
        config.initialiseLogger(logFile);

    // if we created the resource provider we know it's DefaultResourceProvider
    // so can auto-initialise the resource group directories via the config
    if (d_ourResourceProvider)
        config.initialiseResourceGroupDirectories();

    // get config to update XML parser if it needs to.
    config.initialiseXMLParser();

    // set up ImageCodec
    config.initialiseImageCodec();
    if (!d_imageCodec)
        setupImageCodec("");

    // initialise any default resource groups specified in the config.
    config.initialiseDefaultResourceGroups();

    outputLogHeader();

    // beginning main init
    logger.logEvent("---- Begining CEGUI System initialisation ----");

    // create the core system singleton objects
    createSingletons();

    // add the window factories for the core window types
    addStandardWindowFactories();

    char addr_buff[32];
    sprintf(addr_buff, "(%p)", static_cast<void*>(this));
    logger.logEvent("CEGUI::System singleton created. " + String(addr_buff));
    logger.logEvent("---- CEGUI System initialisation completed ----");
    logger.logEvent("");

    // autoload resources specified in config
    config.loadAutoResources();

    // set up defaults
    config.initialiseDefaultFont();
    config.initialiseDefaultMouseCursor();
    config.initialiseDefaulTooltip();

    // scripting available?
    if (d_scriptModule)
    {
        d_scriptModule->createBindings();
        config.executeInitScript();
        d_termScriptName = config.getTerminateScriptName();
    }
}


/*************************************************************************
	Destructor
*************************************************************************/
System::~System(void)
{
	Logger::getSingleton().logEvent("---- Begining CEGUI System destruction ----");

	// execute shut-down script
	if (!d_termScriptName.empty())
	{
		CEGUI_TRY
		{
			executeScriptFile(d_termScriptName);
		}
		CEGUI_CATCH (...) {}  // catch all exceptions and continue system shutdown

	}

    cleanupImageCodec();

    // cleanup XML stuff
    cleanupXMLParser();

    //
	// perform cleanup in correct sequence
	//
    // ensure no windows get created during destruction.  NB: I'm allowing the
    // potential exception to escape here so as to make it obvious that client
    // code should really be adjusted to not create windows during cleanup.
    WindowManager::getSingleton().lock();
	// destroy windows so it's safe to destroy factories
    WindowManager::getSingleton().destroyAllWindows();
    WindowManager::getSingleton().cleanDeadPool();

    // remove factories so it's safe to unload GUI modules
	WindowFactoryManager::getSingleton().removeAllFactories();

    // Cleanup script module bindings
    if (d_scriptModule)
        d_scriptModule->destroyBindings();

	// cleanup singletons
    destroySingletons();

    CEGUI_DELETE_AO d_defaultGUIContext;

    // cleanup resource provider if we own it
    if (d_ourResourceProvider)
        CEGUI_DELETE_AO d_resourceProvider;

    char addr_buff[32];
    sprintf(addr_buff, "(%p)", static_cast<void*>(this));
	Logger::getSingleton().logEvent("CEGUI::System singleton destroyed. " +
       String(addr_buff));
	Logger::getSingleton().logEvent("---- CEGUI System destruction completed ----");

#ifdef CEGUI_HAS_DEFAULT_LOGGER
    // delete the Logger object only if we created it.
    if (d_ourLogger)
        CEGUI_DELETE_AO Logger::getSingletonPtr();
#endif
    
    CEGUI_DELETE_AO d_clipboard;
}

//---------------------------------------------------------------------------//
void System::signalRedraw()
{
    d_defaultGUIContext->markAsDirty();
}

//---------------------------------------------------------------------------//
bool System::isRedrawRequested() const
{
    return d_defaultGUIContext->isDirty();
}

//---------------------------------------------------------------------------//
unsigned int System::getMajorVersion()
{
    return CEGUI_VERSION_MAJOR;
}

//---------------------------------------------------------------------------//
unsigned int System::getMinorVersion()
{
    return CEGUI_VERSION_MINOR;
}

//---------------------------------------------------------------------------//
unsigned int System::getPatchVersion()
{
    return CEGUI_VERSION_PATCH;
}

//---------------------------------------------------------------------------//
const String& System::getVersion()
{
    static String ret(STRINGIZE(CEGUI_VERSION_MAJOR) "." STRINGIZE(CEGUI_VERSION_MINOR) "." STRINGIZE(CEGUI_VERSION_PATCH));
    return ret;
}

//---------------------------------------------------------------------------//
const String& System::getVerboseVersion()
{
    static String ret("");

    if (ret.empty())
    {
        ret = PropertyHelper<uint>::toString(CEGUI_VERSION_MAJOR) + "." +
              PropertyHelper<uint>::toString(CEGUI_VERSION_MINOR) + "." +
              PropertyHelper<uint>::toString(CEGUI_VERSION_PATCH);

        ret += " (Build: " __DATE__;

#if defined(CEGUI_STATIC)
        ret += " Static";
#endif

#if defined(DEBUG) || defined(_DEBUG)
        ret += " Debug";
#endif

#if defined(__linux__)
        ret += " GNU/Linux";
#elif defined (__FreeBSD__)
        ret += " FreeBSD";
#elif defined (__APPLE__)
        ret += " Apple Mac";
#elif defined (_WIN32) || defined (__WIN32__)
        ret += " Microsoft Windows";
#endif

#ifdef __GNUG__
        ret += " g++ " __VERSION__;

#ifdef _LP64
        ret += " 64 bit";
#else
        ret += " 32 bit";
#endif

#elif defined(_MSC_VER)
        ret += " MSVC++ ";
#if _MSC_VER <= 1200
        ret += "Dinosaur Edition!";
#elif _MSC_VER == 1300
        ret += "7.0";
#elif _MSC_VER == 1310
        ret += "7.1";
#elif _MSC_VER == 1400
        ret += "8.0";
#elif _MSC_VER == 1500
        ret += "9.0";
#elif _MSC_VER == 1600
        ret += "10.0";
#elif _MSC_VER > 1600
        ret += "Great Scott!";
#endif

#ifdef _WIN64
        ret += " 64 bit";
#else
        ret += " 32 bit";
#endif

#endif

        ret += ")";
    }

    return ret;
}

/*************************************************************************
	Render the GUI for this frame
*************************************************************************/
void System::renderGUI(void)
{
    d_renderer->beginRendering();

    d_defaultGUIContext->drawContent();

    d_renderer->endRendering();

    // do final destruction on dead-pool windows
    WindowManager::getSingleton().cleanDeadPool();
}


/*************************************************************************
	Set the default font to be used by the system
*************************************************************************/
void System::setDefaultFont(const String& name)
{
	if (name.empty())
	{
		setDefaultFont(0);
	}
	else
	{
		setDefaultFont(&FontManager::getSingleton().get(name));
	}

}


/*************************************************************************
	Set the default font to be used by the system
*************************************************************************/
void System::setDefaultFont(Font* font)
{
	d_defaultFont = font;

	// fire event
	EventArgs args;
	onDefaultFontChanged(args);
}


/*************************************************************************
	Return a pointer to the ScriptModule being used for scripting within
	the GUI system.
*************************************************************************/
ScriptModule* System::getScriptingModule(void) const
{
	return d_scriptModule;
}

/*************************************************************************
    Set the scripting module
*************************************************************************/
void System::setScriptingModule(ScriptModule* scriptModule)
{
    // cleanup old bindings
    if (d_scriptModule)
        d_scriptModule->destroyBindings();
    // set new scriptModule
    d_scriptModule = scriptModule;

    if (d_scriptModule)
    {
        // log the new ScriptModule ID string
        Logger::getSingleton().logEvent("---- Scripting module is now: "
            + d_scriptModule->getIdentifierString() + " ----");

        // create bindings on the new scriptModule
        d_scriptModule->createBindings();
    }
}

/*************************************************************************
	Return a pointer to the ResourceProvider being used for within the GUI
    system.
*************************************************************************/
ResourceProvider* System::getResourceProvider(void) const
{
	return d_resourceProvider;
}

/*************************************************************************
	Execute a script file if possible.
*************************************************************************/
void System::executeScriptFile(const String& filename, const String& resourceGroup) const
{
	if (d_scriptModule)
	{
		CEGUI_TRY
		{
			d_scriptModule->executeScriptFile(filename, resourceGroup);
		}
        // Forward script exceptions with line number and file info
        CEGUI_CATCH(ScriptException& e)
        {
            CEGUI_THROW(e);
        }
		CEGUI_CATCH(...)
		{
			CEGUI_THROW(GenericException("System::executeScriptFile - An exception was thrown during the execution of the script file."));
		}

	}
	else
	{
		Logger::getSingleton().logEvent("System::executeScriptFile - the script named '" + filename +"' could not be executed as no ScriptModule is available.", Errors);
	}

}


/*************************************************************************
	Execute a scripted global function if possible.  The function should
	not take any parameters and should return an integer.
*************************************************************************/
int	System::executeScriptGlobal(const String& function_name) const
{
	if (d_scriptModule)
	{
		CEGUI_TRY
		{
			return d_scriptModule->executeScriptGlobal(function_name);
		}
        // Forward script exceptions with line number and file info
        CEGUI_CATCH(ScriptException& e)
        {
            CEGUI_THROW(e);
        }
		CEGUI_CATCH(...)
		{
			CEGUI_THROW(GenericException("System::executeScriptGlobal - An exception was thrown during execution of the scripted function."));
		}

	}
	else
	{
		Logger::getSingleton().logEvent("System::executeScriptGlobal - the global script function named '" + function_name +"' could not be executed as no ScriptModule is available.", Errors);
	}

	return 0;
}


/*************************************************************************
    If possible, execute script code contained in the given
    CEGUI::String object.
*************************************************************************/
void System::executeScriptString(const String& str) const
{
    if (d_scriptModule)
    {
        CEGUI_TRY
        {
            d_scriptModule->executeString(str);
        }
        // Forward script exceptions with line number and file info
        CEGUI_CATCH(ScriptException& e)
        {
            CEGUI_THROW(e);
        }
        CEGUI_CATCH(...)
        {
            CEGUI_THROW(GenericException("System::executeScriptString - An exception was thrown during execution of the script code."));
        }

    }
    else
    {
        Logger::getSingleton().logEvent("System::executeScriptString - the script code could not be executed as no ScriptModule is available.", Errors);
    }
}


/*************************************************************************
	Method that injects a mouse movement event into the system
*************************************************************************/
bool System::injectMouseMove(float delta_x, float delta_y)
{
    return d_defaultGUIContext->injectMouseMove(delta_x, delta_y);
}

/*************************************************************************
	Method that injects that the mouse is leaves the application window
*************************************************************************/
bool System::injectMouseLeaves(void)
{
    return d_defaultGUIContext->injectMouseLeaves();
}


/*************************************************************************
	Method that injects a mouse button down event into the system.
*************************************************************************/
bool System::injectMouseButtonDown(MouseButton button)
{
    return d_defaultGUIContext->injectMouseButtonDown(button);
}


/*************************************************************************
	Method that injects a mouse button up event into the system.
*************************************************************************/
bool System::injectMouseButtonUp(MouseButton button)
{
    return d_defaultGUIContext->injectMouseButtonUp(button);
}


/*************************************************************************
	Method that injects a key down event into the system.
*************************************************************************/
bool System::injectKeyDown(Key::Scan scan_code)
{
    return d_defaultGUIContext->injectKeyDown(scan_code);
}


/*************************************************************************
	Method that injects a key up event into the system.
*************************************************************************/
bool System::injectKeyUp(Key::Scan scan_code)
{
    return d_defaultGUIContext->injectKeyUp(scan_code);
}


/*************************************************************************
	Method that injects a typed character event into the system.
*************************************************************************/
bool System::injectChar(String::value_type code_point)
{
    return d_defaultGUIContext->injectChar(code_point);
}


/*************************************************************************
	Method that injects a mouse-wheel / scroll-wheel event into the system.
*************************************************************************/
bool System::injectMouseWheelChange(float delta)
{
    return d_defaultGUIContext->injectMouseWheelChange(delta);
}


/*************************************************************************
	Method that injects a new position for the mouse cursor.
*************************************************************************/
bool System::injectMousePosition(float x_pos, float y_pos)
{
    return d_defaultGUIContext->injectMousePosition(x_pos, y_pos);
}


/*************************************************************************
	Method to inject time pulses into the system.
*************************************************************************/
bool System::injectTimePulse(float timeElapsed)
{
    AnimationManager::getSingleton().autoStepInstances(timeElapsed);

    return d_defaultGUIContext->injectTimePulse(timeElapsed);
}

/*************************************************************************
	Return the next window that should receive input in the chain
*************************************************************************/
Window* System::getNextTargetWindow(Window* w) const
{
	return 0;//(w != d_modalTarget) ? w->getParent() : 0;
}


System&	System::getSingleton(void)
{
	return Singleton<System>::getSingleton();
}


System*	System::getSingletonPtr(void)
{
	return Singleton<System>::getSingletonPtr();
}

/*************************************************************************
	Handler called when the main system GUI Sheet (or root window) is changed
*************************************************************************/
void System::onGUISheetChanged(WindowEventArgs& e)
{
	fireEvent(EventGUISheetChanged, e, EventNamespace);
}


/*************************************************************************
	Handler called when the single-click timeout value is changed.
*************************************************************************/
void System::onSingleClickTimeoutChanged(EventArgs& e)
{
	fireEvent(EventSingleClickTimeoutChanged, e, EventNamespace);
}


/*************************************************************************
	Handler called when the multi-click timeout value is changed.
*************************************************************************/
void System::onMultiClickTimeoutChanged(EventArgs& e)
{
	fireEvent(EventMultiClickTimeoutChanged, e, EventNamespace);
}


/*************************************************************************
	Handler called when the size of the multi-click tolerance area is
	changed.
*************************************************************************/
void System::onMultiClickAreaSizeChanged(EventArgs& e)
{
	fireEvent(EventMultiClickAreaSizeChanged, e, EventNamespace);
}


/*************************************************************************
	Handler called when the default system font is changed.
*************************************************************************/
void System::onDefaultFontChanged(EventArgs& e)
{
    // here we need to inform every window using the default font that
    // it's font has been changed.
    WindowManager::WindowIterator iter =
        WindowManager::getSingleton().getIterator();

    // Args structure we will re-use for all windows.
    WindowEventArgs args(0);

    while (!iter.isAtEnd())
    {
        Window* wnd = iter.getCurrentValue();

        if (wnd->getFont(false) == 0)
        {
            args.window = wnd;
            wnd->onFontChanged(args);
            // ensure 'handled' state is reset.
            args.handled = 0;
        }

        ++iter;
    }

	fireEvent(EventDefaultFontChanged, e, EventNamespace);
}


/*************************************************************************
	Handler called when the mouse movement scaling factor is changed.
*************************************************************************/
void System::onMouseMoveScalingChanged(EventArgs& e)
{
	fireEvent(EventMouseMoveScalingChanged, e, EventNamespace);
}


/*************************************************************************
	Handler method for display size change notifications
*************************************************************************/
void System::notifyDisplaySizeChanged(const Sizef& new_size)
{
    // notify other components of the display size change
    d_renderer->setDisplaySize(new_size);
    ImageManager::getSingleton().notifyDisplaySizeChanged(new_size);
    FontManager::getSingleton().notifyDisplaySizeChanged(new_size);

	// notify gui sheet / root if size change, event propagation will ensure everything else
	// gets updated as required.
    //
    // FIXME: This is no longer correct, the RenderTarget the sheet is using as
    // FIXME: it's parent element may not be the main screen.
    d_defaultGUIContext->notifySurfaceSizeChanged(new_size);

    invalidateAllWindows();

    // Fire event
    DisplayEventArgs args(new_size);
    fireEvent(EventDisplaySizeChanged, args, EventNamespace);

    Logger::getSingleton().logEvent(
        "Display resize:"
        " w=" + PropertyHelper<float>::toString(new_size.d_width) +
        " h=" + PropertyHelper<float>::toString(new_size.d_height));
}


/*************************************************************************
	Internal method used to inform the System object whenever a window is
	destroyed, so that System can perform any required housekeeping.
*************************************************************************/
void System::notifyWindowDestroyed(const Window* window)
{
    d_defaultGUIContext->notifyWindowDestroyed(window);

    if (d_defaultTooltip == window)
    {
        d_defaultTooltip = 0;
        d_weOwnTooltip = false;
    }
}

void System::setDefaultTooltip(Tooltip* tooltip)
{
    destroySystemOwnedDefaultTooltipWindow();

    d_defaultTooltip = tooltip;

    if (d_defaultTooltip)
        d_defaultTooltip->setWritingXMLAllowed(false);
}

void System::setDefaultTooltip(const String& tooltipType)
{
    destroySystemOwnedDefaultTooltipWindow();

    d_defaultTooltipType = tooltipType;
}

//----------------------------------------------------------------------------//
void System::createSystemOwnedDefaultTooltipWindow() const
{
    WindowManager& winmgr(WindowManager::getSingleton());

    if (!winmgr.isLocked())
    {
        d_defaultTooltip = static_cast<Tooltip*>(
            winmgr.createWindow(d_defaultTooltipType,
                                "CEGUI::System::default__auto_tooltip__"));
        d_defaultTooltip->setWritingXMLAllowed(false);
        d_weOwnTooltip = true;
    }
}

//----------------------------------------------------------------------------//
void System::destroySystemOwnedDefaultTooltipWindow()
{
    if (d_defaultTooltip && d_weOwnTooltip)
    {
        WindowManager::getSingleton().destroyWindow(d_defaultTooltip);
        d_defaultTooltip = 0;
    }

    d_weOwnTooltip = false;
}

//----------------------------------------------------------------------------//
Tooltip* System::getDefaultTooltip(void) const
{
    if (!d_defaultTooltip && !d_defaultTooltipType.empty())
        createSystemOwnedDefaultTooltipWindow();

    return d_defaultTooltip;
}

//----------------------------------------------------------------------------//

void System::outputLogHeader()
{
    Logger& l(Logger::getSingleton());
    l.logEvent("");
    l.logEvent("********************************************************************************");
    l.logEvent("* Important:                                                                   *");
    l.logEvent("*     To get support at the CEGUI forums, you must post _at least_ the section *");
    l.logEvent("*     of this log file indicated below.  Failure to do this will result in no  *");
    l.logEvent("*     support being given; please do not waste our time.                       *");
    l.logEvent("********************************************************************************");
    l.logEvent("********************************************************************************");
    l.logEvent("* -------- START OF ESSENTIAL SECTION TO BE POSTED ON THE FORUM       -------- *");
    l.logEvent("********************************************************************************");
    l.logEvent("---- Version: " + getVerboseVersion() + " ----");
    l.logEvent("---- Renderer module is: " + d_renderer->getIdentifierString() + " ----");
    l.logEvent("---- XML Parser module is: " + d_xmlParser->getIdentifierString() + " ----");
    l.logEvent("---- Image Codec module is: " + d_imageCodec->getIdentifierString() + " ----");
    l.logEvent(d_scriptModule ? "---- Scripting module is: " + d_scriptModule->getIdentifierString() + " ----" : "---- Scripting module is: None ----");
    l.logEvent("********************************************************************************");
    l.logEvent("* -------- END OF ESSENTIAL SECTION TO BE POSTED ON THE FORUM         -------- *");
    l.logEvent("********************************************************************************");
    l.logEvent("");
}

void System::addStandardWindowFactories()
{
    // Add types all base elements
    WindowFactoryManager::addWindowType<DefaultWindow>();
    WindowFactoryManager::addWindowType<DragContainer>();
    WindowFactoryManager::addWindowType<ScrolledContainer>();
    WindowFactoryManager::addWindowType<ClippedContainer>();
    WindowFactoryManager::addWindowType<Checkbox>();
    WindowFactoryManager::addWindowType<PushButton>();
    WindowFactoryManager::addWindowType<RadioButton>();
    WindowFactoryManager::addWindowType<Combobox>();
    WindowFactoryManager::addWindowType<ComboDropList>();
    WindowFactoryManager::addWindowType<Editbox>();
    WindowFactoryManager::addWindowType<FrameWindow>();
    WindowFactoryManager::addWindowType<ItemEntry>();
    WindowFactoryManager::addWindowType<Listbox>();
    WindowFactoryManager::addWindowType<ListHeader>();
    WindowFactoryManager::addWindowType<ListHeaderSegment>();
    WindowFactoryManager::addWindowType<Menubar>();
    WindowFactoryManager::addWindowType<PopupMenu>();
    WindowFactoryManager::addWindowType<MenuItem>();
    WindowFactoryManager::addWindowType<MultiColumnList>();
    WindowFactoryManager::addWindowType<MultiLineEditbox>();
    WindowFactoryManager::addWindowType<ProgressBar>();
    WindowFactoryManager::addWindowType<ScrollablePane>();
    WindowFactoryManager::addWindowType<Scrollbar>();
    WindowFactoryManager::addWindowType<Slider>();
    WindowFactoryManager::addWindowType<Spinner>();
    WindowFactoryManager::addWindowType<TabButton>();
    WindowFactoryManager::addWindowType<TabControl>();
    WindowFactoryManager::addWindowType<Thumb>();
    WindowFactoryManager::addWindowType<Titlebar>();
    WindowFactoryManager::addWindowType<Tooltip>();
    WindowFactoryManager::addWindowType<ItemListbox>();
    WindowFactoryManager::addWindowType<GroupBox>();
    WindowFactoryManager::addWindowType<Tree>();
    WindowFactoryManager::addWindowType<LayoutCell>();
    WindowFactoryManager::addWindowType<HorizontalLayoutContainer>();
    WindowFactoryManager::addWindowType<VerticalLayoutContainer>();
    WindowFactoryManager::addWindowType<GridLayoutContainer>();
}

void System::createSingletons()
{
    // cause creation of other singleton objects
    CEGUI_NEW_AO ImageManager();
    CEGUI_NEW_AO FontManager();
    CEGUI_NEW_AO WindowFactoryManager();
    CEGUI_NEW_AO WindowManager();
    CEGUI_NEW_AO SchemeManager();
    CEGUI_NEW_AO GlobalEventSet();
    CEGUI_NEW_AO AnimationManager();
    CEGUI_NEW_AO WidgetLookManager();
    CEGUI_NEW_AO WindowRendererManager();
    CEGUI_NEW_AO RenderEffectManager();
}

void System::destroySingletons()
{
    CEGUI_DELETE_AO SchemeManager::getSingletonPtr();
    CEGUI_DELETE_AO WindowManager::getSingletonPtr();
    CEGUI_DELETE_AO WindowFactoryManager::getSingletonPtr();
    CEGUI_DELETE_AO WidgetLookManager::getSingletonPtr();
    CEGUI_DELETE_AO WindowRendererManager::getSingletonPtr();
    CEGUI_DELETE_AO AnimationManager::getSingletonPtr();
    CEGUI_DELETE_AO RenderEffectManager::getSingletonPtr();
    CEGUI_DELETE_AO FontManager::getSingletonPtr();
    CEGUI_DELETE_AO ImageManager::getSingletonPtr();
    CEGUI_DELETE_AO GlobalEventSet::getSingletonPtr();
}

//----------------------------------------------------------------------------//
void System::setupXMLParser()
{
    // handle creation / initialisation of XMLParser
    if (!d_xmlParser)
    {
#ifndef CEGUI_STATIC
        setXMLParser(d_defaultXMLParserName);
#else
        //Static Linking Call
        d_xmlParser = createParser();
        // make sure we know to cleanup afterwards.
        d_ourXmlParser = true;
		d_xmlParser->initialise();
#endif
    }
    // parser object already set, just initialise it.
    else
        d_xmlParser->initialise();
}

//----------------------------------------------------------------------------//
void System::cleanupXMLParser()
{
    // bail out if no parser
    if (!d_xmlParser)
        return;

    // get parser object to do whatever cleanup it needs to
    d_xmlParser->cleanup();

    // exit if we did not create this parser object
    if (!d_ourXmlParser)
        return;

    // if parser module loaded, destroy the parser object & cleanup module
    if (d_parserModule)
    {
        // get pointer to parser deletion function
        void(*deleteFunc)(XMLParser*) = (void(*)(XMLParser*))d_parserModule->
            getSymbolAddress("destroyParser");
        // cleanup the xml parser object
        deleteFunc(d_xmlParser);

        // delete the dynamic module for the xml parser
        CEGUI_DELETE_AO d_parserModule;
        d_parserModule = 0;
    }
#ifdef CEGUI_STATIC
    else
        //Static Linking Call
        destroyParser(d_xmlParser);
#endif

    d_xmlParser = 0;
}

//----------------------------------------------------------------------------//
void System::setXMLParser(const String& parserName)
{
#ifndef CEGUI_STATIC
    cleanupXMLParser();
    // load dynamic module
    d_parserModule = CEGUI_NEW_AO DynamicModule(String("CEGUI") + parserName);
    // get pointer to parser creation function
    XMLParser* (*createFunc)(void) =
        (XMLParser* (*)(void))d_parserModule->getSymbolAddress("createParser");
    // create the parser object
    d_xmlParser = createFunc();
    // make sure we know to cleanup afterwards.
    d_ourXmlParser = true;
    // perform initialisation of XML parser.
    d_xmlParser->initialise();
#else
    Logger::getSingleton().logEvent(
        "System::setXMLParser(const String& parserName) called from statically "
        "linked CEGUI library - unable to load dynamic module!", Errors);
#endif
}

//----------------------------------------------------------------------------//
void System::setXMLParser(XMLParser* parser)
{
    cleanupXMLParser();
    d_xmlParser = parser;
    d_ourXmlParser = false;
    setupXMLParser();
}

//----------------------------------------------------------------------------//
void System::setDefaultXMLParserName(const String& parserName)
{
    d_defaultXMLParserName = parserName;
}

const String System::getDefaultXMLParserName()
{
    return d_defaultXMLParserName;
}

//----------------------------------------------------------------------------//
ImageCodec& System::getImageCodec() const
{
    return *d_imageCodec;
}

//----------------------------------------------------------------------------//
void System::setImageCodec(const String& codecName)
{
    setupImageCodec(codecName);
}

//----------------------------------------------------------------------------//
void System::setImageCodec(ImageCodec& codec)
{
    cleanupImageCodec();
    d_imageCodec = &codec;
    d_ourImageCodec = false;
    d_imageCodecModule = 0;
}

//----------------------------------------------------------------------------//
void System::setupImageCodec(const String& codecName)
{
    // Cleanup the old image codec
    cleanupImageCodec();

    #if defined(CEGUI_STATIC)
        // for static build use static createImageCodec to create codec object
        d_imageCodec = createImageCodec();
    #else
        // load the appropriate image codec module
        d_imageCodecModule = codecName.empty() ?
            new DynamicModule(String("CEGUI") + d_defaultImageCodecName) :
            new DynamicModule(String("CEGUI") + codecName);

        // use function from module to create the codec object.
        d_imageCodec = ((ImageCodec*(*)(void))d_imageCodecModule->
            getSymbolAddress("createImageCodec"))();
    #endif

    // make sure we mark this as our own object so we can clean it up later.
    d_ourImageCodec = true;
}

//----------------------------------------------------------------------------//
void System::cleanupImageCodec()
{
    // bail out if no codec, or if we did not create it.
    if (!d_imageCodec || !d_ourImageCodec)
        return;

    if (d_imageCodecModule)
    {
        ((void(*)(ImageCodec*))d_imageCodecModule->
            getSymbolAddress("destroyImageCodec"))(d_imageCodec);

        CEGUI_DELETE_AO d_imageCodecModule;
        d_imageCodecModule = 0;
    }
#if defined(CEGUI_STATIC)
    else
        destroyImageCodec(d_imageCodec);
#endif

    d_imageCodec = 0;
}

//----------------------------------------------------------------------------//
void System::setDefaultImageCodecName(const String& codecName)
{
    d_defaultImageCodecName = codecName;
}

//----------------------------------------------------------------------------//
const String& System::getDefaultImageCodecName()
{
    return d_defaultImageCodecName;
}

//----------------------------------------------------------------------------//
System& System::create(Renderer& renderer, ResourceProvider* resourceProvider,
                       XMLParser* xmlParser, ImageCodec* imageCodec,
                       ScriptModule* scriptModule, const String& configFile,
                       const String& logFile)
{
    return *CEGUI_NEW_AO System(renderer, resourceProvider, xmlParser, imageCodec,
                       scriptModule, configFile, logFile);
}

//----------------------------------------------------------------------------//
void System::destroy()
{
    CEGUI_DELETE_AO System::getSingletonPtr();
}

//----------------------------------------------------------------------------//
RenderedStringParser* System::getDefaultCustomRenderedStringParser() const
{
    return d_customRenderedStringParser;
}

//----------------------------------------------------------------------------//
void System::setDefaultCustomRenderedStringParser(RenderedStringParser* parser)
{
    if (parser != d_customRenderedStringParser)
    {
        d_customRenderedStringParser = parser;
        
        // fire event
        EventArgs args;
        fireEvent(EventRenderedStringParserChanged, args, EventNamespace);
    }
}

//----------------------------------------------------------------------------//
bool System::injectMouseButtonClick(const MouseButton button)
{
    return d_defaultGUIContext->injectMouseButtonClick(button);
}

//----------------------------------------------------------------------------//
bool System::injectMouseButtonDoubleClick(const MouseButton button)
{
    return d_defaultGUIContext->injectMouseButtonDoubleClick(button);
}

//----------------------------------------------------------------------------//
bool System::injectMouseButtonTripleClick(const MouseButton button)
{
    return d_defaultGUIContext->injectMouseButtonTripleClick(button);
}

//----------------------------------------------------------------------------//
bool System::injectCopyRequest()
{
    return d_defaultGUIContext->injectCopyRequest();
}

//----------------------------------------------------------------------------//
bool System::injectCutRequest()
{
    return d_defaultGUIContext->injectCutRequest();
}

//----------------------------------------------------------------------------//
bool System::injectPasteRequest()
{
    return d_defaultGUIContext->injectPasteRequest();
}

//----------------------------------------------------------------------------//
void System::invalidateAllCachedRendering()
{
    invalidateAllWindows();
    //MouseCursor::getSingleton().invalidate();
}

//----------------------------------------------------------------------------//
void System::invalidateAllWindows()
{
    WindowManager::WindowIterator wi(
        WindowManager::getSingleton().getIterator());

    for ( ; !wi.isAtEnd(); ++wi)
    {
        Window* const wnd(wi.getCurrentValue());
        // invalidate window itself
        wnd->invalidate();
        // if window has rendering window surface, invalidate it's geometry
        RenderingSurface* rs;
        if ((rs = wnd->getRenderingSurface()) && rs->isRenderingWindow())
            static_cast<RenderingWindow*>(rs)->invalidateGeometry();
    }
}

//----------------------------------------------------------------------------//
RegexMatcher& System::createRegexMatcher() const
{
#ifdef CEGUI_HAS_PCRE_REGEX
    return *CEGUI_NEW_AO PCRERegexMatcher();
#else
    CEGUI_THROW(InvalidRequestException("System::createRegexMatcher - "
        "Library was built without support for regular expressions."));
#endif
}

//----------------------------------------------------------------------------//
void System::destroyRegexMatcher(RegexMatcher& rm) const
{
    CEGUI_DELETE_AO &rm;
}

//----------------------------------------------------------------------------//
GUIContext& System::getDefaultGUIContext() const
{
    return *d_defaultGUIContext;
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section

