/***************************************************************************
    NWNX Mysql - Database plugin for MySQL
    Copyright (C) 2007 Ingmar Stieger (Papillon, papillon@blackdagger.com) 

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
#if !defined(MYSQL_H_INCLUDED)
#define MYSQL_H_INCLUDED

#define DLLEXPORT extern "C" __declspec(dllexport)
#define CR_SERVER_GONE_ERROR 2006

typedef unsigned long ulong;

#include "../dbplugin.h"
#include <mysql.h>

class MySQL : public DBPlugin
{
public:
	MySQL();
	~MySQL();

	bool Init(TCHAR* nwnxhome);  

	BOOL WriteScorcoData(BYTE* pData, int Length);
	BYTE* ReadScorcoData(char *param, int *size);

private:
	bool Connect();
	void Disconnect();
	bool Reconnect();
	bool Execute(char* query);
	int Fetch(char* buffer);
	int GetData(int iCol, char* buffer);
	int GetAffectedRows();
	void GetEscapeString(char* str, char* buffer);
	MYSQL_RES* AdvanceToNextValidResultset();

	MYSQL mysql;
	MYSQL* connection;
	MYSQL_RES *result;
	MYSQL_ROW row;
	
	unsigned int num_fields;

	wxString server;
	wxString user;
	wxString password;
	wxString schema;

};

#endif