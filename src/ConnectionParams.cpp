/***************************************************************************
 *
 * Project:  OpenCPN
 *
 ***************************************************************************
 *   Copyright (C) 2013 by David S. Register                               *
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
 **************************************************************************/

#include <wx/tokenzr.h>
#include <wx/intl.h>

#include <wx/statline.h>
#include "ConnectionParams.h"

#include "ocpn_plugin.h"
#include "chart1.h"
#include "options.h"

#if !wxUSE_XLOCALE && wxCHECK_VERSION(3,0,0)
#define wxAtoi(arg) atoi(arg)
#endif


ConnectionParams::ConnectionParams(const wxString &configStr )
{
    Deserialize( configStr );
}

void ConnectionParams::Deserialize(const wxString &configStr)
{
    Valid = true;
    wxArrayString prms = wxStringTokenize( configStr, _T(";") );
    if (prms.Count() < 18) {
        Valid = false;
        return;
    }

    Type = (ConnectionType)wxAtoi(prms[0]);
    NetProtocol = (NetworkProtocol)wxAtoi(prms[1]);
    NetworkAddress = prms[2];
    NetworkPort = (ConnectionType)wxAtoi(prms[3]);
    Protocol = (DataProtocol)wxAtoi(prms[4]);
    Port = prms[5];
    Baudrate = wxAtoi(prms[6]);
    ChecksumCheck = !!wxAtoi(prms[7]);
    int iotval = wxAtoi(prms[8]);
    IOSelect=((iotval <= 2)?static_cast <dsPortType>(iotval):DS_TYPE_INPUT);
    InputSentenceListType = (ListType)wxAtoi(prms[9]);
    InputSentenceList = wxStringTokenize(prms[10], _T(","));
    OutputSentenceListType = (ListType)wxAtoi(prms[11]);
    OutputSentenceList = wxStringTokenize(prms[12], _T(","));
    Priority = wxAtoi(prms[13]);
    Garmin = !!wxAtoi(prms[14]);
    GarminUpload = !!wxAtoi(prms[15]);
    FurunoGP3X = !!wxAtoi(prms[16]);

    bEnabled = true;
    LastNetworkPort = 0;
    b_IsSetup = false;
    if (prms.Count() >= 18){
        bEnabled = !!wxAtoi(prms[17]);
    }
    if (prms.Count() >= 19){
        UserComment = prms[18];
    }
    
}

wxString ConnectionParams::Serialize()
{
    wxString istcs;
    for( size_t i = 0; i < InputSentenceList.Count(); i++ )
    {
        if (i > 0)
            istcs.Append( _T(",") );
        istcs.Append( InputSentenceList[i] );
    }
    wxString ostcs;
    for( size_t i = 0; i < OutputSentenceList.Count(); i++ )
    {
        if (i > 0)
            ostcs.Append( _T(",") );
        ostcs.Append( OutputSentenceList[i] );
    }
    wxString ret = wxString::Format( _T("%d;%d;%s;%d;%d;%s;%d;%d;%d;%d;%s;%d;%s;%d;%d;%d;%d;%d;%s"),
                                     Type,
                                     NetProtocol,
                                     NetworkAddress.c_str(),
                                     NetworkPort,
                                     Protocol,
                                     Port.c_str(),
                                     Baudrate,
                                     ChecksumCheck,
                                     IOSelect,
                                     InputSentenceListType,
                                     istcs.c_str(),
                                     OutputSentenceListType,
                                     ostcs.c_str(),
                                     Priority,
                                     Garmin,
                                     GarminUpload,
                                     FurunoGP3X,
                                     bEnabled,
                                     UserComment.c_str()
                                   );

    return ret;
}

ConnectionParams::ConnectionParams()
{
    Type = SERIAL;
    NetProtocol = TCP;
    NetworkAddress = wxEmptyString;
    NetworkPort = 0;
    Protocol = PROTO_NMEA0183;
    Port = wxEmptyString;
    Baudrate = 4800;
    ChecksumCheck = true;
    Garmin = false;
    FurunoGP3X = false;
    IOSelect = DS_TYPE_INPUT;
    InputSentenceListType = WHITELIST;
    OutputSentenceListType = WHITELIST;
    Priority = 0;
    Valid = true;
    bEnabled = true;
    b_IsSetup = false;
}

