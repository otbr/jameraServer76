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

#ifndef __OTSERV_PROTOCOL_H__
#define __OTSERV_PROTOCOL_H__

#include <stdio.h>
#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>

class NetworkMessage;
class OutputMessage;
class Connection;
typedef boost::shared_ptr<OutputMessage> OutputMessage_ptr;

#define CLIENT_VERSION_MIN 760
#define CLIENT_VERSION_MAX 760

#define STRING_CLIENT_VERSION "This server requires client version 7.6."

class Protocol : boost::noncopyable
{
public:
	Protocol(Connection* connection)
	{
		m_connection = connection;
		m_rawMessages = false;
		m_refCount = 0;
	}

	virtual ~Protocol() {}

	virtual void parsePacket(NetworkMessage& msg){};

	void onSendMessage(OutputMessage_ptr msg);
	void onRecvMessage(NetworkMessage& msg);
	virtual void onRecvFirstMessage(NetworkMessage& msg) = 0;

	Connection* getConnection() { return m_connection;}
	const Connection* getConnection() const { return m_connection;}
	void setConnection(Connection* connection) { m_connection = connection; }

	uint32_t getIP() const;
	int32_t addRef() {return ++m_refCount;}
	int32_t unRef() {return --m_refCount;}

protected:
	//Use this function for autosend messages only
	OutputMessage_ptr getOutputBuffer();
	
	void setRawMessages(bool value) { m_rawMessages = value; }

	virtual void releaseProtocol();
	virtual void deleteProtocolTask();
	friend class Connection;

private:

	OutputMessage_ptr m_outputBuffer;
	Connection* m_connection;
	bool m_rawMessages;
	uint32_t m_refCount;
};

#endif
