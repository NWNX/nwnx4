/***************************************************************************
    NWNX Log - Logging functions
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
#include "log.h"

wxLogNWNX::wxLogNWNX(FILE *fp)
{
    if (fp == NULL)
        fFile = stderr;
    else
        fFile = fp;

	this->append = false;
	this->timestamp = false;

	wxLog::SetActiveTarget(this);
	set_trace_mask();
	wxLogMessage(wxT("%s"), header);
}

wxLogNWNX::wxLogNWNX(FILE *fp, wxString header)
{
    if (fp == NULL)
        fFile = stderr;
    else
        fFile = fp;
	this->header = wxT("NWNX4 default header\n\n");
	this->append = false;
	this->timestamp = false;

	wxLog::SetActiveTarget(this);
	set_trace_mask();
	wxLogMessage(wxT("%s"), header);
}

wxLogNWNX::wxLogNWNX(wxString fName)
{
	this->fName = fName;
	this->maxSizeBytes = 1024 * 1024;
	this->header = wxT("NWNX4 default header\n\n");
	this->append = false;
	this->timestamp = false;
	create_log_file();
}

wxLogNWNX::wxLogNWNX(wxString fName, wxString header)
{
	this->fName = fName;
	this->maxSizeBytes = 1024 * 1024;
	this->header = header;
	this->append = false;
	this->timestamp = false;
	create_log_file();
}

wxLogNWNX::wxLogNWNX(wxString fName, wxString header, long maxSizeKB)
{
	this->fName = fName;
	this->maxSizeBytes = maxSizeKB * 1024;
	this->header = header;
	this->append = false;
	this->timestamp = false;
	create_log_file();
}

wxLogNWNX::wxLogNWNX(wxString fName, bool append, bool timestamp)
{
	this->fName = fName;
	this->maxSizeBytes = 1024 * 1024;
	this->header = wxT("NWNX4 default header\n\n");
	this->append = append;
	this->timestamp = timestamp;

	if (timestamp)
	{
		SetTimestamp( wxT( "%Y-%m-%d %H:%M:%S " ) );
	}
	create_log_file();
}

wxLogNWNX::wxLogNWNX(wxString fName, wxString header, bool append, bool timestamp)
{
	this->fName = fName;
	this->maxSizeBytes = 1024 * 1024;
	this->header = header;
	this->append = append;
	this->timestamp = timestamp;

	if (timestamp)
	{
		SetTimestamp( wxT( "%Y-%m-%d %H:%M:%S " ) );
	}
	create_log_file();
}


void wxLogNWNX::DoLogString(const wxChar *szString, time_t WXUNUSED(t))
{
/*
    wxString str;
    TimeStamp(&str);
    str << szString;

    fputs(str.mb_str(), fFile);
    fputc(_T('\n'), fFile);
    fflush(fFile);
*/
    wxString str;

	if (timestamp)
	{
		TimeStamp( &str );
	}

    str << szString;

    fputs(str.mb_str(), fFile);
    fputc(_T('\n'), fFile);
    fflush(fFile);
}

void wxLogNWNX::create_log_file()
{
	wxString openMode;

	if (append)
		openMode = wxT("a");
	else
		openMode = wxT("w");

	fFile = _tfopen(fName, openMode);
	wxLog::SetActiveTarget(this);
	set_trace_mask();
	wxLogMessage(wxT("%s"), header);
}

void wxLogNWNX::set_trace_mask()
{
	wxLog::AddTraceMask(TRACE_NORMAL);
	wxLog::AddTraceMask(TRACE_VERBOSE);
}