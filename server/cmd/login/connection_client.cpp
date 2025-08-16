// NeLNS - MMORPG Framework <http://dev.ryzom.com/projects/nel/>
// Copyright (C) 2010  Winch Gate Property Limited
//
// This source file has been modified by the following contributors:
// Copyright (C) 2014  Jan BOON (Kaetemi) <jan.boon@kaetemi.be>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

//
// Includes
//

#include <nel/misc/types_nl.h>

#include <math.h>
#include <stdio.h>
#include <ctype.h>

#include <map>
#include <vector>

#include <nel/misc/log.h>
#include <nel/misc/debug.h>
#include <nel/misc/displayer.h>
#include <nel/misc/config_file.h>

#include <nel/net/service.h>
#include <nel/net/login_cookie.h>

#include "login.h"
#include "mysql_helper.h"

//
// Namespaces
//

using namespace std;
using namespace NLMISC;
using namespace NLNET;

//
// Variables
//

static CCallbackServer *ClientsServer = 0;

//
// Functions
//

void sendToClient(CMessage &msgout, TSockId sockId)
{
	nlassert(ClientsServer != 0);
	ClientsServer->send(msgout, sockId);
}

void string_escape(string &str)
{
	string::size_type pos = 0;
	while ((pos = str.find('\'')) != string::npos)
	{
		str.replace(pos, 1, " ");
	}
}

static void cbClientVerifyLoginPassword(CMessage &msgin, TSockId from, CCallbackNetBase &netbase)
{
	string reason;
	sint32 uid = -1;
	ucstring login;
	string cpassword;
	string application;
	msgin.serial(login);
	msgin.serial(cpassword);
	msgin.serial(application);

	auto user = login_service->UserByLogin(login.toUtf8());
	if (!user)
	{
		if (IService::getInstance()->ConfigFile.getVar("AcceptUnknownUsers").asInt() != 1)
		{
			reason = toString("Login '%s' doesn't exist", login.toUtf8().c_str());
			CMessage msgout("VLP");
			msgout.serial(reason);
			netbase.send(msgout, from);
			return;
		}

		user->Password = cpassword;
		user = login_service->UserCreate(*user);
		if (!user)
		{
			reason = "Failed to create user";
			CMessage msgout("VLP");
			msgout.serial(reason);
			netbase.send(msgout, from);
			return;
		}
		nlinfo("The user %s was created for the application '%s'!", login.toUtf8().c_str(), application.c_str());
	}

	if (cpassword != user->Password)
	{
		reason = toString("Bad password");
		CMessage msgout("VLP");
		msgout.serial(reason);
		netbase.send(msgout, from);
		return;
	}

	if (user->State != UserState::Offline)
	{
		// disconnect everyone
		CMessage msgout("DC");
		msgout.serial(uid);
		CUnifiedNetwork::getInstance()->send("WS", msgout);

		reason = toString("User '%s' is already connected", login.toUtf8().c_str());
		CMessage vplMsgout("VLP");
		vplMsgout.serial(reason);
		netbase.send(vplMsgout, from);
		return;
	}

	uid = user->UId;

	CLoginCookie c;
	c.set((uint32)(uintptr_t)from, rand(), uid);

	user->Cookie = c.setToString();
	user->State = UserState::Online;

	if (!login_service->UserUpdate(*user))
	{
		reason = "Failed to update user cookie";
		CMessage msgout("VLP");
		msgout.serial(reason);
		netbase.send(msgout, from);
		return;
	}

	auto shards = login_service->ShardsByClientApplication(application);
	if (shards.empty())
	{
		reason = toString("No shard available for the application '%s'", application.c_str());
		CMessage msgout("VLP");
		msgout.serial(reason);
		netbase.send(msgout, from);
		return;
	}

	// Send success message
	CMessage msgout("VLP");
	msgout.serial(reason);
	msgout.serial(uid);

	for (const auto &shard : shards)
	{
		ucstring shardname;
		shardname.fromUtf8(shard->Name);
		msgout.serial(shardname, shard->PlayerCount, shard->ShardID);
	}

	netbase.send(msgout, from);
	netbase.authorizeOnly("CS", from);

	return;
}

static void cbClientChooseShard(CMessage &msgin, TSockId from, CCallbackNetBase &netbase)
{
	string reason;

	sint32 shardid;
	msgin.serial(shardid);

	auto users = login_service->UsersByState(UserState::Online);
	if (users.empty())
	{
		reason = "No user found";
		CMessage msgout("SCS");
		msgout.serial(reason);
		netbase.send(msgout, from);
		return;
	}

	std::shared_ptr<User> user;

	for (const auto &u : users)
	{
		if (u->Cookie.empty())
		{
			continue;
		}

		CLoginCookie lc;
		lc.setFromString(u->Cookie);
		if (lc.getUserAddr() != (uint32)(uintptr_t)from)
		{
			continue;
		}

		user = u;
		break;
	}
	if (!user)
	{
		reason = "You are not authorized to select a shard";
		CMessage msgout("SCS");
		msgout.serial(reason);
		netbase.send(msgout, from);
		return;
	}

	auto shard = login_service->ShardByShardID(shardid);
	if (!shard)
	{
		reason = "This shard is not available";
		CMessage msgout("SCS");
		msgout.serial(reason);
		netbase.send(msgout, from);
		return;
	}

	user->State = UserState::Waiting;
	user->ShardID = shard->ShardID;

	if (!login_service->UserUpdate(*user))
	{
		reason = "Failed to update user state";
		CMessage msgout("SCS");
		msgout.serial(reason);
		netbase.send(msgout, from);
		return;
	}

	ucstring loginName;
	loginName.fromUtf8(user->Login);
	CLoginCookie lc;
	lc.setFromString(user->Cookie);
	CMessage msgout("CS");
	ucstring shardName, userPrivilege, userExtendedPrivilege;
	shardName.fromUtf8(shard->Name);
	userPrivilege.fromUtf8(user->Privilege);
	userExtendedPrivilege.fromUtf8(user->ExtendedPrivilege);
	msgout.serial(lc, shardName, userPrivilege, userExtendedPrivilege);
	CUnifiedNetwork::getInstance()->send(TServiceId(shard->ShardID), msgout);
}

