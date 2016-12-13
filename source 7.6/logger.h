//////////////////////////////////////////////////////////////////////
// OpenTibia - an opensource roleplaying game
//////////////////////////////////////////////////////////////////////
// Logger class - captures everything that happens on the server
//////////////////////////////////////////////////////////////////////
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//////////////////////////////////////////////////////////////////////


#ifndef __OTSERV_LOGGER_H__
#define __OTSERV_LOGGER_H__

#include "definitions.h"

#ifdef __GNUC__
#define __OTSERV_PRETTY_FUNCTION__ __PRETTY_FUNCTION__
#endif
#ifdef _MSC_VER 
#define __OTSERV_PRETTY_FUNCTION__ __FUNCDNAME__
#endif

/*
#define LOG_MESSAGE(channel, type, level, message) \
	Logger::getInstance()->logMessage(channel, type, level, message, __OTSERV_PRETTY_FUNCTION__, __LINE__, __FILE__);
*/

#define LOG_MESSAGE(channel, type, level, message) \
	Logger::getInstance()->logMessage(channel, type, level, message, __OTSERV_PRETTY_FUNCTION__);

#include <string>
#include <map>

enum eLogType {
	LOGTYPE_EVENT,
	LOGTYPE_WARNING,
	LOGTYPE_ERROR,
};

class Logger {
public:
	~Logger();
	static Logger* getInstance(){
		static Logger instance;
		return &instance;
	}
	/*void logMessage(std::string channel, eLogType type, int level,
			std::string message, std::string func,
			int line, std::string file);
	*/
	void logMessage(const char* channel, eLogType type, int level,
			std::string message, const char* func);
private:
	FILE* m_file;
	Logger();
};

#endif
