/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  OpenCPN Route printout
 * Author:   Pavel Saviankou
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

#include "routeprintout.h"
#include "printtable.h"
#include "urldownloader.h"

#define PRINT_WP_NAME 0
#define PRINT_WP_POSITION 1
#define PRINT_WP_COURSE 2
#define PRINT_WP_DISTANCE 3
#define PRINT_WP_DESCRIPTION 4

#define PRINT_WP_IMAGE_APPROACH 5

// Global print data, to remember settings during the session
extern wxPrintData*     g_printData;
// Global page setup data
extern wxPageSetupData* g_pageSetupData;

MyRoutePrintout::MyRoutePrintout( std::vector<bool> _toPrintOut,
                                  Route*            route,
                                  const wxChar*     title
                                  ) : MyPrintout( title ),
                                      myRoute( route ),
                                      toPrintOut( _toPrintOut )
{
    // Let's have at least some device units margin
    marginX = 5;
    marginY = 5;

    // Offset text from the edge of the cell (Needed on Linux)
    textOffsetX = 5;
    textOffsetY = 8;
    wxString _title = _T("From ") + route->GetRouteStart() + _T(" to ") + route->GetRouteEnd(); 
    table.SetTableTitle( _title );
    table.StartFillHeader();
    // setup widths for columns
    if ( toPrintOut[ PRINT_WP_NAME ] ) {
        table << (const char *)wxString(_("Name")).mb_str();
    }
    if ( toPrintOut[ PRINT_WP_POSITION ] ) {
        table << (const char *)wxString(_("Position")).mb_str();
    }
    if ( toPrintOut[ PRINT_WP_COURSE ] ) {
        table << (const char *)wxString(_("Course")).mb_str();
    }
    if ( toPrintOut[ PRINT_WP_DISTANCE ] ) {
        table << (const char *)wxString(_("Distance")).mb_str();
    }
    if ( toPrintOut[ PRINT_WP_DESCRIPTION ] ) {
        table << (const char *)wxString(_("Description")).mb_str();
    }

    table.StartFillWidths();
    // setup widths for columns
    if ( toPrintOut[ PRINT_WP_NAME ] ) {
        table << 23;
    }
    if ( toPrintOut[ PRINT_WP_POSITION ] ) {
        table << 40;
    }
    if ( toPrintOut[ PRINT_WP_COURSE ] ) {
        table << 30;
    }
    if ( toPrintOut[ PRINT_WP_DISTANCE ] ) {
        table << 38;
    }
    if ( toPrintOut[ PRINT_WP_DESCRIPTION ] ) {
        table << 100;
    }

    table.StartFillData();

    for ( int n = 1; n <= myRoute->GetnPoints(); n++ ) {
        RoutePoint* point = myRoute->GetPoint( n );
        RoutePoint* next_point = (n < myRoute->GetnPoints())?myRoute->GetPoint( n+1 ):NULL;
        if ( toPrintOut[ PRINT_WP_NAME ] ) {
            string cell( point->GetName().mb_str() );
            table << cell;
        }
        if ( toPrintOut[ PRINT_WP_POSITION ] ) {
            wxString point_position = toSDMM( 1, point->m_lat, point->m_bIsInTrack ) + _T( "\n" ) + toSDMM( 2, point->m_lon, point->m_bIsInTrack );
            string   cell( point_position.mb_str() );
            table << cell;
        }
        if ( toPrintOut[ PRINT_WP_COURSE ] ) {
            wxString point_course;
            point_course.Printf( _T( "%03.0f Deg" ), point->GetCourse() );
            string   cell( point_course.mb_str() );
            table << cell;
        }
        if ( toPrintOut[ PRINT_WP_DISTANCE ] ) {
            wxString point_distance;
            point_distance.Printf( _T( "%6.2f NM" ), (next_point!= NULL)?(next_point->GetDistance()):0. );
            string   cell( point_distance.mb_str() );
            table << cell;
        }
        if ( toPrintOut[ PRINT_WP_DESCRIPTION ] ) {
            string cell( point->GetDescription().mb_str() );
            table << cell;
        }
        table << "\n";
        
        // here we collect points, which do participate as approaches
        if (point->IsApproach())
        {
                wxString _approach_name = point->GetApproachName();
                approach_points[_approach_name].push_back(point);
        }
     }
     
     for (std::map<wxString, std::vector<RoutePoint* > >::iterator iter=approach_points.begin(); iter != approach_points.end(); iter++) {
        std::vector<RoutePoint* > _approach = (*iter).second;
        PrintTable &_table = approach_tables[(*iter).first];      
        _table.SetTableTitle( (*iter).first );
        _table.StartFillHeader();
        _table << (char *)::wxGetTranslation ((const wxChar *)"Name");
        _table << (char *)::wxGetTranslation ((const wxChar *)"Description");
        _table << (char *)::wxGetTranslation ((const wxChar *)"Passing");
        _table << (char *)::wxGetTranslation ((const wxChar *)"OK?");
        _table.StartFillWidths();
        _table << 23;
        _table << 140;
        _table << 40;
        _table << 15;
        _table.StartFillData();
        for (std::vector<RoutePoint* >::iterator p_iter=_approach.begin(); p_iter != _approach.end(); p_iter++) { 
            RoutePoint * _point = *p_iter;
            
             if ( toPrintOut[ PRINT_WP_IMAGE_APPROACH ] ) {
                HyperlinkList::iterator iter;
                HyperlinkList* href_list = _point->m_HyperlinkList;
                for (iter = href_list->begin(); iter != href_list->end(); ++iter)
                {
                    Hyperlink *link = *iter;
                    if (link->DescrText.CmpNoCase(_T("APPROACH"))==0 || link->DescrText.CmpNoCase(_T("PLAN"))==0  )
                    {
                        _table.SetApproachImageUrl( link->Link );
                        _table.SetImage( UrlDownloader::LoadImage( link->Link ) );
                        break;
                    }
                }    
            }
            string cell1( _point->GetName().mb_str() );
            _table << cell1;
            string cell2( _point->GetDescription().mb_str() );
            _table << cell2;
            BouyPassingSide passing_side =  _point->GetBouyPassingSide();
            cout << passing_side << " ";
            string cell3(  (passing_side == BOUY_SB) ? "Strb" : 
                           (passing_side == BOUY_P)? "Port" : 
                           (passing_side == BOUY_GATE)? "Gate" : 
                           (passing_side == BOUY_AHEAD)? "Ahead" : 
                           (passing_side == BOUY_REAR)? "Rear" : 
                           "None" );
            cout << cell3 << endl;
            _table << cell3;
            string cell4( "   " );
            _table << cell4;
            _table << "\n";
        }
     }
}


