/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  OpenCPN Image preview widget
 * Author:   Pavel Saviankou (taken and adopted from 
 * http://wiki.wxwidgets.org/An_image_panel)
 *
 ***************************************************************************
 *   Copyright (C) 2010 by David S. Register                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.         *
 ***************************************************************************
 *
 *
 */
#include <iostream>
#include <vector>
using namespace std;


#ifndef __IMAGEPREVIEW_H__
#define __IMAGEPREVIEW_H__

#include <wx/print.h>
#include <wx/datetime.h>
#include <wx/cmdline.h>
#include <wx/string.h>
#ifdef __WXMSW__
#include <wx/msw/private.h>
#endif



class wxImagePanel : public wxPanel
{
    wxImage image;
    wxBitmap resized;
    int w, h;
    
public:
    wxImagePanel(wxWindow* parent);
    
    void paintEvent(wxPaintEvent & evt);
    void paintNow();
    void OnSize(wxSizeEvent& event);
    void render(wxDC& dc);
    void loadImage( wxString file, wxBitmapType format);
    
    // some useful events
    /*
     void mouseMoved(wxMouseEvent& event);
     void mouseDown(wxMouseEvent& event);
     void mouseWheelMoved(wxMouseEvent& event);
     void mouseReleased(wxMouseEvent& event);
     void rightClick(wxMouseEvent& event);
     void mouseLeftWindow(wxMouseEvent& event);
     void keyPressed(wxKeyEvent& event);
     void keyReleased(wxKeyEvent& event);
     */
    
    DECLARE_EVENT_TABLE()
};


#endif
