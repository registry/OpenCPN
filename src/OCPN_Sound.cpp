/******************************************************************************
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
 ***************************************************************************
 */

#include "OCPN_Sound.h"
#include <wx/defs.h>
#include <wx/dialog.h>
#include <wx/file.h>
#include <wx/log.h>
#include <wx/string.h>
#include <wx/wxchar.h>


OcpnSound::OcpnSound()
{
    m_OK = false;
    m_deviceIx = -1;
    m_soundfile = "";
    m_onFinished = 0;
    m_callbackData = 0;
}


OcpnSound::~OcpnSound()
{
}

void OcpnSound::SetFinishedCallback(AudioDoneCallback cb, void* userData)
{
    m_onFinished = cb;
    m_callbackData = userData;
}
