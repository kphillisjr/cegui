/************************************************************************
	filename: 	CEGUIPropertySet.cpp
	created:	21/2/2004
	author:		Paul D Turner
	
	purpose:	Implements PropertySet class
*************************************************************************/
/*************************************************************************
    Crazy Eddie's GUI System (http://crayzedsgui.sourceforge.net)
    Copyright (C)2004 Paul D Turner (crayzed@users.sourceforge.net)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*************************************************************************/
#include "CEGUIPropertySet.h"
#include "CEGUIProperty.h"
#include "CEGUIExceptions.h"

// Start of CEGUI namespace section
namespace CEGUI
{

/*************************************************************************
	Add a new property to the set
*************************************************************************/
void PropertySet::addProperty(Property* property)
{
	if (property == NULL)
	{
		throw NullObjectException((utf8*)"The given Property object pointer is NULL.");
	}

	if (d_properties.find(property->getName()) != d_properties.end())
	{
		throw AlreadyExistsException((utf8*)"A Property named '" + property->getName() + (utf8*)"' already exists in the PropertySet.");
	}

	d_properties[property->getName()] = property;
}

/*************************************************************************
	Remove a property from the set
*************************************************************************/
void PropertySet::removeProperty(const String& name)
{
	PropertyRegistry::iterator pos = d_properties.find(name);

	if (pos != d_properties.end())
	{
		d_properties.erase(pos);
	}
}

/*************************************************************************
	Remove all properties from the set
*************************************************************************/
void PropertySet::clearProperties(void)
{
	d_properties.clear();
}

/*************************************************************************
	Return true if a property with the given name is in the set
*************************************************************************/
bool PropertySet::isPropertyPresent(const String& name) const
{
	return (d_properties.find(name) != d_properties.end());
}

/*************************************************************************
	Return the help string for a property
*************************************************************************/
const String& PropertySet::getPropertyHelp(const String& name) const
{
	PropertyRegistry::const_iterator pos = d_properties.find(name);

	if (pos == d_properties.end())
	{
		throw UnknownObjectException((utf8*)"There is no Property named '" + name + (utf8*)"' available in the set.");
	}

	return pos->second->getHelp();
}

/*************************************************************************
	Return the current value of a property
*************************************************************************/
String PropertySet::getProperty(const String& name) const
{
	PropertyRegistry::const_iterator pos = d_properties.find(name);

	if (pos == d_properties.end())
	{
		throw UnknownObjectException((utf8*)"There is no Property named '" + name + (utf8*)"' available in the set.");
	}

	return pos->second->get();
}

/*************************************************************************
	Set the current value of a property
*************************************************************************/
void PropertySet::setProperty(const String& name,const String& value)
{
	PropertyRegistry::iterator pos = d_properties.find(name);

	if (pos == d_properties.end())
	{
		throw UnknownObjectException((utf8*)"There is no Property named '" + name + (utf8*)"' available in the set.");
	}

	pos->second->set(value);
}

} // End of  CEGUI namespace section