void MyRoutePrintout::GetPageInfo( int* minPage, int* maxPage, int* selPageFrom, int* selPageTo )
{
    *minPage     = 1;
    *maxPage     = numberOfPages;
    *selPageFrom = 1;
    *selPageTo   = numberOfPages;
}


void MyRoutePrintout::OnPreparePrinting()
{
    pageToPrint = 1;
    wxDC*  dc = GetDC();
    wxFont routePrintFont( 10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL );
    dc->SetFont( routePrintFont );

    // Get the size of the DC in pixels
    int w, h;
    dc->GetSize( &w, &h );
    
    // We don't know before hand what size the Print DC will be, in pixels.  Varies by host.
    // So, if the dc size is greater than 1000 pixels, we scale accordinly.
    
    int maxX = wxMin(w, 1000);
    int maxY = wxMin(h, 1000);
    
    // Calculate a suitable scaling factor
    double scaleX = ( double )( w / maxX );
    double scaleY = ( double )( h / maxY );
    
    // Use x or y scaling factor, whichever fits on the DC
    double actualScale = wxMin( scaleX, scaleY );
    
    // Set the scale and origin
    dc->SetUserScale( actualScale, actualScale );
    dc->SetDeviceOrigin( ( long )marginX, ( long )marginY );
    
    table.AdjustCells( dc, marginX, marginY );
    numberOfPages = table.GetNumberPages();
    for (std::map<wxString, std::vector<RoutePoint* > >::iterator iter=approach_points.begin(); iter != approach_points.end(); iter++) {
        PrintTable &_table = approach_tables[(*iter).first];   
        _table.SetStartPage(numberOfPages);
        _table.AdjustCells( dc, marginX, marginY );        
        numberOfPages += _table.GetNumberPages();       
    }

}


bool MyRoutePrintout::OnPrintPage( int page )
{
    wxDC* dc = GetDC();
    if( dc ) {
        if( page <= numberOfPages ){
            pageToPrint = page;
            DrawPage( dc );
            return true;
        }
        else
            return false;
    } else
        return false;
    
}

