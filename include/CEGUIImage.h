/************************************************************************
	filename: 	CEGUIImage.h
	created:	13/3/2004
	author:		Paul D Turner
	
	purpose:	Defines interface for Image class
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
#ifndef _CEGUIImage_h_
#define _CEGUIImage_h_

#include "CEGUIBase.h"
#include "CEGUIString.h"
#include "CEGUIRect.h"
#include "CEGUIColourRect.h"
#include "CEGUIVector.h"
#include "CEGUISize.h"
#include <map>


// Start of CEGUI namespace section
namespace CEGUI
{
/*!
\brief
	Class that represents a single Image of an Imageset.
*/
class CEGUIBASE_API Image
{
public:
	/*!
	\brief
		Return a Size object containing the dimensions of the Image.

	\return
		Size object holding the width and height of the Image.
	*/
	Size	getSize(void) const			{return Size(d_area.getWidth(), d_area.getHeight());}
	

	/*!
	\brief
		Return the pixel width of the image.

	\return
		Width of this Image in pixels.
	*/
	float	getWidth(void) const		{return d_area.getWidth();}


	/*!
	\brief
		Return the pixel height of the image.

	\return
		Height of this Image in pixels
	*/
	float	getHeight(void) const		{return d_area.getHeight();}


	/*!
	\brief
		Return a Point object that contains the offset applied when rendering this Image

	\return
		Point object containing the offsets applied when rendering this Image
	*/
	Point	getOffsets(void) const		{return d_offset;}


	/*!
	\brief
		Return the X rendering offset

	\return
		X rendering offset.  This is the number of pixels that the image is offset by when rendering at any given location.
	*/
	float	getOffsetX(void) const		{return	d_offset.d_x;}


	/*!
	\brief
		Return the Y rendering offset

	\return
		Y rendering offset.  This is the number of pixels that the image is offset by when rendering at any given location.
	*/
	float	getOffsetY(void) const		{return d_offset.d_y;}


	/*!
	\brief
		Queue the image to be drawn. 
		
	\note
		The final position of the Image will be adjusted by the offset values defined for this Image object.  If absolute positioning is
		essential then these values should be taken into account prior to calling the draw() methods.  However, by doing this you take
		away the ability of the Imageset designer to adjust the alignment and positioning of Images, therefore your component is far
		less useful since it requires code changes to modify image positioning that could have been handled from a data file.

	\param position
		Vector3 object containing the location where the Image is to be drawn

	\param size
		Size object describing the size that the Image is to be drawn at.

	\param clip_rect
		Rect object that defines an on-screen area that the Image will be clipped to when drawing.

	\param top_left_colour
		Colour (as 0xAARRGGBB value) to be applied to the top-left corner of the Image.

	\param top_right_colour
		Colour (as 0xAARRGGBB value) to be applied to the top-right corner of the Image.

	\param bottom_left_colour
		Colour (as 0xAARRGGBB value) to be applied to the bottom-left corner of the Image.

	\param bottom_right_colour
		Colour (as 0xAARRGGBB value) to be applied to the bottom-right corner of the Image.

	\return
		Nothing
	*/
	void	draw(const Vector3& position, const Size& size, const Rect& clip_rect, colour top_left_colour = -1, colour top_right_colour = -1, colour bottom_left_colour = -1, colour bottom_right_colour = -1) const
	{
		draw(Rect(position.d_x, position.d_y, position.d_x + size.d_width, position.d_y + size.d_height), position.d_z, clip_rect, 
			ColourRect(top_left_colour, top_right_colour, bottom_left_colour, bottom_right_colour));
	}


