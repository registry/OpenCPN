/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  OpenCPN Url Download class
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


#ifndef __URL_DOWNLOAD_H__
#define __URL_DOWNLOAD_H__

#include <wx/print.h>
#include <wx/datetime.h>
#include <wx/cmdline.h>
#include <wx/string.h>
#ifdef __WXMSW__
#include <wx/msw/private.h>
#endif



class UrlDownloader 
{
    
public:
    static
    wxImage LoadImage( wxString url );
};


#endif
