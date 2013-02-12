/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  OpenCPN Url Download class 
 * Author:   Pavel Saviankou (taken and adopted from 
 * http://wiki.wxwidgets.org/An_image_panel)
 *
 ***************************************************************************
 *   Copyright (C) 2012 by David S. Register                               *
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
 */
#include <iostream>
#include <sstream>
#include <vector>
using namespace std;

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers
#ifdef __WXMSW__
//#include "c:\\Program Files\\visual leak detector\\include\\vld.h"
#endif

#include "wx/print.h"
#include "wx/printdlg.h"
#include "wx/artprov.h"
#include "wx/stdpaths.h"
#include <wx/intl.h>
#include <wx/listctrl.h>
#include <wx/aui/aui.h>
#include <wx/dialog.h>
#include <wx/progdlg.h>
#include <wx/brush.h>
#include <wx/colour.h>
#include <wx/tokenzr.h>
#include <wx/url.h>
#include <wx/mstream.h>

#define BUFSIZE 0x10000

#if wxCHECK_VERSION( 2, 9, 0 )
#include <wx/dialog.h>
#else
//  #include "scrollingdialog.h"
#endif

#include "dychart.h"

#ifdef __WXMSW__
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <psapi.h>
#endif

#ifndef __WXMSW__
#include <signal.h>
#include <setjmp.h>
#endif

#include "urldownloader.h"

 
 /* Called to load an url
  * 
  */
wxImage UrlDownloader::LoadImage( wxString _url )
{
    // load the file... ideally add a check to see if loading was successful
    wxURL url(_url);
    wxImage _image; 
   if(url.GetError()==wxURL_NOERR)
   {
        wxInputStream *in = url.GetInputStream();
        wxMemoryBuffer buf;
        if(in && in->IsOk())
        {
            unsigned char tempbuf[BUFSIZE];


            size_t total_len = in->GetSize();
            size_t data_loaded = 0;
            bool abort = false;

            wxProgressDialog progress( wxT("Downloading..."), wxT("Download in progress"), total_len, NULL, wxPD_CAN_ABORT | wxPD_AUTO_HIDE | wxPD_APP_MODAL);
            while( in->CanRead() && !in->Eof() && !abort )
            {
                in->Read(tempbuf, BUFSIZE);
                size_t readlen = in->LastRead();
                if( readlen>0 )   
                {
                buf.AppendData(tempbuf, readlen);
                data_loaded += readlen;
                }

                if( total_len>0 )
                {
                // if we know the length of the file, display correct progress
                wxString msg;
                msg.Printf( wxT("Downloaded %ld of %ld bytes"), data_loaded, total_len);
                abort = !progress.Update( data_loaded, msg );
                }
                else
                {
                // if we don't know the length of the file, just Pulse
                abort = !progress.Pulse();
                }
            }

            if( abort )
            {
                wxLogMessage( wxT("Download was cancelled.") );
            }
            else
            {
                // wxMemoryBuffer buf now contains the downloaded data
                wxLogMessage( wxT("Downloaded %ld bytes"), buf.GetDataLen());
        }
        wxMemoryInputStream mis(buf.GetData(), buf.GetBufSize()); 
        delete in;
        _image.LoadFile(mis);
        }
    }
    return _image;
}
 
 