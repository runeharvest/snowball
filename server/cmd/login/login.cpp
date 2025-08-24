// NeLNS - MMORPG Framework <http://dev.ryzom.com/projects/nel/>
// Copyright (C) 2010  Winch Gate Property Limited
//
// This source file has been modified by the following contributors:
// Copyright (C) 2025 Xackery <lordxackery@hotmail.com>
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#ifndef NELNS_CONFIG
#define NELNS_CONFIG ""
#endif // NELNS_CONFIG

#ifndef NELNS_LOGS
#define NELNS_LOGS ""
#endif // NELNS_LOGS

#ifndef NELNS_STATE
#define NELNS_STATE ""
#endif // NELNS_STATE

#include "nel/misc/types_nl.h"

#include <cstdio>
#include <ctype.h>
#include <cmath>

#include <vector>
#include <map>

#include "nel/misc/debug.h"
#include "nel/misc/config_file.h"
#include "nel/misc/displayer.h"
#include "nel/misc/command.h"
#include "nel/misc/log.h"
#include "nel/misc/window_displayer.h"
#include "nel/misc/path.h"

#include "nel/net/service.h"
#include "nel/net/login_cookie.h"

#include "login.h"
#include "connection_client.h"
#include "connection_ws.h"
#include "login_service.h"
#include "storage/memory/shard_memory.h"
#include "storage/memory/user_memory.h"
#include "message/nel/nel_message.h"
#include <domain/user.h>
#include <domain/shard.h>
#include "config_toml.h" // Add this include if ConfigToml is defined here

//
// Namespaces
//

using namespace std;
using namespace NLMISC;
using namespace NLNET;

//
// Variables
//

// store specific user information
NLMISC::CFileDisplayer *Fd = NULL;
NLMISC::CStdDisplayer Sd;
NLMISC::CLog *Output = NULL;

// uint32 CUser::NextUserId = 1;	// 0 is reserved

// vector<CUser>	Users;
// vector<CShard>	Shards;

vector<CShard> Shards;
LoginService *loginService = nullptr;
static ShardMemory shardMemory;
static UserMemory userMemory;

ConfigService *configService = nullptr;

NetworkService *netListener = nullptr;
static NelMessage nelMessage;

//
// Functions
//

sint findShard(sint32 shardId)
{
	for (sint i = 0; i < (sint)Shards.size(); i++)
	{
		if (Shards[i].ShardId == shardId)
		{
			return i;
		}
	}
	// shard not found
	return -1;
}

sint findShardWithSId(TServiceId sid)
{
	for (sint i = 0; i < (sint)Shards.size(); i++)
	{
		if (Shards[i].SId == sid)
		{
			return i;
		}
	}
	// shard not found
	return -1;
}

// transform "192.168.1.1:80" into "192.168.1.1"
string removePort(const string &addr)
{
	return addr.substr(0, addr.find(":"));
}

void checkClients()
{
	nldebug("checkClients ()");
	/*for (uint i = 0; i < Users.size (); i++)
	{
	    switch (Users[i].State)
	    {
	    case CUser::Offline:
	        nlassert (Users[i].SockId == NULL);
	        nlassert (!Users[i].Cookie.isValid());
	        nlassert (Users[i].ShardId == NULL);
	        break;
	    case CUser::Authorized:
	        nlassert (Users[i].SockId != NULL);
	        nlassert (Users[i].Cookie.isValid());
	        nlassert (Users[i].ShardId == NULL);
	        break;
	    case CUser::Awaiting:
	        nlassert (Users[i].SockId == NULL);
	        nlassert (!Users[i].Cookie.isValid());
	        nlassert (Users[i].ShardId == NULL);
	        break;
	    case CUser::Online:
	        nlassert (Users[i].SockId == NULL);
	        nlassert (!Users[i].Cookie.isValid());
	        nlassert (Users[i].ShardId != NULL);
	        break;
	    default:
	        nlstop;
	        break;
	    }
	}*/
}

