// This file has been generated by Py++.

#include "boost/python.hpp"
#include "python_CEGUI.h"
#include "Sizef.pypp.hpp"

namespace bp = boost::python;

void register_Sizef_class(){

    { //::CEGUI::Size< float >
        typedef bp::class_< CEGUI::Size< float > > Sizef_exposer_t;
        Sizef_exposer_t Sizef_exposer = Sizef_exposer_t( "Sizef", bp::init< >() );
        bp::scope Sizef_scope( Sizef_exposer );
        Sizef_exposer.def( bp::init< float const &, float const & >(( bp::arg("width"), bp::arg("height") )) );
        Sizef_exposer.def( bp::init< CEGUI::Size< float > const & >(( bp::arg("v") )) );
        Sizef_exposer.def( bp::self != bp::self );
        Sizef_exposer.def( bp::self * bp::other< float >() );
        Sizef_exposer.def( bp::self + bp::self );
        Sizef_exposer.def( bp::self == bp::self );
        Sizef_exposer.def_readwrite( "d_height", &CEGUI::Size< float >::d_height );
        Sizef_exposer.def_readwrite( "d_width", &CEGUI::Size< float >::d_width );
    }

}