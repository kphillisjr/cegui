/***********************************************************************
    filename:   Win32ClipboardProvider.cpp
    created:    19/09/2013
    author:     Alexander Gladis

    purpose:    Implement a clipboard provider for Windows
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2013 Paul D Turner & The CEGUI Development Team
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
#include <Windows.h>
#include "CEGUI/CEGUI.h"
#include "CEGUI/Win32ClipboardProvider.h"

namespace CEGUI
{

Win32ClipboardProvider::Win32ClipboardProvider() :
    d_buffer(0),
    d_bufferSize(0)
{
}

Win32ClipboardProvider::~Win32ClipboardProvider()
{
   deallocateBuffer();
}

void Win32ClipboardProvider::allocateBuffer(size_t size)
{
   if(d_buffer)
      delete [] d_buffer;

   d_buffer = new char[size];
   d_bufferSize = size;
}

void Win32ClipboardProvider::deallocateBuffer()
{
   delete [] d_buffer;
   d_buffer = 0;
   d_bufferSize = 0;
}

void Win32ClipboardProvider::sendToClipboard(const String& mime_type, void* buffer, size_t size)
{
   if(mime_type == "text/plain")
   {
      if(OpenClipboard(0))
      {
         // Transcode buffer to UTF-16
#if CEGUI_STRING_CLASS == CEGUI_STRING_CLASS_UNICODE
         String str(static_cast<const utf8*>(buffer), size);
#else
         String str(static_cast<const char*>(buffer), size);
#endif
         uint16* str_utf16 = System::getSingleton().getStringTranscoder().stringToUTF16(str);
         size_t size_in_bytes = (str.size() + 1) * sizeof(uint16);

         // Copy to clipboard
         EmptyClipboard();
         HGLOBAL clipboard_data = GlobalAlloc(GMEM_DDESHARE,size_in_bytes);
         LPWSTR clipboard = static_cast<LPWSTR>(GlobalLock(clipboard_data));
         if(clipboard)
            memcpy(clipboard, str_utf16, size_in_bytes);
         GlobalUnlock(clipboard_data);
         SetClipboardData(CF_UNICODETEXT, clipboard_data);
         CloseClipboard();

         // Free temporary UTF-16 buffer
         System::getSingleton().getStringTranscoder().deleteUTF16Buffer(str_utf16);
      }
   }
}

void Win32ClipboardProvider::retrieveFromClipboard(String& mime_type, void*& buffer, size_t& size)
{
   if(OpenClipboard(0))
   {
      // Open & read UTF-16 clipboard data
      HGLOBAL clipboard_data = GetClipboardData(CF_UNICODETEXT);
      const uint16* clipboard = static_cast<const uint16*>(GlobalLock(clipboard_data));
      if(clipboard)
      {
         // Transcode UTF-16 to native format and copy to local buffer
         String str = System::getSingleton().getStringTranscoder().stringFromUTF16(clipboard);
         allocateBuffer(strlen(str.c_str())); // We need the actual byte count which can be different from str.size() when using UTF-8!
         memcpy(d_buffer, str.c_str(), d_bufferSize);

         mime_type = "text/plain";
         buffer = d_buffer;
         size = d_bufferSize;
      }

      // Close clipboard
      GlobalUnlock(clipboard_data);
      CloseClipboard();
   }
}

}
