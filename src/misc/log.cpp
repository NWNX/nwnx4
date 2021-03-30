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
#include "log.h"
#include <iostream>
#include <fstream>
#include <cstdarg>

LogNWNX::LogNWNX(std::string fName)
{
	this->fName = fName;
	this->header = "NWNX4 default header\n\n";
	CreateLogFile();
}

LogNWNX::LogNWNX(std::string fName, std::string header)
{
	this->fName = fName;
	this->header = header;
	CreateLogFile();
}

void LogNWNX::Trace(const std::string format...){
    va_list args;
    va_start(args, format);
    Log(std::string("Trace:") + format, args);
}
void LogNWNX::Debug(const std::string format...){
    va_list args;
    va_start(args, format);
    Log(std::string("Dbg:  ") + format, args);
}
void LogNWNX::Info(const std::string format...){
    va_list args;
    va_start(args, format);
    Log(std::string("Info: ") + format, args);
}
void LogNWNX::Warn(const std::string format...){
    va_list args;
    va_start(args, format);
    Log(std::string("Warn: ") + format, args);
}
void LogNWNX::Err(const std::string format...){
    va_list args;
    va_start(args, format);
    Log(std::string("Err:  ") + format, args);
}

void LogNWNX::Log(const std::string format...){
    va_list args;
    va_start(args, format);
    int size_s = snprintf( nullptr, 0, format.c_str(), args) + 1; // Extra space for '\0'
    if( size_s <= 0 )
        throw std::runtime_error( "Error during formatting." );
    size_t size = size_s;
    auto buf = std::make_unique<char[]>(size);
    snprintf( buf.get(), size, format.c_str(), args);
    LogStr(std::string( buf.get(), buf.get() + size - 1 ));
}
void LogNWNX::LogStr(const std::string message){
    m_fileStream << message << std::endl;
    m_fileStream.flush();
}


void LogNWNX::CreateLogFile()
{
    m_fileStream = std::ofstream(fName);

    if (!m_fileStream){
        std::cerr << "Canot open log file: " << fName << std::endl;
    }
    else{
        Log(this->header);
    }
}
