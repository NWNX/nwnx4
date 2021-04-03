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

#if !defined(LOG_H_INCLUDED)
#define LOG_H_INCLUDED

#include <string>
#include <fstream>

class LogNWNX
{
public:
    LogNWNX();
    LogNWNX(std::string);

    void Trace(const char* format, ...);
    void Debug(const char* format, ...);
    void Info(const char* format, ...);
    void Warn(const char* format, ...);
    void Err(const char* format, ...);
protected:
    std::string filePath;
    std::ofstream m_ofStream;

    void LogNWNX::Log(const char* format, va_list args);
    void LogNWNX::LogStr(const char* message);
};


#endif