wxString ConnectionParams::GetSourceTypeStr()
{
    if ( Type == SERIAL )
        return _("Serial");
    else if ( Type == NETWORK )
        return _("Network");
    else if ( Type == INTERNAL_GPS )
        return _("GPS");
    else if ( Type == INTERNAL_BT )
        return _("BT");
    else
        return _T("");
}

wxString ConnectionParams::GetAddressStr()
{
    if ( Type == SERIAL )
        return wxString::Format( _T("%s"), Port.c_str() );
    else if ( Type == NETWORK )
        return wxString::Format( _T("%s:%d"), NetworkAddress.c_str(), NetworkPort );
    else if ( Type == INTERNAL_GPS )
        return _("Internal");
    else if ( Type == INTERNAL_BT )
        return NetworkAddress;
    else
        return _T("");
}

wxString ConnectionParams::GetParametersStr()
{
    if ( Type == SERIAL )
        return wxString::Format( _T("%d"), Baudrate );
    else if ( Type == NETWORK ){
        if ( NetProtocol == TCP )
            return _("TCP");
        else if (NetProtocol == UDP)
            return _("UDP");
        else
            return _("GPSD");
    }
    else if ( Type == INTERNAL_GPS )
        return _T("");
    else if ( Type == INTERNAL_BT )
        return Port;
    else
        return _T("");
}

wxString ConnectionParams::GetIOTypeValueStr()
{
    if ( IOSelect == DS_TYPE_INPUT )
        return _("Input");
    else if ( IOSelect == DS_TYPE_OUTPUT )
        return _("Output");
    else
        return _("In/Out");
}

wxString ConnectionParams::FilterTypeToStr(ListType type, FilterDirection dir)
{
    if(dir == FILTER_INPUT) {
        if ( type == BLACKLIST )
            return _("Reject");
        else
            return _("Accept");
    }
    else {
        if ( type == BLACKLIST )
            return _("Drop");
        else
            return _("Send");
    }
}

wxString ConnectionParams::GetFiltersStr()
{
    wxString istcs;
    for( size_t i = 0; i < InputSentenceList.Count(); i++ )
    {
        if ( i > 0 )
            istcs.Append( _T(",") );
        istcs.Append( InputSentenceList[i] );
    }
    wxString ostcs;
    for( size_t i = 0; i < OutputSentenceList.Count(); i++ )
    {
        if ( i > 0 )
            ostcs.Append( _T(",") );
        ostcs.Append( OutputSentenceList[i] );
    }
    wxString ret = wxEmptyString;
    if ( istcs.Len() > 0 ){
        ret.Append( _("In") );
        ret.Append(wxString::Format( _T(": %s %s"),
                                     FilterTypeToStr(InputSentenceListType, FILTER_INPUT).c_str(), istcs.c_str()) );
    }
    else
        ret.Append( _("In: None") );

    if ( ostcs.Len() > 0 ){
        ret.Append(  _T(", ") );
        ret.Append(  _("Out") );
        ret.Append( wxString::Format( _T(": %s %s"),
                                      FilterTypeToStr(OutputSentenceListType, FILTER_OUTPUT).c_str(), ostcs.c_str() ) );
    }
    else
        ret.Append( _(", Out: None") );
    return  ret;
}

wxString ConnectionParams::GetDSPort()
{
    if ( Type == SERIAL )
        return wxString::Format( _T("Serial:%s"), Port.c_str() );
    else if( Type == NETWORK){
        wxString proto;
        if ( NetProtocol == TCP )
            proto = _T("TCP");
        else if (NetProtocol == UDP)
            proto = _T("UDP");
        else
            proto = _T("GPSD");
        return wxString::Format( _T("%s:%s:%d"), proto.c_str(), NetworkAddress.c_str(), NetworkPort );
    }
    else if( Type == INTERNAL_BT ){
        //  GPSD:HOLUX GR-231:0
        //wxString proto = _T("GPSD");
        //return wxString::Format( _T("%s:%s:%d"), proto.c_str(), NetworkAddress.c_str(), NetworkPort );
        return Port;   //mac
    }
    else
        return _T("");
    
}