/*void disconnectClient (CUser &user, bool disconnectClient, bool disconnectShard)
{
    nlinfo ("User %d '%s' need to be disconnect (%d %d %d)", user.Id, user.Login.c_str(), user.State, disconnectClient, disconnectShard);

    switch (user.State)
    {
    case CUser::Offline:
        break;

    case CUser::Authorized:
        if (disconnectClient)
        {
            CNetManager::getNetBase("LS")->disconnect(user.SockId);
        }
        user.Cookie.clear ();
        break;

    case CUser::Awaiting:
        break;

    case CUser::Online:
        if (disconnectShard)
        {
            // ask the WS to disconnect the player from the shard
            CMessage msgout (CNetManager::getNetBase("WSLS")->getSIDA (), "DC");
            msgout.serial (user.Id);
            CNetManager::send ("WSLS", msgout, user.ShardId);
        }
        user.ShardId = NULL;
        break;

    default:
        nlstop;
        break;
    }

    user.SockId = NULL;
    user.State = CUser::Offline;
}
*/
sint findUser(uint32 Id)
{
	/*	for (sint i = 0; i < (sint) Users.size (); i++)
	    {
	        if (Users[i].Id == Id)
	        {
	            return i;
	        }
	    }
	    // user not found
	*/
	return -1;
}
/*
string CUser::Authorize (TSockId sender, CCallbackNetBase &netbase)
{
    string reason;

    switch (State)
    {
    case Offline:
        State = Authorized;
        SockId = sender;
        Cookie = CLoginCookie(netbase.hostAddress(sender).internalIPAddress(), Id);
        break;

    case Authorized:
        nlwarning ("user %d already authorized! disconnect him and the other one", Id);
        reason = "You are already authorized (another user uses your account?)";
        disconnectClient (*this, true, true);
        disconnectClient (Users[findUser(Id)], true, true);
        break;

    case Awaiting:
        nlwarning ("user %d already awaiting! disconnect the new user and the other one", Id);
        reason = "You are already awaiting (another user uses your account?)";
        disconnectClient (*this, true, true);
        disconnectClient (Users[findUser(Id)], true, true);
        break;

    case Online:
        nlwarning ("user %d already online! disconnect the new user and the other one", Id);
        reason = "You are already online (another user uses your account?)";
        disconnectClient (*this, true, true);
        disconnectClient (Users[findUser(Id)], true, true);
        break;

    default:
        reason = "default case should never occurs, there's a bug in the login.cpp";
        nlstop;
        break;
    }
    return reason;
}
*/
void displayShards()
{
	ICommand::execute("shards", *InfoLog);
}

