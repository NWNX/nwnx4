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
#include <vector>
#include <iostream>
#include <fstream>
#include <cstdarg>


LogNWNX::LogNWNX()
{
    LogNWNX::LogNWNX("test.txt");
    //m_outStream = new std::ostream(std::cout.rdbuf());
}
LogNWNX::LogNWNX(std::string filePath)
{
    this->filePath = filePath;

    m_ofStream.open(filePath, std::ofstream::out | std::ofstream::app);
    if (!m_ofStream) {
        std::cerr << "Canot open log file: " << filePath << std::endl;
    }
}


void LogNWNX::Trace(const char* format, ...){
    va_list args;
    va_start(args, format);
    Log((std::string("Trace:") + format).c_str(), args);
    va_end(args);
}
void LogNWNX::Debug(const char* format, ...){
    va_list args;
    va_start(args, format);
    Log((std::string("Dbg:  ") + format).c_str(), args);
    va_end(args);
}
void LogNWNX::Info(const char* format, ...){
    va_list args;
    va_start(args, format);
    Log((std::string("Info: ") + format).c_str(), args);
    va_end(args);
}
void LogNWNX::Warn(const char* format, ...){
    va_list args;
    va_start(args, format);
    Log((std::string("Warn: ") + format).c_str(), args);
    va_end(args);
}
void LogNWNX::Err(const char* format, ...){
    va_list args;
    va_start(args, format);
    Log((std::string("Err:  ") + format).c_str(), args);
    va_end(args);
}

void LogNWNX::Log(const char* format, va_list a){
     va_list args;
     va_copy(args, a);
     size_t len = std::vsnprintf(NULL, 0, format, args);
     va_end(args);
     std::vector<char> vec(len + 1);
     va_copy(args, a);
     std::vsnprintf(&vec[0], len + 1, format, args);
     va_end(args);
     LogStr(&vec[0]);
}
void LogNWNX::LogStr(const char* message){
    if (m_ofStream) {
        m_ofStream << message << std::endl;
        m_ofStream.flush();
    }
    else {
        std::cout << message << std::endl;
        std::cout.flush();
    }
}
