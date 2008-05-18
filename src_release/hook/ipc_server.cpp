/***************************************************************************
    NWNX IPC SERVER - Interprocess communication with controller
    Copyright (C) 2007 Ingmar Stieger (Papillon, papillon@nwnx.org)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

***************************************************************************/

#include "stdwx.h"
#include "ipc_server.h"
#include "wx/datetime.h"

NWNXServer::NWNXServer() : wxServer()
{
    m_connection = NULL;
}

NWNXServer::~NWNXServer()
{
    Disconnect();
}

wxConnectionBase *NWNXServer::OnAcceptConnection(const wxString& topic)
{
    wxLogDebug(wxT("OnAcceptConnection(\"%s\")"), topic.c_str());

    if (topic == IPC_TOPIC)
    {
        m_connection = new NWNXServerConnection();
        wxLogMessage(wxT("Connection accepted"));
        return m_connection;
    }
    // unknown topic
	wxLogMessage(wxT("Connection NOT accepted"));
    return NULL;
}

void NWNXServer::Disconnect()
{
    if (m_connection)
    {
        m_connection->Disconnect();
        delete m_connection;
        m_connection = NULL;
        wxLogMessage(wxT("Disconnected client"));
    }
}

void NWNXServer::Advise()
{
}

// ----------------------------------------------------------------------------
// NWNXServerConnection
// ----------------------------------------------------------------------------

NWNXServerConnection::NWNXServerConnection() : wxConnection()
{
	initialized = false;
}

NWNXServerConnection::~NWNXServerConnection()
{
}

bool NWNXServerConnection::OnExecute(const wxString& topic,
    wxChar *data, int size, wxIPCFormat format)
{
	wxString command(data);

	if (command == wxT("init"))
	{
		if (!initialized)
		{
			init();
			initialized = true;
			return true;
		}
	}
	else
	{
		Log(wxT("OnExecute"), topic, wxT("empty"), data, size, format);
	}
    return false;
}

bool NWNXServerConnection::OnPoke(const wxString& topic,
    const wxString& item, wxChar *data, int size, wxIPCFormat format)
{
    Log(wxT("OnPoke"), topic, item, data, size, format);

	if (item == wxT("set_nwnx_home"))
	{
		nwnxhome = new wxString(data);
	}

    return wxConnection::OnPoke(topic, item, data, size, format);
}

wxChar *NWNXServerConnection::OnRequest(const wxString& topic,
    const wxString& item, int* size, wxIPCFormat format)
{
    wxChar *data;
    if (item == wxT("Date"))
    {
        m_sRequestDate = wxDateTime::Now().Format();
        data = (wxChar *)m_sRequestDate.c_str();
        *size = -1;
    }    
    else if (item == wxT("Date+len"))
    {
        m_sRequestDate = wxDateTime::Now().FormatTime() + wxT(" ") + wxDateTime::Now().FormatDate();
        data = (wxChar *)m_sRequestDate.c_str();
        *size = (int)(m_sRequestDate.Length() + 1) * sizeof(wxChar);
    }    
    else if (item == wxT("bytes[3]"))
    {
        data = (wxChar *)m_achRequestBytes;
        m_achRequestBytes[0] = '1'; m_achRequestBytes[1] = '2'; m_achRequestBytes[2] = '3';
        *size = 3;
    }
    else
    {
        data = NULL;
        *size = 0;
    }
     Log(wxT("OnRequest"), topic, item, data, *size, format);
    return data;
}

bool NWNXServerConnection::OnStartAdvise(const wxString& topic, const wxString& item)
{
    wxLogMessage(wxT("OnStartAdvise(\"%s\",\"%s\")"), topic.c_str(), item.c_str());
    wxLogMessage(wxT("Returning true"));
    m_sAdvise = item;
    return true;
}

bool NWNXServerConnection::OnStopAdvise(const wxString& topic, const wxString& item)
{
    wxLogMessage(wxT("OnStopAdvise(\"%s\",\"%s\")"), topic.c_str(), item.c_str());
    wxLogMessage(wxT("Returning true"));
    m_sAdvise.Empty();
    return true;
}

void NWNXServerConnection::Log(const wxString& command, const wxString& topic,
    const wxString& item, wxChar *data, int size, wxIPCFormat format)
{
    wxString s;
    if (topic.IsEmpty() && item.IsEmpty())
        s.Printf(wxT("%s("), command.c_str());
    else if (topic.IsEmpty())
        s.Printf(wxT("%s(\"%s\","), command.c_str(), item.c_str());
    else if (item.IsEmpty())
        s.Printf(wxT("%s(\"%s\","), command.c_str(), topic.c_str());
    else
        s.Printf(wxT("%s(\"%s\",\"%s\","), command.c_str(), topic.c_str(), item.c_str());

    if (format == wxIPC_TEXT || format == wxIPC_UNICODETEXT) 
        wxLogMessage(wxT("%s\"%s\",%d)"), s.c_str(), data, size);
    else if (format == wxIPC_PRIVATE)
    {
        if (size == 3)
        {
            char *bytes = (char *)data;
            wxLogTrace(TRACE_VERBOSE, wxT("%s'%c%c%c',%d)"), s.c_str(), bytes[0], bytes[1], bytes[2], size);
        }
        else
            wxLogTrace(TRACE_VERBOSE, wxT("%s...,%d)"), s.c_str(), size);
    }
    else if (format == wxIPC_INVALID) 
        wxLogTrace(TRACE_VERBOSE, wxT("%s[invalid data],%d)"), s.c_str(), size);
}

bool NWNXServerConnection::Advise(const wxString& item, wxChar *data, int size, wxIPCFormat format)
{
    Log(wxT("Advise"), wxT(""), item, data, size, format);
    return wxConnection::Advise(item, data, size, format);
}

bool NWNXServerConnection::OnDisconnect()
{
    wxLogTrace(TRACE_VERBOSE, wxT("OnDisconnect()"));
    return true;
}