void displayUsers()
{
	ICommand::execute("users", *InfoLog);
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
/////////////// SERVICE IMPLEMENTATION /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

void configValidate()
{

	auto displayedVariablesResult = configService->ValuesResult("login", "displayed_variables");
	if (!displayedVariablesResult)
	{
		nlerror("Failed to validate displayed_variables: %s", displayedVariablesResult.error().c_str());
	}

	auto wsPortResult = configService->ValueInt32Result("login", "ws_port");
	if (!wsPortResult)
	{
		nlerror("Failed to validate ws_port: %s", wsPortResult.error().c_str());
	}

	auto webPortResult = configService->ValueInt32Result("login", "web_port");
	if (!webPortResult)
	{
		nlerror("Failed to validate web_port: %s", webPortResult.error().c_str());
	}

	auto clientPortResult = configService->ValueInt32Result("login", "client_port");
	if (!clientPortResult)
	{
		nlerror("Failed to validate client_port: %s", clientPortResult.error().c_str());
	}

	auto isExteranlShardAllowed = configService->ValueBoolResult("login", "is_external_shard_allowed");
	if (!isExteranlShardAllowed)
	{
		nlerror("Failed to validate is_external_shard_allowed: %s", isExteranlShardAllowed.error().c_str());
	}

	auto isUnknownUserAllowedResult = configService->ValueBoolResult("login", "is_unknown_user_allowed");
	if (!isUnknownUserAllowedResult)
	{
		nlerror("Failed to validate is_unknown_user_allowed: %s", isUnknownUserAllowedResult.error().c_str());
	}

	auto isUnknownUserCreationAllowedResult = configService->ValueBoolResult("login", "is_user_creation_allowed");
	if (!isUnknownUserCreationAllowedResult)
	{
		nlerror("Failed to validate is_user_creation_allowed: %s", isUnknownUserCreationAllowedResult.error().c_str());
	}

	auto beepResult = configService->ValueBoolResult("login", "beep");
	if (!beepResult)
	{
		nlerror("Failed to validate beep: %s", beepResult.error().c_str());
	}

	auto databaseHostResult = configService->ValueResult("login", "database_host");
	if (!databaseHostResult)
	{
		nlerror("Failed to validate database host: %s", databaseHostResult.error().c_str());
	}

	auto databaseNameResult = configService->ValueResult("login", "database_name");
	if (!databaseNameResult)
	{
		nlerror("Failed to validate database name: %s", databaseNameResult.error().c_str());
	}

	auto databaseUserResult = configService->ValueResult("login", "database_username");
	if (!databaseUserResult)
	{
		nlerror("Failed to validate database user: %s", databaseUserResult.error().c_str());
	}

	auto databasePasswordResult = configService->ValueResult("login", "database_password");
	if (!databasePasswordResult)
	{
		nlerror("Failed to validate database password: %s", databasePasswordResult.error().c_str());
	}

	auto databaseForceReconnectionResult = configService->ValueResult("login", "force_database_reconnection");
	if (!databaseForceReconnectionResult)
	{
		nlerror("Failed to validate force_database_reconnection: %s", databaseForceReconnectionResult.error().c_str());
	}

	auto isNamingServiceUsedResult = configService->ValueBoolResult("login", "is_naming_service_used");
	if (!isNamingServiceUsedResult)
	{
		nlerror("Failed to validate is_naming_service_used: %s", isNamingServiceUsedResult.error().c_str());
	}

	auto isAesUsed = configService->ValueBoolResult("login", "is_aes_used");
	if (!isAesUsed)
	{
		nlerror("Failed to validate is_aes_used: %s", isAesUsed.error().c_str());
	}

	auto shardIDResult = configService->ValueInt32Result("login", "shard_id");
	if (!shardIDResult)
	{
		nlerror("Failed to validate shard_id: %s", shardIDResult.error().c_str());
	}
}

void beep(uint freq, uint nb, uint beepDuration, uint pauseDuration)
{
#ifdef NL_OS_WINDOWS
	try
	{
		if (configService->ValueBool("login", "beep") == false) return;
		for (uint i = 0; i < nb; i++)
		{
			Beep(freq, beepDuration);
			nlSleep(pauseDuration);
		}
	}
	catch (Exception &)
	{
	}
#endif // NL_OS_WINDOWS
}

class CLogin : public IService
{
public:
	CLogin()
	{
	}

	/// Init the service, load the universal time.
	void init()
	{
		beep();

		auto configToml = new ConfigToml();
		configService = new ConfigService(*configToml);
		auto parseResult = configService->Load("config/", "login");
		if (!parseResult)
		{
			nlerror("Failed to load config: %s", parseResult.error().c_str());
		}

		configValidate();

		netListener = new NetworkService(nelMessage);
		auto netInitResult = netListener->Listen("", (uint8_t)8191);
		if (!netInitResult)
		{
			nlerror("Failed to initialize network listener: %s", netInitResult.error().c_str());
		}

		auto nelConnectMessage = new NelMessage();

		auto netConnect = new NetworkService(*nelConnectMessage);
		auto netConnectResult = netConnect->Connect("127.0.0.1", (uint8_t)8191);
		if (!netConnectResult)
		{
			nlerror("Failed to connect to network: %s", netConnectResult.error().c_str());
		}

		nlinfo("network connected?");
		auto sendResult = netConnect->SendRaw("hello world!");
		if (!sendResult)
		{
			nlerror("Failed to send: %s", sendResult.error().c_str());
		}

		loginService = new LoginService(shardMemory, userMemory);

		domain::Shard shard;
		shard.Account = "Test";
		loginService->ShardCreate(shard);
		// nlinfo("shard created with id %d", shard.ShardID);
        // auto newShard = loginService->ShardByShardID(0);
        // if (newShard == nullptr)
        // {
        // 	nlerror("new shard not found");
        // 	return;
        // }
        // nlinfo("shard account found: %s", newShard->Account.c_str());

        domain::Shard testShard;
        testShard.Account = "SnowBall Service";
        testShard.ClientApplication = "snowball";
        auto shardResult = loginService->ShardCreate(testShard);
        nlinfo("shard created with id %d", shardResult->ShardID);

        Output = new CLog;

        string fn = IService::getInstance()->SaveFilesDirectory;
        fn += "login.stat";
        nlinfo("Login stat in directory '%s'", fn.c_str());
        Fd = new NLMISC::CFileDisplayer(fn);
        Output->addDisplayer(Fd);
        if (WindowDisplayer) Output->addDisplayer(WindowDisplayer);

        // Initialize the database access
        // sqlInit();

        connectionWSInit();

        connectionClientInit();

        Output->displayNL("Login Service initialized");
    }

    bool update()
    {
        connectionWSUpdate();
        connectionClientUpdate();
		return true;
    }

    /// release the service, save the universal time
    void release()
	{
		connectionWSRelease();
		connectionClientRelease();

		Output->displayNL("Login Service released");
	}

private:
};

// Service instantiation
NLNET_SERVICE_MAIN(CLogin, "LS", "login", 49999, EmptyCallbackArray, NELNS_CONFIG, NELNS_LOGS);

//
// Variables
//

NLMISC_DYNVARIABLE(uint, OnlineUsersNumber, "number of actually connected users")
{
	// we can only read the value
	if (get)
	{
		// uint32 nbusers = 0;
		// for (uint i = 0; i < Users.size(); i++)
		//{
		//	if (Users[i].State == CUser::Online)
		//		nbusers++;
		// }
		*pointer = NbPlayers;
	}
}

//
// Commands
//

NLMISC_COMMAND(shards, "displays the list of all registered shards", "")
{
	if (args.size() != 0) return false;

	log.displayNL("Display the %d registered shards :", Shards.size());
	for (uint i = 0; i < Shards.size(); i++)
	{
		log.displayNL("* ShardId: %d SId: %d NbPlayers: %d", Shards[i].ShardId, Shards[i].SId.get(), Shards[i].NbPlayers);
		CUnifiedNetwork::getInstance()->displayUnifiedConnection(Shards[i].SId, &log);
	}
	log.displayNL("End of the list");

	return true;
}

NLMISC_COMMAND(shardDatabase, "displays the list of all shards in the database", "")
{
	if (args.size() != 0) return false;

    auto shardsResult = loginService->Shards();
    if (!shardsResult)
    {
        nlwarning("shards: %s", shardsResult.error());
        return true;
    }
    auto &shards = *shardsResult;
    if (shards.size() == 0)
    {
        log.displayNL("No shards found in the database");
        return true;
    }

    log.displayNL("Display the %d shards in the database :", shards.size());
    log.displayNL(" > ShardId, WsAddr, NbPlayers, Name, Online, ClientApplication, Version, DynPatchURL");

    for (const auto &shard : shards)
    {
        log.displayNL(" > %d '%s' %d '%s' %d '%s' '%s' '%s'", shard.ShardID, shard.WSAddr.c_str(), shard.PlayerCount, shard.Name.c_str(),
            shard.IsOnline, shard.ClientApplication.c_str(), shard.Version.c_str(), shard.DynPatchURL.c_str());
    }

    log.displayNL("End of the list");

    return true;
}

NLMISC_COMMAND(registeredUsers, "displays the list of all registered users", "")
{
	if (args.size() != 0) return false;

    auto usersResult = loginService->Users();
    if (!usersResult)
    {
        log.displayNL("Users failed: %s", usersResult.error());
        return true;
    }
    auto users = usersResult.value();
    if (users.empty())
    {
        log.displayNL("No registered users found in the database");
        return true;
    }
    log.displayNL("Display the %d registered users:", users.size());
    log.displayNL(" > UserID, Login, ShardId, State, Privilege, ExtendedPrivilege, Cookie");
    for (const auto &user : users)
    {
        log.displayNL(" > %d '%s' %d %d '%s' '%s' '%s'", user.UserID, user.Login.c_str(), user.ShardID,
            user.State, user.Privilege.c_str(), user.ExtendedPrivilege.c_str(), user.Cookie.c_str());
    }
    log.displayNL("End of the list");

    return true;
}

NLMISC_COMMAND(onlineUsers, "displays the list of online users", "")
{
	if (args.size() != 0) return false;

    auto usersResult = loginService->UsersByState(domain::UserState::Online);
    if (!usersResult)
    {
        log.displayNL("UsersByState failed: %s", usersResult.error());
        return true;
    }
    auto &users = *usersResult;
    if (users.size() == 0)
    {
        log.displayNL("No online users found in the database");
        return true;
    }
    auto usersOnline = users.size();
    log.displayNL("Display the %d online users:", users.size());
    log.displayNL(" > UserID, Login, ShardId, State, Privilege, ExtendedPrivilege, Cookie");
    for (const auto &user : users)
    {
        log.displayNL(" > %d '%s' %d %d '%s' '%s' '%s'", user.UserID, user.Login.c_str(), user.ShardID,
            user.State, user.Privilege.c_str(), user.ExtendedPrivilege.c_str(), user.Cookie.c_str());
    }

    usersResult = loginService->UsersByState(domain::UserState::Waiting);
    if (!usersResult)
    {
        log.displayNL("UsersByState failed: %s", usersResult.error());
        return true;
    }
    users = *usersResult;
    if (users.size() != 0)
    {
        log.displayNL("Display the %d waiting users:", users.size());
        log.displayNL(" > UserID, Login, ShardId, State, Privilege, ExtendedPrivilege, Cookie");
        for (const auto &user : users)
        {
            log.displayNL(" > %d '%s'  %d '%s' %d '%s' '%s'", user.UserID, user.Login.c_str(), user.ShardID,
                user.State, user.Privilege.c_str(), user.ExtendedPrivilege.c_str(), user.Cookie.c_str());
        }
    }
    auto usersWaiting = users.size();
    usersResult = loginService->UsersByState(domain::UserState::Authorized);
    if (!usersResult)
    {
        log.displayNL("UsersByState failed: %s", usersResult.error());
        return true;
    }
    users = *usersResult;
    if (users.size() != 0)
    {

        log.displayNL("Display the %d authorized users:", users.size());
        log.displayNL(" > UserID, Login, ShardId, State, Privilege, ExtendedPrivilege, Cookie");
        for (const auto &user : users)
        {
            log.displayNL(" > %d '%s' %d %d '%s' '%s' '%s'", user.UserID, user.Login.c_str(), user.ShardID,
                user.State, user.Privilege.c_str(), user.ExtendedPrivilege.c_str(), user.Cookie.c_str());
        }
    }
    auto usersAuthorized = users.size();

    log.displayNL("End of the list (%d online users, %d waiting, %d authorized)", usersOnline, usersWaiting, usersAuthorized);
    return true;
}
