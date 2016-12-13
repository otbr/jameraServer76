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

#ifndef __OTSERV_PARTY_H__
#define __OTSERV_PARTY_H__

#include "definitions.h"
#include "const76.h"

#include <list>
#include <vector>
#include <map>

class Player;
class Party;
class Creature;

typedef std::vector<Player*> PlayerVector;

class Party
{
public:
	Party(Player* _leader);
	~Party();

	Player* getLeader() const {return leader;}
	void setLeader(Player* _leader) {leader = _leader;}

	void disband();
	bool invitePlayer(Player* player);
	bool joinParty(Player* player);
	bool revokeInvitation(Player* player);
	bool passPartyLeadership(Player* player);
	bool leaveParty(Player* player);
	
	bool removeInvite(Player* player);

	bool isPlayerMember(const Player* player) const;
	bool isPlayerInvited(const Player* player) const;
	void updatePartyIcons(Player* player, PartyShields_t shield);
	void updateInvitationIcons(Player* player, PartyShields_t shield);
	void broadcastPartyMessage(MessageClasses msgClass, const std::string& msg, bool sendToInvitations = false);
	bool disbandParty() {return (memberList.empty() && inviteList.empty());}

	const PlayerVector& getMemberList() {return memberList;}

protected:
	Player* leader;
	PlayerVector memberList;
	PlayerVector inviteList;
};

#endif
