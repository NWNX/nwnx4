/***************************************************************************
    NWNX IPC SERVER - Interprocess communication with controller
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

#define IPC_TOPIC wxT("NWNX SERVER") // the IPC topic

#define ID_START      10000
#define ID_DISCONNECT 10001
#define ID_ADVISE     10002
#define ID_LOG        10003
#define ID_SERVERNAME 10004

extern void init();
extern wxString* nwnxhome;

class NWNXServerConnection : public wxConnection
{
public:
    NWNXServerConnection();
    ~NWNXServerConnection();
    virtual bool OnExecute(const wxString& topic, wxChar *data, int size, wxIPCFormat format);
    virtual wxChar *OnRequest(const wxString& topic, const wxString& item, int *size, wxIPCFormat format);
    virtual bool OnPoke(const wxString& topic, const wxString& item, wxChar *data, int size, wxIPCFormat format);
    virtual bool OnStartAdvise(const wxString& topic, const wxString& item);
    virtual bool OnStopAdvise(const wxString& topic, const wxString& item);
    virtual bool Advise(const wxString& item, wxChar *data, int size = -1, wxIPCFormat format = wxIPC_TEXT);
    virtual bool OnDisconnect();
protected:
    void Log(const wxString& command, const wxString& topic, const wxString& item, wxChar *data, int size, wxIPCFormat format);
public:
    wxString m_sAdvise;
protected:
    wxString m_sRequestDate;
    char m_achRequestBytes[3];
	bool initialized;
};

class NWNXServer: public wxServer
{
public:
    NWNXServer();
    ~NWNXServer();
    void Disconnect();
    bool IsConnected() { return m_connection != NULL; };
    NWNXServerConnection *GetConnection() { return m_connection; };
    void Advise();
    bool CanAdvise() { return m_connection != NULL && !m_connection->m_sAdvise.IsEmpty(); };
    wxConnectionBase *OnAcceptConnection(const wxString& topic);
	
protected:
    NWNXServerConnection *m_connection;

};

#endif