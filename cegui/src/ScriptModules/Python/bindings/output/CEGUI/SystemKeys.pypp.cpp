// This file has been generated by Py++.

#include "boost/python.hpp"
#include "generators/include/python_CEGUI.h"
#include "SystemKeys.pypp.hpp"

namespace bp = boost::python;

void register_SystemKeys_class(){

    { //::CEGUI::SystemKeys
        typedef bp::class_< CEGUI::SystemKeys > SystemKeys_exposer_t;
        SystemKeys_exposer_t SystemKeys_exposer = SystemKeys_exposer_t( "SystemKeys", bp::init< >() );
        bp::scope SystemKeys_scope( SystemKeys_exposer );
        bp::enum_< CEGUI::SystemKeys::SystemKey>("SystemKey")
            .value("None", CEGUI::SystemKeys::None)
            .value("LeftMouse", CEGUI::SystemKeys::LeftMouse)
            .value("RightMouse", CEGUI::SystemKeys::RightMouse)
            .value("Shift", CEGUI::SystemKeys::Shift)
            .value("Control", CEGUI::SystemKeys::Control)
            .value("MiddleMouse", CEGUI::SystemKeys::MiddleMouse)
            .value("X1Mouse", CEGUI::SystemKeys::X1Mouse)
            .value("X2Mouse", CEGUI::SystemKeys::X2Mouse)
            .value("Alt", CEGUI::SystemKeys::Alt)
            .export_values()
            ;
        { //::CEGUI::SystemKeys::get
        
            typedef ::CEGUI::uint ( ::CEGUI::SystemKeys::*get_function_type )(  ) const;
            
            SystemKeys_exposer.def( 
                "get"
                , get_function_type( &::CEGUI::SystemKeys::get ) );
        
        }
        { //::CEGUI::SystemKeys::isPressed
        
            typedef bool ( ::CEGUI::SystemKeys::*isPressed_function_type )( ::CEGUI::SystemKeys::SystemKey ) const;
            
            SystemKeys_exposer.def( 
                "isPressed"
                , isPressed_function_type( &::CEGUI::SystemKeys::isPressed )
                , ( bp::arg("key") ) );
        
        }
        { //::CEGUI::SystemKeys::keyCodeToSystemKey
        
            typedef ::CEGUI::SystemKeys::SystemKey ( *keyCodeToSystemKey_function_type )( ::CEGUI::Key::Scan );
            
            SystemKeys_exposer.def( 
                "keyCodeToSystemKey"
                , keyCodeToSystemKey_function_type( &::CEGUI::SystemKeys::keyCodeToSystemKey )
                , ( bp::arg("key") ) );
        
        }
        { //::CEGUI::SystemKeys::keyPressed
        
            typedef void ( ::CEGUI::SystemKeys::*keyPressed_function_type )( ::CEGUI::Key::Scan ) ;
            
            SystemKeys_exposer.def( 
                "keyPressed"
                , keyPressed_function_type( &::CEGUI::SystemKeys::keyPressed )
                , ( bp::arg("key") )
                , "! notify that the given key was presed\n" );
        
        }
        { //::CEGUI::SystemKeys::keyReleased
        
            typedef void ( ::CEGUI::SystemKeys::*keyReleased_function_type )( ::CEGUI::Key::Scan ) ;
            
            SystemKeys_exposer.def( 
                "keyReleased"
                , keyReleased_function_type( &::CEGUI::SystemKeys::keyReleased )
                , ( bp::arg("key") )
                , "! notify that the given key was presed\n\
            ! notify that the given key was released.\n" );
        
        }
        { //::CEGUI::SystemKeys::mouseButtonPressed
        
            typedef void ( ::CEGUI::SystemKeys::*mouseButtonPressed_function_type )( ::CEGUI::MouseButton ) ;
            
            SystemKeys_exposer.def( 
                "mouseButtonPressed"
                , mouseButtonPressed_function_type( &::CEGUI::SystemKeys::mouseButtonPressed )
                , ( bp::arg("button") )
                , "! notify that the given key was released.\n\
            ! notify that the given mouse button was pressed.\n" );
        
        }
        { //::CEGUI::SystemKeys::mouseButtonReleased
        
            typedef void ( ::CEGUI::SystemKeys::*mouseButtonReleased_function_type )( ::CEGUI::MouseButton ) ;
            
            SystemKeys_exposer.def( 
                "mouseButtonReleased"
                , mouseButtonReleased_function_type( &::CEGUI::SystemKeys::mouseButtonReleased )
                , ( bp::arg("button") )
                , "! notify that the given mouse button was pressed.\n\
            ! notify that the given mouse button was released.\n" );
        
        }
        { //::CEGUI::SystemKeys::mouseButtonToSystemKey
        
            typedef ::CEGUI::SystemKeys::SystemKey ( *mouseButtonToSystemKey_function_type )( ::CEGUI::MouseButton );
            
            SystemKeys_exposer.def( 
                "mouseButtonToSystemKey"
                , mouseButtonToSystemKey_function_type( &::CEGUI::SystemKeys::mouseButtonToSystemKey )
                , ( bp::arg("button") ) );
        
        }
        { //::CEGUI::SystemKeys::reset
        
            typedef void ( ::CEGUI::SystemKeys::*reset_function_type )(  ) ;
            
            SystemKeys_exposer.def( 
                "reset"
                , reset_function_type( &::CEGUI::SystemKeys::reset ) );
        
        }
        SystemKeys_exposer.staticmethod( "keyCodeToSystemKey" );
        SystemKeys_exposer.staticmethod( "mouseButtonToSystemKey" );
    }

}
