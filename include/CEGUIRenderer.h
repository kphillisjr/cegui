/************************************************************************
	filename: 	CEGUIRenderer.h
	created:	20/2/2004
	author:		Paul D Turner
	
	purpose:	Defines interface for abstract Renderer class
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
#ifndef _CEGUIRenderer_h_
#define _CEGUIRenderer_h_

#include "CEGUIBase.h"
#include "CEGUIString.h"
#include "CEGUIRect.h"
#include "CEGUIColourRect.h"
#include "CEGUIEventSet.h"


// Start of CEGUI namespace section
namespace CEGUI
{
/*!
\brief
	Abstract class defining the interface for Renderer objects.

	Objects derived from Renderer are the means by which the GUI system interfaces
	with specific rendering technologies.  To use a rendering system or API to draw
	CEGUI imagery requires that an appropriate Renderer object be available.
*/
class CEGUIBASE_API Renderer : public EventSet
{
public:
	/*************************************************************************
		Event name constants
	*************************************************************************/
	/*!
		event that fires when the underlying display mode had changed.
		
		It is important that all Renderer implementers fire this properly as the
		system itself subscribes to this event.
	*/
	static const utf8	ModeChangedEvent[];


	/*************************************************************************
		Abstract interface methods
	*************************************************************************/
	/*!
	\brief
		Add a quad to the rendering queue.  All clipping and other adjustments should have been made prior to calling this.

	\param dest_rect
		Rect object describing the destination area (values are in pixels)

	\param z
		float value specifying the z co-ordinate / z order of the quad

	\param tex
		pointer to the Texture object that holds the imagery to be rendered

	\param texture_rect
		Rect object holding the area of \a tex that is to be rendered (values are in texture co-ordinates).

	\param colours
		ColourRect object describing the colour values that are to be applied when rendering.

	\return
		Nothing
	*/
	virtual	void	addQuad(const Rect& dest_rect, float z, const Texture* tex, const Rect& texture_rect, const ColourRect& colours) = 0;


	/*!
	\brief
		Perform final rendering for all quads that have been queued for rendering

		The contents of the rendering queue is retained and can be rendered again as required.  If the contents is not required call clearRenderList().

	\return
		Nothing
	*/
	virtual	void	doRender(void) = 0;


	/*!
	\brief
		Clears all queued quads from the render queue.
	
	\return
		Nothing
	*/
	virtual	void	clearRenderList(void) = 0;


	/*!
	\brief
		Creates a 'null' Texture object.

	\return
		a newly created Texture object.  The returned Texture object has no size or imagery associated with it, and is
		generally of little or no use.
	*/
	virtual	Texture*	createTexture(void) = 0;


	/*!
	\brief
		Create a Texture object using the given image file.

	\param filename
		String object that specifies the path and filename of the image file to use when creating the texture.

	\return
		a newly created Texture object.  The initial contents of the texture memory is the requested image file.

	\note
		Textures are always created with a size that is a power of 2.  If the file you specify is of a size that is not
		a power of two, the final size will be rounded up.  Additionally, textures are always square, so the ultimate
		size is governed by the larger of the width and height of the specified file.  You can check the ultimate sizes
		by querying the texture after creation.
	*/
	virtual	Texture*	createTexture(const String& filename) = 0;


	/*!
	\brief
		Create a Texture object with the given pixel dimensions as specified by \a size.  NB: Textures are always square.

	\param size
		float value that specifies the size to use for the width and height when creating the new texture.

	\return
		a newly created Texture object.  The initial contents of the texture memory is undefined / random.

	\note
		Textures are always created with a size that is a power of 2.  If you specify a size that is not a power of two, the final
		size will be rounded up.  So if you specify a size of 1024, the texture will be (1024 x 1024), however, if you specify a size
		of 1025, the texture will be (2048 x 2048).  You can check the ultimate size by querying the texture after creation.
	*/	
	virtual	Texture*	createTexture(float size) = 0;


	/*!
	\brief
		Destroy the given Texture object.

	\param texture
		pointer to the Texture object to be destroyed

	\return
		Nothing
	*/
	virtual	void		destroyTexture(Texture* texture) = 0;

	
	/*!
	\brief
		Destroy all Texture objects.

	\return
		Nothing
	*/
	virtual void		destroyAllTextures(void) = 0;


protected:
	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	Renderer(void);
	virtual ~Renderer(void);
};

} // End of  CEGUI namespace section


#endif	// end of guard _CEGUIRenderer_h_