void MyRoutePrintout::DrawPage( wxDC* dc )
{
    PrintTable & __table = table;
    bool table_found = false;
    for (std::map<wxString, std::vector<RoutePoint* > >::iterator iter=approach_points.begin(); iter != approach_points.end(); iter++) {
        PrintTable &_table = approach_tables[(*iter).first];   
        vector< vector < PrintCell > > & cells = _table.GetContent();
        for ( size_t i = 0; i < cells.size(); i++ ) {
            vector< PrintCell >& content_row = cells[ i ];
            for ( size_t j = 0; j < content_row.size(); j++ ) {
                PrintCell& cell = content_row[ j ];
                if ( cell.GetPage() == pageToPrint ) {
                    table_found = true;
                    __table = _table;   
                    break;
                }
            }
            if (table_found)
                break;
        }
   
     }


    int currentX = marginX;
    int currentY = marginY;    
     
    wxFont routePrintFont_bold( 10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD );
    dc->SetFont( routePrintFont_bold );
    if (pageToPrint == __table.GetStartPage() + 1)
    {
        // adjust font for title
        wxFont orig_font = dc->GetFont();
        wxFont _font = orig_font;
        
        _font.SetWeight( wxFONTWEIGHT_BOLD );
        _font.SetPixelSize( orig_font.GetPixelSize().Scale(1.5, 1.5) );
        
        dc->SetFont( _font );
        int title_w, title_h;
        dc->GetMultiLineTextExtent( __table.GetTableTitle(), &title_w, &title_h );
        
        dc->DrawText( __table.GetTableTitle(),  currentX, currentY  );
        currentY += title_h;
        
        dc->SetFont( orig_font );    
    }
    currentY += marginY;    
    
    wxBrush brush( wxColour(255,255,255),  wxTRANSPARENT );
    dc->SetBrush( brush );


    int header_textOffsetX = 2;
    int header_textOffsetY = 2;
    
   if ( ! ( pageToPrint == (__table.GetStartPage() + __table.GetNumberPages() + 1) ) ) // dont print header if image
    {
        vector< PrintCell >& header_content = __table.GetHeader();
        for ( size_t j = 0; j < header_content.size(); j++ ) {
            PrintCell& cell = header_content[ j ];
            dc->DrawRectangle( currentX, currentY, cell.GetWidth(), cell.GetHeight() );
            dc->DrawText( cell.GetText(),  currentX +header_textOffsetX, currentY + header_textOffsetY );
            currentX += cell.GetWidth();
        }
    }
    wxFont  routePrintFont_normal( 10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL );
    dc->SetFont( routePrintFont_normal );

    vector< vector < PrintCell > > & cells = __table.GetContent();
    currentY += marginY + __table.GetHeaderHeight();
    int currentHeight = 0;
    for ( size_t i = 0; i < cells.size(); i++ ) {
        vector< PrintCell >& content_row = cells[ i ];
        currentX = marginX;
        for ( size_t j = 0; j < content_row.size(); j++ ) {
            PrintCell& cell = content_row[ j ];
            if ( cell.GetPage() == pageToPrint ) {
                wxRect r( currentX, currentY, cell.GetWidth(), cell.GetHeight() );
                dc->DrawRectangle( r );
                r.Offset( textOffsetX, textOffsetY );
                dc->DrawLabel(cell.GetText(), r);
                currentX     += cell.GetWidth();
                currentHeight = cell.GetHeight();
            }
        }
        currentY += currentHeight;
    }
    
    if (__table.HasImage() && pageToPrint == (__table.GetStartPage() + __table.GetNumberPages()) )
    {
        // resale to fil into page with preserving proportions...
        int w, h;
        dc->GetSize(&w, &h);
        
        int image_margin = 10;
        w -= 4*image_margin;
        h -= 4*image_margin;
        wxImage __image = __table.GetImage();
        int image_h = __image.GetHeight();
        int image_w = __image.GetWidth();
        int new_w = 0;
        int new_h = 0;
        
        if ((image_h >= h) || (image_w >= w ))
        {
            double ratio = (double)image_w/(double)image_h;
            if (ratio > 1.)
            {
                new_w = w;
                new_h = h / ratio * ( (double)w / (double)image_w);
            }else
            {
                new_h = h;
                new_w = w * ratio* ( (double)h / (double)image_h);
            }
        }else
        {
            new_h = image_h;
            new_w = image_w;
        }
        dc->DrawBitmap( wxBitmap(__image.Scale( new_w , new_h, wxIMAGE_QUALITY_HIGH )), image_margin, image_margin, false );
    }
    
}


