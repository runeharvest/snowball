// NeLNS - MMORPG Framework <http://dev.ryzom.com/projects/nel/>
// Copyright (C) 2010  Winch Gate Property Limited
//
// This source file has been modified by the following contributors:
// Copyright (C) 2025 Xackery <lordxackery@hotmail.com>
// Copyright (C) 2014 Jan BOON (Kaetemi) <jan.boon@kaetemi.be>
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

using namespace std;
using namespace NLMISC;
using namespace NLNET;

static CCallbackServer *ClientsServer = 0;

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
	std::string reason;
	sint32 uid = -1;
	ucstring login;
	std::string cpassword;
	std::string application;
	msgin.serial(login);
	msgin.serial(cpassword);
	msgin.serial(application);

    auto user = loginService->UserByLogin(login.toUtf8());
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

        domain::User newUser;
        newUser.Login = login.toUtf8();
        newUser.Password = cpassword;
        newUser.State = domain::UserState::Offline;
        user = loginService->UserCreate(newUser);
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

    if (user->State != domain::UserState::Offline)
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

    uid = user->UserID;

    CLoginCookie c;
    c.set((uint32)(uintptr_t)from, rand(), uid);

    user->Cookie = c.setToString();
    user->State = domain::UserState::Online;

    if (!loginService->UserUpdate(*user))
    {
        reason = "Failed to update user cookie";
        CMessage msgout("VLP");
        msgout.serial(reason);
        netbase.send(msgout, from);
        return;
    }

    auto shardsResult = loginService->ShardsByClientApplication(application);
    if (!shardsResult)
    {
        reason = toString("ShardsByClientApplication failed: %s", shardsResult.error());
        CMessage msgout("VLP");
        msgout.serial(reason);
        netbase.send(msgout, from);
        return;
    }
    auto &shards = *shardsResult;

    // Send success message
    CMessage msgout("VLP");
    msgout.serial(reason);

    uint32 shardCount = (uint32)shards.size();
    msgout.serial(shardCount);

    for (const auto &shard : shards)
    {
        ucstring shardName;
        shardName.fromUtf8(shard.Name);
        msgout.serial(shardName);
        uint32 playerCount = shard.PlayerCount;
        msgout.serial(playerCount);
		uint32 shardID = shard.ShardID;
        msgout.serial(shardID);
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

    auto usersResult = loginService->UsersByState(domain::UserState::Online);
    if (!usersResult)
    {
        reason = toString("UsersByState failed: %s", usersResult.error());
        CMessage msgout("SCS");
        msgout.serial(reason);
        netbase.send(msgout, from);
        return;
    }
    auto &users = *usersResult;
    if (users.empty())
    {
        reason = "No user found";
        CMessage msgout("SCS");
        msgout.serial(reason);
        netbase.send(msgout, from);
        return;
    }
    domain::User *user = nullptr;
    for (auto &u : users)
    {
        if (u.Cookie.empty())
        {
            continue;
        }

        CLoginCookie lc;
        lc.setFromString(u.Cookie);
        if (lc.getUserAddr() != (uint32)(uintptr_t)from)
        {
            continue;
        }

        user = &u;
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

    auto shardResult = loginService->ShardByShardID(shardid);
    if (!shardResult)
    {
        reason = "This shard is not available";
        CMessage msgout("SCS");
        msgout.serial(reason);
        netbase.send(msgout, from);
        return;
    }
    auto &shard = *shardResult;

    user->State = domain::UserState::Waiting;
    user->ShardID = shard.ShardID;

    if (!loginService->UserUpdate(*user))
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
    shardName.fromUtf8(shard.Name);
    userPrivilege.fromUtf8(user->Privilege);
    userExtendedPrivilege.fromUtf8(user->ExtendedPrivilege);
    msgout.serial(lc, shardName, userPrivilege, userExtendedPrivilege);
    CUnifiedNetwork::getInstance()->send(TServiceId(shard.ShardID), msgout);
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

    auto usersResp = loginService->Users();
    if (!usersResp)
    {
        nlwarning("users: %s", usersResp.error());
        return;
    }
    auto users = usersResp.value();
    if (users.empty())
    {
        return;
    }

    domain::User *user = nullptr;
    for (auto &u : users)
    {
        if (u.Cookie.empty())
        {
            continue;
        }

        CLoginCookie lc;
        lc.setFromString(u.Cookie);
        if (lc.getUserAddr() != (uint32)(uintptr_t)from)
        {
            continue;
        }

        if (u.State == domain::UserState::Offline)
        {
            nldebug("User %s (%d) already offline, ignoring disconnection from %s", u.Login.c_str(), u.UserID, ia.asString().c_str());
            return;
        }

        user = &u;
        break;
    }

    if (!user)
    {
        nldebug("No user found for disconnection from %s", ia.asString().c_str());
        return;
    }

    nlinfo("User %s (%d) disconnected from %s", user->Login.c_str(), user->UserID, ia.asString().c_str());
    user->State = domain::UserState::Offline;
    if (!loginService->UserUpdate(*user))
    {
        nlwarning("Failed to update user state for %s (%d)", user->Login.c_str(), user->UserID);
        return;
    }
}

const TCallbackItem ClientCallbackArray[] = {
	{ "VLP", cbClientVerifyLoginPassword },
	{ "CS", cbClientChooseShard },
};

static void cbWSShardChooseShard(CMessage &msgin, const std::string &serviceName, TServiceId sid)
{
	CMessage msgout("SCS");

	CLoginCookie cookie;
	string reason;
	msgin.serial(reason);
	msgin.serial(cookie);

    auto user = loginService->UserByCookie(cookie.toString());
    if (!user)
    {
        reason = "User not found for cookie";
        nldebug("SCS from WS failed: %s", reason.c_str());
        msgout.serial(reason);
        CUnifiedNetwork::getInstance()->send(serviceName, msgout);
        return;
    }

    if (user->State != domain::UserState::Online)
    {
        reason = "User is not online";
        nldebug("SCS from WS failed: %s", reason.c_str());
        msgout.serial(reason);
        CUnifiedNetwork::getInstance()->send(serviceName, msgout);
        return;
    }

    if (user->ShardID == 0)
    {
        reason = "User has no shard selected";
        nldebug("SCS from WS failed: %s", reason.c_str());
        msgout.serial(reason);
        CUnifiedNetwork::getInstance()->send(serviceName, msgout);
        return;
    }

    auto shard = loginService->ShardByShardID(user->ShardID);
    if (!shard)
    {
        reason = "Shard not found for user";
        nldebug("SCS from WS failed: %s", reason.c_str());
        msgout.serial(reason);
        CUnifiedNetwork::getInstance()->send(serviceName, msgout);
        return;
    }

    msgout.serial(reason);
    string str = cookie.setToString();
    msgout.serial(str);
    string addr;
    msgin.serial(addr);
    msgout.serial(addr);
    ClientsServer->send(msgout, (TSockId)cookie.getUserAddr()); // FIXME: 64-bit
}

static const TUnifiedCallbackItem WSCallbackArray[] = {
	{ "SCS", cbWSShardChooseShard },
};

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