	/*!
	\brief
		Queue the image to be drawn.

	\note
		The final position of the Image will be adjusted by the offset values defined for this Image object.  If absolute positioning is
		essential then these values should be taken into account prior to calling the draw() methods.  However, by doing this you take
		away the ability of the Imageset designer to adjust the alignment and positioning of Images, therefore your component is far
		less useful since it requires code changes to modify image positioning that could have been handled from a data file.

	\param dest_rect
		Rect object defining the area on-screen where the Image is to be drawn.  The Image will be scaled to fit the area as required.

	\param z
		Z-order position for the image.  Positions increase "into the screen", so 0.0f is at the top of the z-order.

	\param clip_rect
		Rect object that defines an on-screen area that the Image will be clipped to when drawing.

	\param top_left_colour
		Colour (as 0xAARRGGBB value) to be applied to the top-left corner of the Image.

	\param top_right_colour
		Colour (as 0xAARRGGBB value) to be applied to the top-right corner of the Image.

	\param bottom_left_colour
		Colour (as 0xAARRGGBB value) to be applied to the bottom-left corner of the Image.

	\param bottom_right_colour
		Colour (as 0xAARRGGBB value) to be applied to the bottom-right corner of the Image.

	\return
		Nothing
	*/
	void	draw(const Rect& dest_rect, float z, const Rect& clip_rect, colour top_left_colour = -1, colour top_right_colour = -1, colour bottom_left_colour = -1, colour bottom_right_colour = -1) const
	{
		draw(dest_rect, z, clip_rect, ColourRect(top_left_colour, top_right_colour, bottom_left_colour, bottom_right_colour));
	}


	/*!
	\brief
		Queue the image to be drawn.

	\note
		The final position of the Image will be adjusted by the offset values defined for this Image object.  If absolute positioning is
		essential then these values should be taken into account prior to calling the draw() methods.  However, by doing this you take
		away the ability of the Imageset designer to adjust the alignment and positioning of Images, therefore your component is far
		less useful since it requires code changes to modify image positioning that could have been handled from a data file.

	\param position
		Vector3 object containing the location where the Image is to be drawn

	\note
		The image will be drawn at it's internally defined size.

	\param clip_rect
		Rect object that defines an on-screen area that the Image will be clipped to when drawing.

	\param colours
		ColourRect object that describes the colour values to use for each corner of the Image.

	\return
		Nothing
	*/
	void	draw(const Vector3& position, const Rect& clip_rect, const ColourRect& colours) const
	{
		draw(Rect(position.d_x, position.d_y, position.d_x + getWidth(), position.d_y + getHeight()), position.d_z, clip_rect, colours);
	}


	/*!
	\brief
		Queue the image to be drawn.

	\note
		The final position of the Image will be adjusted by the offset values defined for this Image object.  If absolute positioning is
		essential then these values should be taken into account prior to calling the draw() methods.  However, by doing this you take
		away the ability of the Imageset designer to adjust the alignment and positioning of Images, therefore your component is far
		less useful since it requires code changes to modify image positioning that could have been handled from a data file.

	\param dest_rect
		Rect object defining the area on-screen where the Image is to be drawn.  The Image will be scaled to fit the area as required.

	\param z
		Z-order position for the image.  Positions increase "into the screen", so 0.0f is at the top of the z-order.

	\param clip_rect
		Rect object that defines an on-screen area that the Image will be clipped to when drawing.

	\param colours
		ColourRect object that describes the colour values to use for each corner of the Image.

	\return
		Nothing
	*/
	void	draw(const Rect& dest_rect, float z, const Rect& clip_rect,const ColourRect& colours) const;


private:
	/*************************************************************************
		Friends (allows us to ensure only the GUI system creates Image objects)
	*************************************************************************/
	friend class Imageset;
	friend class std::map;
	friend struct std::pair<const String, Image>;


	/*************************************************************************
		Implementation Methods
	*************************************************************************/
	/*!
	\brief
		Default constructor (only used by std::map)
	*/
	Image(void) {}


	/*!
	\brief
		Constructor for Image objects.  This is not normally used directly by client code, use the Imageset interface instead.

	\param owner
		Pointer to a Imageset object that owns this Image.  This must not be NULL.

	\exception NullObjectException	Thrown if \a owner was NULL.
	*/
	Image(const Imageset* owner, const Rect& area, const Point& render_offset);


	/*!
	\brief
		Copy constructor
	*/
	Image(const Image& image);


	/*!
	\brief
		Destructor for Image objects.
	*/
	virtual ~Image(void);


	/*************************************************************************
		Implementation Data
	*************************************************************************/
	const Imageset*	d_owner;		//!< Link back to Imageset that owns this image
	Rect			d_area;			//!< Rect defining the area on the texture that makes up this image.
	Point			d_offset;		//!< Offset to use when rendering
};

} // End of  CEGUI namespace section


#endif	// end of guard _CEGUIImage_h_