// ---------- RoutePrintSelection dialof implementation

/*!
 * RoutePrintSelection type definition
 */

IMPLEMENT_DYNAMIC_CLASS( RoutePrintSelection, wxDialog )
/*!
 * RouteProp event table definition
 */

BEGIN_EVENT_TABLE( RoutePrintSelection, wxDialog )
EVT_BUTTON( ID_ROUTEPRINT_SELECTION_CANCEL, RoutePrintSelection::OnRoutepropCancelClick )
EVT_BUTTON( ID_ROUTEPRINT_SELECTION_OK, RoutePrintSelection::OnRoutepropOkClick )
END_EVENT_TABLE()

/*!
 * RouteProp constructors
 */

RoutePrintSelection::RoutePrintSelection()
{
}


RoutePrintSelection::RoutePrintSelection( wxWindow*       parent,
                                          Route*          _route,
                                          wxWindowID      id,
                                          const wxString& caption,
                                          const wxPoint&  pos,
                                          const wxSize&   size,
                                          long            style )
{
    route = _route;

    long wstyle = style;

    Create( parent, id, caption, pos, size, wstyle );
    Centre();
}


RoutePrintSelection::~RoutePrintSelection()
{
}


/*!
 * RouteProp creator
 */

bool RoutePrintSelection::Create( wxWindow* parent, wxWindowID id, const wxString& caption,
                             const wxPoint& pos, const wxSize& size, long style )
{
    SetExtraStyle( GetExtraStyle() | wxWS_EX_BLOCK_EVENTS );

#ifdef __WXOSX__
    style |= wxSTAY_ON_TOP;
#endif
    
    wxDialog::Create( parent, id, _("Print Route Selection"), pos, size, style );

    CreateControls();

    return TRUE;
}


/*!
 * Control creation for RouteProp
 */

