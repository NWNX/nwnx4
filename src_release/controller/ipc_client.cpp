/***************************************************************************
    NWNX IPC SERVER - Interprocess communication with server
    Copyright (C) 2006 Ingmar Stieger (Papillon, papillon@nwnx.org)

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
#include "ipc_client.h"

// ----------------------------------------------------------------------------
// NWNXClient
// ----------------------------------------------------------------------------
NWNXClient::NWNXClient() : wxClient()
{
    m_connection = NULL;
}

bool NWNXClient::Connect(const wxString& sHost, const wxString& sService, const wxString& sTopic)
{
    // suppress the log messages from MakeConnection()
    wxLogNull nolog;

	//wxLogMessage(_T("Connect host %s, service %s, topic %s"), sHost, sService, sTopic);

    m_connection = (NWNXClientConnection *)MakeConnection(sHost, sService, sTopic);
    return m_connection != NULL;
}

wxConnectionBase *NWNXClient::OnMakeConnection()
{
    return new NWNXClientConnection;
}

void NWNXClient::Disconnect()
{
    if (m_connection)
    {
        m_connection->Disconnect();
        delete m_connection;
        m_connection = NULL;
        wxLogMessage(_T("Client disconnected from server"));
    }
}

NWNXClient::~NWNXClient()
{
    Disconnect();
}

// ----------------------------------------------------------------------------
// NWNXServerConnection
// ----------------------------------------------------------------------------

void NWNXClientConnection::Log(const wxString& command, const wxString& topic,
    const wxString& item, wxChar *data, int size, wxIPCFormat format)
{
    wxString s;
    if (topic.IsEmpty() && item.IsEmpty())
        s.Printf(_T("%s("), command.c_str());
    else if (topic.IsEmpty())
        s.Printf(_T("%s(item=\"%s\","), command.c_str(), item.c_str());
    else if (item.IsEmpty())
        s.Printf(_T("%s(topic=\"%s\","), command.c_str(), topic.c_str());
    else
        s.Printf(_T("%s(topic=\"%s\",item=\"%s\","), command.c_str(), topic.c_str(), item.c_str());

    if (format == wxIPC_TEXT || format == wxIPC_UNICODETEXT) 
        wxLogMessage(_T("%s\"%s\",%d)"), s.c_str(), data, size);
    else if (format == wxIPC_PRIVATE)
    {
        if (size == 3)
        {
            char *bytes = (char *)data;
            wxLogMessage(_T("%s'%c%c%c',%d)"), s.c_str(), bytes[0], bytes[1], bytes[2], size);
        }
        else
            wxLogMessage(_T("%s...,%d)"), s.c_str(), size);
    }
    else if (format == wxIPC_INVALID) 
        wxLogMessage(_T("%s[invalid data],%d)"), s.c_str(), size);
}

bool NWNXClientConnection::OnAdvise(const wxString& topic, const wxString& item, wxChar *data,
    int size, wxIPCFormat format)
{
    Log(_T("OnAdvise"), topic, item, data, size, format);
    return true;
}

bool NWNXClientConnection::OnDisconnect()
{
    wxLogTrace(TRACE_VERBOSE, _T("OnDisconnect()"));
    return true;
}

bool NWNXClientConnection::Execute(const wxChar *data, int size, wxIPCFormat format)
{
	//wxLogTrace(TRACE_VERBOSE, wxT("Execute"), (wxChar *)data);
    bool retval = wxConnection::Execute(data, size, format);
    if (!retval)
        wxLogMessage(_T("Execute failed!"));
    return retval;
}

wxChar *NWNXClientConnection::Request(const wxString& item, int *size, wxIPCFormat format)
{
    wxChar *data =  wxConnection::Request(item, size, format);
    Log(_T("Request"), _T(""), item, data, size ? *size : -1, format);
    return data;
}

bool NWNXClientConnection::Poke(const wxString& item, wxChar *data, int size, wxIPCFormat format)
{
	//wxLogTrace(TRACE_VERBOSE, wxT("Poke(%s, %s)"), item, (wxChar *)data);
    return wxConnection::Poke(item, data, size, format);
}