wxString ConnectionParams::GetLastDSPort()
{
    if ( Type == SERIAL )
        return wxString::Format( _T("Serial:%s"), Port.c_str() );
    else
    {
        wxString proto;
        if ( LastNetProtocol == TCP )
            proto = _T("TCP");
        else if (LastNetProtocol == UDP)
            proto = _T("UDP");
        else
            proto = _T("GPSD");
        return wxString::Format( _T("%s:%s:%d"), proto.c_str(), LastNetworkAddress.c_str(), LastNetworkPort );
    }
}

extern "C"  bool GetGlobalColor(wxString colorName, wxColour *pcolour);

BEGIN_EVENT_TABLE(ConnectionParamsPanel, wxPanel)
EVT_PAINT ( ConnectionParamsPanel::OnPaint )
EVT_ERASE_BACKGROUND(ConnectionParamsPanel::OnEraseBackground)
END_EVENT_TABLE()

ConnectionParamsPanel::ConnectionParamsPanel(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size,
                                             ConnectionParams *p_itemConnectionParams, options *pContainer, int index)
:wxPanel(parent, id, pos, size, wxBORDER_NONE)
{
    m_pContainer = pContainer;
    m_pConnectionParams = p_itemConnectionParams;
    m_index = index;
    m_bSelected = false;

    int refHeight = GetCharHeight();
    SetMinSize(wxSize(-1, 5 * refHeight));
    m_unselectedHeight = 5 * refHeight;
    
    Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(ConnectionParamsPanel::OnSelected), NULL, this);
    CreateControls(); 
    
}

ConnectionParamsPanel::~ConnectionParamsPanel()
{
}

void ConnectionParamsPanel::OnSelected( wxMouseEvent &event )
{
    
    if(!m_bSelected){
        SetSelected( true );
        m_pContainer->SetSelectedConnectionPanel( this );
    }
    else{
        SetSelected( false );
        m_pContainer->SetSelectedConnectionPanel( NULL );
    }
 
}

void ConnectionParamsPanel::SetSelected( bool selected )
{
    m_bSelected = selected;
    wxColour colour;
    int refHeight = GetCharHeight();
    
    if (selected)
    {
        GetGlobalColor(_T("UIBCK"), &colour);
        m_boxColour = colour;
        SetSize(wxSize(-1, 9 * refHeight));
    }
    else
    {
        GetGlobalColor(_T("DILG0"), &colour);
        m_boxColour = colour;
        SetSize(wxSize(-1, 5 * refHeight));
    }
    
    GetSizer()->Layout();
    Refresh( true );
    
}

void ConnectionParamsPanel::OnEnableCBClick(wxCommandEvent &event){
    if(m_pContainer){
        m_pContainer->EnableConnection( m_pConnectionParams, event.IsChecked());
    }
}

