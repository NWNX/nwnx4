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

#if !defined(IPC_SERVER_H)
#define IPC_SERVER_H

//#define wxUSE_DDE_FOR_IPC 0
#include <wx/ipc.h>
#include "../misc/log.h"

#define ID_START         10000
#define ID_DISCONNECT    10001
#define ID_STARTADVISE    10002
#define ID_LOG          10003
#define ID_SERVERNAME    10004
#define ID_STOPADVISE    10005
#define ID_POKE            10006
#define ID_REQUEST        10007
#define ID_EXECUTE        10008
#define ID_TOPIC        10009
#define ID_HOSTNAME        10010

#define IPC_TOPIC wxT("NWNX SERVER") // the IPC topic

class NWNXClientConnection: public wxConnection
{
public:
    virtual bool Execute(const wxChar *data, int size = -1, wxIPCFormat format = wxIPC_TEXT);
    virtual wxChar *Request(const wxString& item, int *size = NULL, wxIPCFormat format = wxIPC_TEXT);
    virtual bool Poke(const wxString& item, wxChar *data, int size = -1, wxIPCFormat format = wxIPC_TEXT);
    virtual bool OnAdvise(const wxString& topic, const wxString& item, wxChar *data, int size, wxIPCFormat format);
    virtual bool OnDisconnect();
protected:    
    void Log(const wxString& command, const wxString& topic,
        const wxString& item, wxChar *data, int size, wxIPCFormat format);
};

class NWNXClient: public wxClient
{
public:
    NWNXClient();
    ~NWNXClient();
    bool Connect(const wxString& sHost, const wxString& sService, const wxString& sTopic);
    void Disconnect();
    wxConnectionBase *OnMakeConnection();
    bool IsConnected() { return m_connection != NULL; };
    NWNXClientConnection *GetConnection() { return m_connection; };

protected:
    NWNXClientConnection *m_connection;
};


#endif