void RoutePrintSelection::CreateControls()
{
    RoutePrintSelection* itemDialog1 = this;

    wxStaticBox*         itemStaticBoxSizer3Static = new wxStaticBox( itemDialog1, wxID_ANY,
                                                                      _( "Elements to print..." ) );

    wxStaticBoxSizer* itemBoxSizer1 = new wxStaticBoxSizer( itemStaticBoxSizer3Static,  wxVERTICAL );
    itemDialog1->SetSizer( itemBoxSizer1 );

    wxFlexGridSizer* fgSizer2;
    fgSizer2 = new wxFlexGridSizer( 5, 2, 0, 0 );

    m_checkBoxWPName = new wxCheckBox( itemDialog1, wxID_ANY, _( "Name" ),
                                       wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
    m_checkBoxWPName->SetValue( true );
    fgSizer2->Add( m_checkBoxWPName, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5 );

    wxStaticText* label1 = new  wxStaticText( itemDialog1, wxID_ANY, _( "Show Waypoint name." ), wxDefaultPosition, wxDefaultSize );
    fgSizer2->Add( label1, 1, wxALL | wxALIGN_CENTER_VERTICAL, 5 );

    m_checkBoxWPPosition = new wxCheckBox( itemDialog1, wxID_ANY, _( "Position" ),
                                           wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
    m_checkBoxWPPosition->SetValue( true );
    fgSizer2->Add( m_checkBoxWPPosition, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5 );
    wxStaticText* label2 = new  wxStaticText( itemDialog1, wxID_ANY, _( "Show Waypoint position." ), wxDefaultPosition, wxDefaultSize );
    fgSizer2->Add( label2, 1, wxALL | wxALIGN_CENTER_VERTICAL, 5 );

    m_checkBoxWPCourse = new wxCheckBox( itemDialog1, wxID_ANY, _( "Course" ),
                                         wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
    m_checkBoxWPCourse->SetValue( true );
    fgSizer2->Add( m_checkBoxWPCourse, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5 );
    wxStaticText* label3 = new  wxStaticText( itemDialog1, wxID_ANY, _( "Show course from each Waypoint to the next one. " ), wxDefaultPosition, wxDefaultSize );
    fgSizer2->Add( label3, 1, wxALL | wxALIGN_CENTER_VERTICAL, 5 );

    m_checkBoxWPDistanceToNext = new wxCheckBox( itemDialog1, wxID_ANY, _( "Distance" ),
                                                 wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
    m_checkBoxWPDistanceToNext->SetValue( true );
    fgSizer2->Add( m_checkBoxWPDistanceToNext, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5 );
    wxStaticText* label4 = new  wxStaticText( itemDialog1, wxID_ANY, _( "Show Distance from each Waypoint to the next one." ), wxDefaultPosition, wxDefaultSize );
    fgSizer2->Add( label4, 1, wxALL | wxALIGN_CENTER_VERTICAL, 5 );

    m_checkBoxWPDescription = new wxCheckBox( itemDialog1, wxID_ANY, _( "Description" ),
                                              wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
    m_checkBoxWPDescription->SetValue( true );
    fgSizer2->Add( m_checkBoxWPDescription, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5 );
    wxStaticText* label5 = new  wxStaticText( itemDialog1, wxID_ANY, _( "Show Waypoint description." ), wxDefaultPosition, wxDefaultSize );
    fgSizer2->Add( label5, 1, wxALL | wxALIGN_CENTER_VERTICAL, 5 );

    
    m_checkBoxApproachImage = new wxCheckBox( itemDialog1, wxID_ANY, _( "Approach/Harbour plan" ),
                                              wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
    m_checkBoxApproachImage->SetValue( true );
    fgSizer2->Add( m_checkBoxApproachImage, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5 );
    wxStaticText* label6 = new  wxStaticText( itemDialog1, wxID_ANY, _( "Approach/Harbour plan." ), wxDefaultPosition, wxDefaultSize );
    fgSizer2->Add( label6, 1, wxALL | wxALIGN_CENTER_VERTICAL, 5 );

    itemBoxSizer1->Add( fgSizer2, 5, wxEXPAND, 5 );
    

    wxBoxSizer* itemBoxSizer16 = new wxBoxSizer( wxHORIZONTAL );
    itemBoxSizer1->Add( itemBoxSizer16, 0, wxALIGN_RIGHT | wxALL, 5 );

    m_CancelButton = new wxButton( itemDialog1, ID_ROUTEPRINT_SELECTION_CANCEL, _( "Cancel" ), wxDefaultPosition,
                                   wxDefaultSize, 0 );
    itemBoxSizer16->Add( m_CancelButton, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxALL, 5 );

    m_OKButton = new wxButton( itemDialog1, ID_ROUTEPRINT_SELECTION_OK, _( "OK" ), wxDefaultPosition,
                               wxDefaultSize, 0 );
    itemBoxSizer16->Add( m_OKButton, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxALL, 5 );
    m_OKButton->SetDefault();

    SetColorScheme( ( ColorScheme )0 );
}


void RoutePrintSelection::SetColorScheme( ColorScheme cs )
{
    DimeControl( this );
}


/*
 * Should we show tooltips?
 */

bool RoutePrintSelection::ShowToolTips()
{
    return TRUE;
}


void RoutePrintSelection::SetDialogTitle( wxString title )
{
    SetTitle( title );
}


void RoutePrintSelection::OnRoutepropCancelClick( wxCommandEvent& event )
{
    Hide();
    event.Skip();
}


void RoutePrintSelection::OnRoutepropOkClick( wxCommandEvent& event )
{
    std::vector<bool> toPrintOut;
    toPrintOut.push_back( m_checkBoxWPName->GetValue() );
    toPrintOut.push_back( m_checkBoxWPPosition->GetValue() );
    toPrintOut.push_back( m_checkBoxWPCourse->GetValue() );
    toPrintOut.push_back( m_checkBoxWPDistanceToNext->GetValue() );
    toPrintOut.push_back( m_checkBoxWPDescription->GetValue() );
    toPrintOut.push_back( m_checkBoxApproachImage->GetValue() );
    if ( NULL == g_printData ) {
        g_printData = new wxPrintData;
        g_printData->SetOrientation( wxLANDSCAPE );
        g_pageSetupData = new wxPageSetupDialogData;
    }

    MyRoutePrintout*  myrouteprintout1 = new MyRoutePrintout( toPrintOut, route,  _( "Route Print" ) );

    wxPrintDialogData printDialogData( *g_printData );
    printDialogData.EnablePageNumbers( true );

    wxPrinter printer( &printDialogData );
    if ( !printer.Print( this, myrouteprintout1, true ) ) {
        if ( wxPrinter::GetLastError() == wxPRINTER_ERROR ) {
            OCPNMessageBox(
                NULL,
                _( "There was a problem printing.\nPerhaps your current printer is not set correctly?" ),
                _T( "OpenCPN" ), wxOK );
        }
    }

    Hide();
    event.Skip();
}