void ConnectionParamsPanel::CreateControls( void ){
    int metric = GetCharHeight();

    wxFont *dFont = GetOCPNScaledFont_PlugIn(_("Dialog"));
    double font_size = dFont->GetPointSize() * 17/16;
    wxFont *bFont = wxTheFontList->FindOrCreateFont( font_size, dFont->GetFamily(), dFont->GetStyle(), wxFONTWEIGHT_BOLD);
    
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);

    mainSizer->AddSpacer( metric);

    wxBoxSizer* panelSizer = new wxBoxSizer(wxHORIZONTAL);
    mainSizer->Add(panelSizer, 1, wxLEFT, 5);

    // Enable cbox
    wxBoxSizer* enableSizer = new wxBoxSizer(wxVERTICAL);
    panelSizer->Add(enableSizer, 1, wxLEFT, metric);

    m_cbEnable = new wxCheckBox(this, wxID_ANY, _("Enable"));
    m_cbEnable->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED,
                          wxCommandEventHandler(ConnectionParamsPanel::OnEnableCBClick),
                          NULL, this);
    m_cbEnable->SetValue( m_pConnectionParams->bEnabled);
    
    enableSizer->Add(m_cbEnable, 1, wxLEFT | wxEXPAND, metric);
    
    //  Parms
    wxBoxSizer* parmSizer = new wxBoxSizer(wxVERTICAL);
    panelSizer->Add(parmSizer, 3, wxLEFT, metric);

    if(m_pConnectionParams->Type == SERIAL){
        
        wxFlexGridSizer *serialGrid = new wxFlexGridSizer(2, 6, 0, metric);
        serialGrid->SetFlexibleDirection(wxHORIZONTAL);
        parmSizer->Add(serialGrid, 0, wxALIGN_LEFT);

        wxString ioDir = m_pConnectionParams->GetIOTypeValueStr();
        
        wxStaticText *t1 = new wxStaticText(this, wxID_ANY, _("Type"));
        serialGrid->Add(t1, 0, wxALIGN_CENTER_HORIZONTAL );

        wxStaticText *t3 = new wxStaticText(this, wxID_ANY, _T(""));
        serialGrid->Add(t3, 0, wxALIGN_CENTER_HORIZONTAL );
        
        wxStaticText *t5 = new wxStaticText(this, wxID_ANY, _("Direction"));
        serialGrid->Add(t5, 0, wxALIGN_CENTER_HORIZONTAL );

        wxStaticText *t11 = new wxStaticText(this, wxID_ANY, _("Protocol"));
        serialGrid->Add(t11, 0, wxALIGN_CENTER_HORIZONTAL );
       
        wxStaticText *t13 = new wxStaticText(this, wxID_ANY, _("Serial Port"));
        serialGrid->Add(t13, 0, wxALIGN_CENTER_HORIZONTAL );

        wxStaticText *t15 = new wxStaticText(this, wxID_ANY, _("Baudrate"));
        serialGrid->Add(t15, 0, wxALIGN_CENTER_HORIZONTAL );

        //line 2
        wxStaticText *t2 = new wxStaticText(this, wxID_ANY, _("Serial"));
        t2->SetFont(*bFont);
        serialGrid->Add(t2, 0, wxALIGN_CENTER_HORIZONTAL );

        wxStaticText *t4 = new wxStaticText(this, wxID_ANY, _T(""));
        serialGrid->Add(t4, 0, wxALIGN_CENTER_HORIZONTAL );

        wxStaticText *t6 = new wxStaticText(this, wxID_ANY, ioDir);
        t6->SetFont(*bFont);
        serialGrid->Add(t6, 0, wxALIGN_CENTER_HORIZONTAL );

        wxString proto;
        switch(m_pConnectionParams->Protocol){
            case PROTO_NMEA0183:
                proto = _T("NMEA 0183"); break;
            case PROTO_SEATALK:
                proto = _T("SEATALK"); break;
            case PROTO_NMEA2000:
                proto = _T("NMEA 2000"); break;
            default:
                proto = _("Undefined"); break;
        }

        wxStaticText *t12 = new wxStaticText(this, wxID_ANY, proto);
        t12->SetFont(*bFont);
        serialGrid->Add(t12, 0, wxALIGN_CENTER_HORIZONTAL );

        wxString serialPort = m_pConnectionParams->Port;
        wxStaticText *t14 = new wxStaticText(this, wxID_ANY, serialPort);
        t14->SetFont(*bFont);
        serialGrid->Add(t14, 0, wxALIGN_CENTER_HORIZONTAL );

        wxString baudRate;  baudRate.Printf(_T("%d"), m_pConnectionParams->Baudrate);
        wxStaticText *t16 = new wxStaticText(this, wxID_ANY, baudRate);
        t16->SetFont(*bFont);
        serialGrid->Add(t16, 0, wxALIGN_CENTER_HORIZONTAL );

        wxStaticLine *line = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
        parmSizer->Add(line, 0, wxEXPAND);
        
        wxStaticText *t21 = new wxStaticText(this, wxID_ANY, _("Comment: ") + m_pConnectionParams->UserComment);
        parmSizer->Add(t21, 0);

#if 0
        wxStaticText *t1 = new wxStaticText(this, wxID_ANY, _("Connection Type: ") + _("Serial") + _T("     ") + ioDir);
        parmSizer->Add(t1, 0);

        wxString proto;
        switch(m_pConnectionParams->Protocol){
            case PROTO_NMEA0183:
                proto = _T("NMEA 0183"); break;
            case PROTO_SEATALK:
                proto = _T("SEATALK"); break;
            case PROTO_NMEA2000:
                proto = _T("NMEA 2000"); break;
            default:
                proto = _("Undefined"); break;
        }
        
        wxString serialPort = m_pConnectionParams->Port;
        wxString baudRate;  baudRate.Printf(_T("%d"), m_pConnectionParams->Baudrate);

        wxStaticText *t2 = new wxStaticText(this, wxID_ANY, _("Protocol: ") + proto + _T("       ") +
                _("Serial Port:  ") + serialPort + _T("      ") + _("Baudrate:  ") + baudRate);
        parmSizer->Add(t2, 0);

        wxStaticText *t3 = new wxStaticText(this, wxID_ANY, _("Comment: ") + m_pConnectionParams->UserComment);
        parmSizer->Add(t3, 0);
#endif
    }
    
 
    else{
        wxString ioDir = m_pConnectionParams->GetIOTypeValueStr();

        wxFlexGridSizer *netGrid = new wxFlexGridSizer(2, 6, 0, metric);
        netGrid->SetFlexibleDirection(wxHORIZONTAL);
        parmSizer->Add(netGrid, 0, wxALIGN_LEFT);
#if 0       
        wxStaticText *t1 = new wxStaticText(this, wxID_ANY, _("Type:"));
        netGrid->Add(t1, 0);

        wxStaticText *t2 = new wxStaticText(this, wxID_ANY, _("Network"));
        t2->SetFont(*bFont);
        netGrid->Add(t2, 0);

        wxStaticText *t3 = new wxStaticText(this, wxID_ANY, _T(""));
        netGrid->Add(t3, 0);
        
        wxStaticText *t4 = new wxStaticText(this, wxID_ANY, _T(""));
        netGrid->Add(t4, 0);

        wxStaticText *t5 = new wxStaticText(this, wxID_ANY, _("Direction:"));
        netGrid->Add(t5, 0);

        wxStaticText *t6 = new wxStaticText(this, wxID_ANY, ioDir);
        t6->SetFont(*bFont);
        netGrid->Add(t6, 0);
 
  
        wxStaticText *t11 = new wxStaticText(this, wxID_ANY, _("Protocol: "));
        netGrid->Add(t11, 0);

        wxString proto;
        switch(m_pConnectionParams->NetProtocol){
            case UDP:
                proto = _T("UDP"); break;
            case TCP:
                proto = _T("TCP"); break;
            case GPSD:
                proto = _T("GPSD"); break;
            default:
                proto = _("Undefined"); break;
        }

        wxStaticText *t12 = new wxStaticText(this, wxID_ANY, proto);
        t12->SetFont(*bFont);
        netGrid->Add(t12, 0);
        
       
        wxStaticText *t13 = new wxStaticText(this, wxID_ANY, _("Address:  "));
        netGrid->Add(t13, 0);
        
        wxString address = m_pConnectionParams->NetworkAddress;
        wxStaticText *t14 = new wxStaticText(this, wxID_ANY, address);
        t14->SetFont(*bFont);
        netGrid->Add(t14, 0);

        wxStaticText *t15 = new wxStaticText(this, wxID_ANY, _("Port:  "));
        netGrid->Add(t15, 0);
        
        wxString port;  port.Printf(_T("%d"), m_pConnectionParams->NetworkPort);
        wxStaticText *t16 = new wxStaticText(this, wxID_ANY, port);
        t16->SetFont(*bFont);
        netGrid->Add(t16, 0);
#endif

        wxStaticText *t1 = new wxStaticText(this, wxID_ANY, _("Type"));
        netGrid->Add(t1, 0, wxALIGN_CENTER_HORIZONTAL );

        wxStaticText *t3 = new wxStaticText(this, wxID_ANY, _T(""));
        netGrid->Add(t3, 0, wxALIGN_CENTER_HORIZONTAL );
        
        wxStaticText *t5 = new wxStaticText(this, wxID_ANY, _("Direction"));
        netGrid->Add(t5, 0, wxALIGN_CENTER_HORIZONTAL );

        wxStaticText *t11 = new wxStaticText(this, wxID_ANY, _("Protocol "));
        netGrid->Add(t11, 0, wxALIGN_CENTER_HORIZONTAL );
       
        wxStaticText *t13 = new wxStaticText(this, wxID_ANY, _("Address  "));
        netGrid->Add(t13, 0, wxALIGN_CENTER_HORIZONTAL );

        wxStaticText *t15 = new wxStaticText(this, wxID_ANY, _("Port  "));
        netGrid->Add(t15, 0, wxALIGN_CENTER_HORIZONTAL );

        //line 2
        wxStaticText *t2 = new wxStaticText(this, wxID_ANY, _("Network"));
        t2->SetFont(*bFont);
        netGrid->Add(t2, 0, wxALIGN_CENTER_HORIZONTAL );

        wxStaticText *t4 = new wxStaticText(this, wxID_ANY, _T(""));
        netGrid->Add(t4, 0, wxALIGN_CENTER_HORIZONTAL );

        wxStaticText *t6 = new wxStaticText(this, wxID_ANY, ioDir);
        t6->SetFont(*bFont);
        netGrid->Add(t6, 0, wxALIGN_CENTER_HORIZONTAL );

        wxString proto;
        switch(m_pConnectionParams->NetProtocol){
            case UDP:
                proto = _T("UDP"); break;
            case TCP:
                proto = _T("TCP"); break;
            case GPSD:
                proto = _T("GPSD"); break;
            default:
                proto = _("Undefined"); break;
        }

        wxStaticText *t12 = new wxStaticText(this, wxID_ANY, proto);
        t12->SetFont(*bFont);
        netGrid->Add(t12, 0, wxALIGN_CENTER_HORIZONTAL );

        wxString address = m_pConnectionParams->NetworkAddress;
        wxStaticText *t14 = new wxStaticText(this, wxID_ANY, address);
        t14->SetFont(*bFont);
        netGrid->Add(t14, 0, wxALIGN_CENTER_HORIZONTAL );

        wxString port;  port.Printf(_T("%d"), m_pConnectionParams->NetworkPort);
        wxStaticText *t16 = new wxStaticText(this, wxID_ANY, port);
        t16->SetFont(*bFont);
        netGrid->Add(t16, 0, wxALIGN_CENTER_HORIZONTAL );

        wxStaticLine *line = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
        parmSizer->Add(line, 0, wxEXPAND);
        
        wxStaticText *t21 = new wxStaticText(this, wxID_ANY, _("Comment: ") + m_pConnectionParams->UserComment);
        parmSizer->Add(t21, 0);
        
    }            
}



