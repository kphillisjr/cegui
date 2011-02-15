// This file has been generated by Py++.

#include "boost/python.hpp"
#include "python_CEGUI.h"
#include "Size.pypp.hpp"

namespace bp = boost::python;

void register_Size_class(){

    { //::CEGUI::Size< float >
        typedef bp::class_< CEGUI::Size< float > > Size_exposer_t;
        Size_exposer_t Size_exposer = Size_exposer_t( "Size", bp::init< >() );
        bp::scope Size_scope( Size_exposer );
        Size_exposer.def( bp::init< float const &, float const & >(( bp::arg("width"), bp::arg("height") )) );
        Size_exposer.def( bp::init< CEGUI::Size< float > const & >(( bp::arg("v") )) );
        Size_exposer.def( bp::self != bp::self );
        Size_exposer.def( bp::self * bp::other< float >() );
        Size_exposer.def( bp::self + bp::self );
        Size_exposer.def( bp::self == bp::self );
        Size_exposer.def_readwrite( "d_height", &CEGUI::Size< float >::d_height );
        Size_exposer.def_readwrite( "d_width", &CEGUI::Size< float >::d_width );
    }

}
