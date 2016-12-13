//////////////////////////////////////////////////////////////////////
// OpenTibia - an opensource roleplaying game
//////////////////////////////////////////////////////////////////////
//
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

#ifndef __OTSERV_TOOLS_H__
#define __OTSERV_TOOLS_H__

#include "const76.h"
#include "position.h"

#include <string>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <vector>

#include <libxml/parser.h>


enum DistributionType_t{
	DISTRO_UNIFORM,
	DISTRO_SQUARE,
	DISTRO_NORMAL
};

bool fileExists(const char* filename);
void replaceString(std::string& str, const std::string sought, const std::string replacement);
void trim_right(std::string& source, const std::string& t);
void trim_left(std::string& source, const std::string& t);
void toLowerCaseString(std::string& source);
void toUpperCaseString(std::string& source);
std::string asLowerCaseString(const std::string& source);
std::string asUpperCaseString(const std::string& source);
bool utf8ToLatin1(char* intext, std::string& outtext);
bool readXMLInteger(xmlNodePtr node, const char* tag, int& value);
#if (defined __WINDOWS__ || defined WIN32) && !defined __GNUC__
bool readXMLInteger(xmlNodePtr node, const char* tag, int32_t& value);
#endif
bool readXMLInteger64(xmlNodePtr node, const char* tag, uint64_t& value);
bool readXMLFloat(xmlNodePtr node, const char* tag, float& value);
bool readXMLString(xmlNodePtr node, const char* tag, std::string& value);
bool readXMLContentString(xmlNodePtr node, std::string& value);
std::vector<std::string> explodeString(const std::string& inString, const std::string& separator);
bool hasBitSet(uint32_t flag, uint32_t flags);

int random_range(int lowest_number, int highest_number, DistributionType_t type = DISTRO_UNIFORM);

void hexdump(unsigned char *_data, int _len);
char upchar(char c);

std::string urlEncode(const char* str);
std::string urlEncode(const std::string& str);

bool passwordTest(const std::string &plain, std::string &hash);

Position getNextPosition(Direction direction, Position pos);

//buffer should be at least 17 bytes
void formatIP(uint32_t ip, char* buffer);
//buffer should have at least 21 bytes. dd/mm/yyyy  hh:mm:ss
void formatDate(time_t time, char* buffer);
//buffer should have at least 16 bytes
void formatDate2(time_t time, char* buffer);
std::string formatTime(int32_t hours, int32_t minutes);

MagicEffectClasses getMagicEffect(const std::string& strValue);
ShootType_t getShootType(const std::string& strValue);
Ammo_t getAmmoType(const std::string& strValue);
AmmoAction_t getAmmoAction(const std::string& strValue);

#endif