void ConnectionParamsPanel::OnEraseBackground( wxEraseEvent &event )
{
}

void ConnectionParamsPanel::OnPaint( wxPaintEvent &event )
{
    int width, height;
    GetSize( &width, &height );
    wxPaintDC dc( this );
 
    
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(wxBrush(GetBackgroundColour()));
    dc.DrawRectangle(GetVirtualSize());
    
    wxColour c;
    
    wxString nameString = m_pConnectionParams->Serialize();

    wxFont *dFont = GetOCPNScaledFont_PlugIn(_("Dialog"));
    
    
    if(m_bSelected){
        dc.SetBrush( wxBrush( m_boxColour ) );
        
        GetGlobalColor( _T ( "UITX1" ), &c );
        dc.SetPen( wxPen( wxColor(0xCE, 0xD5, 0xD6), 3 ));
        
        dc.DrawRoundedRectangle( 0, 0, width-1, height-1, height / 10);
        
        int base_offset = height / 10;
        
        // Draw the thumbnail
        int scaledWidth = height;
        
//        int scaledHeight = (height - (2 * base_offset)) * 95 / 100;
//        wxBitmap &bm = m_pChart->GetChartThumbnail( scaledHeight );
        
//        if(bm.IsOk()){
//            dc.DrawBitmap(bm, base_offset + 3, base_offset + 3);
//        }
        
//         wxFont *dFont = GetOCPNScaledFont_PlugIn(_("Dialog"));
//         double font_size = dFont->GetPointSize() * 3/2;
//         wxFont *qFont = wxTheFontList->FindOrCreateFont( font_size, dFont->GetFamily(), dFont->GetStyle(), dFont->GetWeight());
        
        int text_x = scaledWidth * 12 / 10;
//        dc.SetFont( *qFont );
        dc.SetTextForeground(wxColour(0,0,0));
//        dc.DrawText(nameString, text_x, height * 5 / 100);
  
        
#if 0        
        int hTitle = dc.GetCharHeight();
        int y_line = (height * 5 / 100) + hTitle;
        dc.DrawLine( text_x, y_line, width - base_offset, y_line);
        
        
        dc.SetFont( *dFont );           // Restore default font
        int offset = GetCharHeight();
        
        int yPitch = GetCharHeight();
        int yPos = y_line + 4;
        wxString tx;
        
        int text_x_val = scaledWidth + ((width - scaledWidth) * 4 / 10);
        
        // Create and populate the current chart information
        tx = _("Chart Edition:");
        dc.DrawText( tx, text_x, yPos);
        tx = m_pChart->currentChartEdition;
        dc.DrawText( tx, text_x_val, yPos);
        yPos += yPitch;
        
        tx = _("Order Reference:");
        dc.DrawText( tx, text_x, yPos);
        tx = m_pChart->orderRef;
        dc.DrawText( tx, text_x_val, yPos);
        yPos += yPitch;
        
        tx = _("Purchase date:");
        dc.DrawText( tx, text_x, yPos);
        tx = m_pChart->purchaseDate;
        dc.DrawText( tx, text_x_val, yPos);
        yPos += yPitch;
        
        tx = _("Expiration date:");
        dc.DrawText( tx, text_x, yPos);
        tx = m_pChart->expDate;
        dc.DrawText( tx, text_x_val, yPos);
        yPos += yPitch;
        
        tx = _("Status:");
        dc.DrawText( tx, text_x, yPos);
        tx = m_pChart->getStatusString();
        if(g_statusOverride.Len())
            tx = g_statusOverride;
        dc.DrawText( tx, text_x_val, yPos);
        yPos += yPitch;
#endif


        
        
    }
    else{
        dc.SetBrush( wxBrush( m_boxColour ) );
    
        GetGlobalColor( _T ( "UITX1" ), &c );
        dc.SetPen( wxPen( c, 1 ) );
    
        int offset = height / 10;
        dc.DrawRectangle( offset, offset, width - (2 * offset), height - (2 * offset));


//        // Draw the thumbnail
//        int scaledHeight = (height - (2 * offset)) * 95 / 100;
//        wxBitmap &bm = m_pChart->GetChartThumbnail( scaledHeight );
        
//        if(bm.IsOk()){
//            dc.DrawBitmap(bm, offset + 3, offset + 3);
//        }
        
//        int scaledWidth = bm.GetWidth() * scaledHeight / bm.GetHeight();
        int scaledWidth = height;
        
        
//         wxFont *dFont = GetOCPNScaledFont_PlugIn(_("Dialog"));
//         double font_size = dFont->GetPointSize() * 3/2;
//         wxFont *qFont = wxTheFontList->FindOrCreateFont( font_size, dFont->GetFamily(), dFont->GetStyle(), dFont->GetWeight());
// 
//         dc.SetFont( *qFont );
        dc.SetTextForeground(wxColour(128, 128, 128));
        
  //      if(m_pContainer->GetSelectedChart())
  //          dc.SetTextForeground(wxColour(220,220,220));
        
//        dc.DrawText(nameString, scaledWidth * 15 / 10, height * 35 / 100);
        
    }
    
    
}