static void cbClientConnection(TSockId from, void *arg)
{
	CCallbackNetBase *cnb = ClientsServer;
	const CInetAddress &ia = cnb->hostAddress(from);
	nldebug("new client connection: %s", ia.asString().c_str());
	Output->displayNL("CCC: Connection from %s", ia.asString().c_str());
	cnb->authorizeOnly("VLP", from);
}

static void cbClientDisconnection(TSockId from, void *arg)
{
	CCallbackNetBase *cnb = ClientsServer;
	const CInetAddress &ia = cnb->hostAddress(from);

	nldebug("new client disconnection: %s", ia.asString().c_str());

	string reason;

	CMysqlResult result;
	MYSQL_ROW row;
	sint32 nbrow;
	// sql: userByState
	reason = sqlQuery("select UId, State, Cookie from user where State!='Offline'", nbrow, row, result);
	if (!reason.empty()) return;

	if (nbrow == 0)
	{
		return;
	}

	while (row != 0)
	{
		CLoginCookie lc;
		string str = row[2];
		if (!str.empty())
		{
			lc.setFromString(str);
			if (lc.getUserAddr() == (uint32)(uintptr_t)from)
			{
				// got it, if he is not in waiting state, it s not normal, remove all
				if (row[1] == string("Authorized"))
				{
					// sql: userUpdate
					sqlQuery("update user set state='Offline', ShardId=-1, Cookie='' where UId=" + string(row[0]));
				}
				return;
			}
		}
		row = mysql_fetch_row(result);
	}
}

const TCallbackItem ClientCallbackArray[] = {
	{ "VLP", cbClientVerifyLoginPassword },
	{ "CS", cbClientChooseShard },
};

static void cbWSShardChooseShard(CMessage &msgin, const std::string &serviceName, TServiceId sid)
{
	//
	// S10: receive "SCS" message from WS
	//
	CMessage msgout("SCS");

	CLoginCookie cookie;
	string reason;

	breakable
	{
		msgin.serial(reason);
		msgin.serial(cookie);

		if (!reason.empty())
		{
			nldebug("SCS from WS failed: %s", reason.c_str());
			// sql: userUpdate
			sqlQuery("update user set state='Offline', ShardId=-1, Cookie='' where Cookie='" + cookie.setToString() + "'");
			break;
		}

		CMysqlResult result;
		MYSQL_ROW row;
		sint32 nbrow;
		// sql: userByCookie
		reason = sqlQuery("select UId, Cookie, Privilege, ExtendedPrivilege from user where Cookie='" + cookie.setToString() + "'", nbrow, row, result);
		if (!reason.empty()) break;
		if (nbrow != 1)
		{
			reason = "More than one row was found";
			nldebug("SCS from WS failed with duplicate cookies, sending disconnect messages.");
			// disconnect them all
			while (row != 0)
			{
				CMessage msgout("DC");
				uint32 uid = atoui(row[0]);
				msgout.serial(uid);
				CUnifiedNetwork::getInstance()->send("WS", msgout);
				row = mysql_fetch_row(result);
			}
			break;
		}

		msgout.serial(reason);
		string str = cookie.setToString();
		msgout.serial(str);
		string addr;
		msgin.serial(addr);
		msgout.serial(addr);
		ClientsServer->send(msgout, (TSockId)cookie.getUserAddr()); // FIXME: 64-bit
		return;
	}
	msgout.serial(reason);
	ClientsServer->send(msgout, (TSockId)cookie.getUserAddr()); // FIXME: 64-bit
}

static const TUnifiedCallbackItem WSCallbackArray[] = {
	{ "SCS", cbWSShardChooseShard },
};

//
//
//

void connectionClientInit()
{
	nlassert(ClientsServer == 0);

	ClientsServer = new CCallbackServer();
	nlassert(ClientsServer != 0);

	uint16 port = (uint16)IService::getInstance()->ConfigFile.getVar("ClientsPort").asInt();
	ClientsServer->init(port);

	ClientsServer->addCallbackArray(ClientCallbackArray, sizeof(ClientCallbackArray) / sizeof(ClientCallbackArray[0]));
	ClientsServer->setConnectionCallback(cbClientConnection, 0);
	ClientsServer->setDisconnectionCallback(cbClientDisconnection, 0);

	// catch the messages from Welcome Service to know if the user can connect or not
	CUnifiedNetwork::getInstance()->addCallbackArray(WSCallbackArray, sizeof(WSCallbackArray) / sizeof(WSCallbackArray[0]));
}

void connectionClientUpdate()
{
	nlassert(ClientsServer != 0);

	try
	{
		ClientsServer->update();
	}
	catch (Exception &e)
	{
		nlwarning("Error during update: '%s'", e.what());
	}
}

void connectionClientRelease()
{
	nlassert(ClientsServer != 0);

	delete ClientsServer;
	ClientsServer = 0;
}
