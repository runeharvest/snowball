// NeLNS - MMORPG Framework <http://dev.ryzom.com/projects/nel/>
// Copyright (C) 2010  Winch Gate Property Limited
//
// This source file has been modified by the following contributors:
// Copyright (C) 2025 Xackery
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

#include "nel/misc/types_nl.h"

#include <cstdio>
#include <ctype.h>
#include <cmath>

#include <vector>
#include <map>

#include "nel/misc/debug.h"
#include "nel/misc/config_file.h"
#include "nel/misc/displayer.h"
#include "nel/misc/log.h"

#include "nel/net/service.h"
#include "nel/net/login_cookie.h"

#include "login.h"

using namespace std;
using namespace NLMISC;
using namespace NLNET;

static uint RecordNbPlayers = 0;
uint NbPlayers = 0;

void refuseShard(TServiceId sid, const char *format, ...)
{
	string reason;
	NLMISC_CONVERT_VARGS(reason, format, NLMISC::MaxCStringSize);
	nlwarning(reason.c_str());
	CMessage msgout("FAILED");
	msgout.serial(reason);
	CUnifiedNetwork::getInstance()->send(sid, msgout);
}

static void cbWSConnection(const std::string &serviceName, TServiceId sid, void *arg)
{
	TSockId from;
	CCallbackNetBase *cnb = CUnifiedNetwork::getInstance()->getNetBase(sid, from);
	const CInetAddress &ia = cnb->hostAddress(from);

	nldebug("new potential shard: %s", ia.asString().c_str());

	if (configService->ValueBool("login", "is_external_shard_allowed") != true)
	{
		return;
	}

	auto shard = loginService->ShardByWSAddr(ia.ipAddress());
	if (!shard)
    {
        refuseShard(sid, "Bad shard identification, the shard (WSAddr %s) is not in the database and can't be added", ia.ipAddress().c_str());
        return;
    }
}

static void cbWSDisconnection(const std::string &serviceName, TServiceId sid, void *arg)
{
	TSockId from;
	CCallbackNetBase *cnb = CUnifiedNetwork::getInstance()->getNetBase(sid, from);
	const CInetAddress &ia = cnb->hostAddress(from);

	nldebug("shard disconnection: %s", ia.asString().c_str());

	for (uint32 i = 0; i < Shards.size(); i++)
	{
		if (Shards[i].SId != sid)
		{
			continue;
		}

		nlinfo("ShardId %d with IP '%s' is offline!", Shards[i].ShardId, ia.asString().c_str());
		nlinfo("*** ShardId %3d NbPlayers %3d -> %3d", Shards[i].ShardId, Shards[i].NbPlayers, 0);

        auto shard = loginService->ShardByShardID(Shards[i].ShardId);
        if (!shard)
        {
            nlwarning("ShardId %d not found in the database, can't update it", Shards[i].ShardId);
            continue;
        }
        shard->IsOnline = 0;
        shard->PlayerCount = 0;
        if (!loginService->ShardUpdate(*shard))
        {
            nlwarning("Failed to update shard %d in the database", Shards[i].ShardId);
        }

        NbPlayers -= Shards[i].NbPlayers;

        Shards[i].NbPlayers = 0;

        auto usersResult = loginService->UsersByShardID(Shards[i].ShardId);
        if (!usersResult)
        {
            nlwarning("Failed to get users by shard id %d", Shards[i].ShardId);
            continue;
        }
        auto users = *usersResult;
        for (auto user : users)
        {
            user.State = domain::UserState::Offline;

            if (!loginService->UserUpdate(user))
            {
                nlwarning("Failed to update user %d state to Offline", user.UserID);
            }
        }

        Shards.erase(Shards.begin() + i);
        return;
    }
    nlwarning("Shard %s goes offline but wasn't online!", ia.asString().c_str());
}

static void cbWSIdentification(CMessage &msgin, const std::string &serviceName, TServiceId sid)
{
	TSockId from;
	CCallbackNetBase *cnb = CUnifiedNetwork::getInstance()->getNetBase(sid, from);
	const CInetAddress &ia = cnb->hostAddress(from);

	sint32 shardId;
	msgin.serial(shardId);
	string application;
	try
	{
		msgin.serial(application);
	}
	catch (Exception &)
	{
	}

	string reason;

    auto shard = loginService->ShardByShardID(shardId);
    if (!shard)
    {
		if (configService->ValueBool("login", "is_external_shard_allowed") != true)
		{
            refuseShard(sid, "Bad shard identification, The shard %d is not in the database and can't be added", shardId);
            return;
        }

        auto newShard = domain::Shard();
        newShard.ShardID = shardId;
        newShard.WSAddr = ia.ipAddress();
        newShard.ClientApplication = application;

        shard = loginService->ShardCreate(newShard);
        if (!reason.empty())
        {
            refuseShard(sid, "Failed to create shard %d: %s", shardId, reason.c_str());
            return;
        }
        nlinfo("The ShardId %d with ip '%s' was inserted in the database and is online!", shardId, ia.ipAddress().c_str());
        Shards.push_back(CShard(shardId, sid));
        return;
    }

    if (shard->WSAddr != ia.ipAddress())
    {
        refuseShard(sid, "Bad shard identification, ShardId %d should come from '%s' and come from '%s'", shardId, shard->WSAddr.c_str(), ia.ipAddress().c_str());
        return;
    }

    sint32 s = findShard(shardId);
    if (s != -1)
    {
        // the shard is already online, disconnect the old one and set the new one
        refuseShard(Shards[s].SId, "A new shard connected with the same IP/ShardId, you were replaced");

        Shards[s].SId = sid;
		Shards[s].ShardId = shardId;
    }
    else
    {
        Shards.push_back(CShard(shardId, sid));
    }

    shard->IsOnline = 1;
	shard->PlayerCount = 0;
    if (!loginService->ShardUpdate(*shard))
    {
        refuseShard(sid, "Failed to update shard %d in the database", shardId);
        return;
    }
}

static void cbWSClientConnected(CMessage &msgin, const std::string &serviceName, TServiceId sid)
{

	uint32 Id;
	uint8 con;
	msgin.serial(Id);
	msgin.serial(con); // con=1 means a client is connected on the shard, 0 means a client disconnected

	if (con)
		nlinfo("Received a validation that a client is connected on the frontend");
	else
		nlinfo("Received a validation that a client is disconnected on the frontend");

    auto user = loginService->UserByUserID(Id);
    if (!user)
    {
        nlwarning("User with UserID %d not found", Id);
        Output->displayNL("###: %3d User not found", Id);
        return;
    }

    sint ShardPos = findShardWithSId(sid);

    switch (con)
    {
    case 0:
        if (user->State != domain::UserState::Online)
        {
            nlwarning("User %d is not online, can't disconnect him from the shard", Id);
            Output->displayNL("###: %3d User isn't online, his state is '%d'", Id, user->State);
            return;
        }

        user->State = domain::UserState::Online;
        user->ShardID = Shards[ShardPos].ShardId;
        if (!loginService->UserUpdate(*user))
        {
            nlwarning("Failed to set user %d state to Online", user->UserID);
            return;
        }

        if (ShardPos != -1)
        {
            nlinfo("*** ShardId %3d NbPlayers %3d -> %3d", Shards[ShardPos].ShardId, Shards[ShardPos].NbPlayers, Shards[ShardPos].NbPlayers + 1);
            Shards[ShardPos].NbPlayers++;

            auto shard = loginService->ShardByShardID(Shards[ShardPos].ShardId);
            if (!shard)
            {
                nlwarning("ShardId %d not found in the database, can't update it", Shards[ShardPos].ShardId);
                return;
            }

            shard->PlayerCount = Shards[ShardPos].NbPlayers;
            if (!loginService->ShardUpdate(*shard))
            {
                nlwarning("Failed to update shard %d in the database", Shards[ShardPos].ShardId);
                return;
            }
        }
        else
            nlwarning("user connected shard isn't in the shard list");

        nldebug("Id %d is connected on the shard", Id);
        Output->displayNL("###: %3d User connected to the shard (%d)", Id, Shards[ShardPos].ShardId);

        NbPlayers++;
        if (NbPlayers > RecordNbPlayers)
        {
            RecordNbPlayers = NbPlayers;
            beep(2000, 1, 100, 0);
            nlwarning("New player number record!!! %d players online on all shards", RecordNbPlayers);
        }
        return;
    case 1: // new client connecting
        if (user->State != domain::UserState::Waiting)
        {
            nlwarning("User %d is not waiting, can't connect him to the shard", Id);
            Output->displayNL("###: %3d User isn't waiting, his state is '%d'", Id, user->State);
            return;
        }
        break;
    default: // disconnecting
        user->State = domain::UserState::Offline;
        user->ShardID = -1;
        if (!loginService->UserUpdate(*user))
        {
            nlwarning("Failed to set user %d state to Offline", user->UserID);
            return;
        }

        if (ShardPos == -1)
        {
            nlwarning("user disconnected shard isn't in the shard list");
            return;
        }
        nlinfo("*** ShardId %3d NbPlayers %3d -> %3d", Shards[ShardPos].ShardId, Shards[ShardPos].NbPlayers, Shards[ShardPos].NbPlayers - 1);
        Shards[ShardPos].NbPlayers--;

        auto shard = loginService->ShardByShardID(Shards[ShardPos].ShardId);
        if (!shard)
        {
            nlwarning("ShardId %d not found in the database, can't update it", Shards[ShardPos].ShardId);
            return;
        }

        shard->PlayerCount = Shards[ShardPos].NbPlayers;
        if (!loginService->ShardUpdate(*shard))
        {
            nlwarning("Failed to update shard %d in the database", Shards[ShardPos].ShardId);
            return;
        }

        nldebug("Id %d is disconnected from the shard", Id);
        Output->displayNL("###: %3d User disconnected from the shard (%d)", Id, Shards[ShardPos].ShardId);

        NbPlayers--;

        return;
    }
}

static void cbWSReportFSState(CMessage &msgin, const std::string &serviceName, TServiceId sid)
{
	sint shardPos = findShardWithSId(sid);

	if (shardPos == -1)
	{
		nlwarning("unknown WS %d reported state of a fs", sid.get());
		return;
	}

	CShard &cshard = Shards[shardPos];

	TServiceId FSSId;
	bool alive;
	bool patching;
	std::string patchURI;

	msgin.serial(FSSId);
	msgin.serial(alive);
	msgin.serial(patching);
	msgin.serial(patchURI);

	if (!alive)
	{
		nlinfo("Shard %d frontend %d reported as offline", cshard.ShardId, FSSId.get());
		std::vector<CFrontEnd>::iterator itfs;
		for (itfs = cshard.FrontEnds.begin(); itfs != cshard.FrontEnds.end(); ++itfs)
		{
			if ((*itfs).SId == FSSId)
			{
				cshard.FrontEnds.erase(itfs);
				break;
			}
		}
	}
	else
	{
		CFrontEnd *updateFS = NULL;
		std::vector<CFrontEnd>::iterator itfs;
		for (itfs = cshard.FrontEnds.begin(); itfs != cshard.FrontEnds.end(); ++itfs)
		{
			if ((*itfs).SId == FSSId)
			{
				// update FS state
				CFrontEnd &fs = (*itfs);
				fs.Patching = patching;
				fs.PatchURI = patchURI;

				updateFS = &fs;

				break;
			}
		}

		if (itfs == cshard.FrontEnds.end())
		{
			nlinfo("Shard %d frontend %d reported as online", cshard.ShardId, FSSId.get());
			// unknown fs, create new entry
			cshard.FrontEnds.push_back(CFrontEnd(FSSId, patching, patchURI));

			updateFS = &(cshard.FrontEnds.back());
		}

		if (updateFS != NULL)
		{
			nlinfo("Shard %d frontend %d status updated: patching=%s patchURI=%s", cshard.ShardId, FSSId.get(), (updateFS->Patching ? "yes" : "no"), updateFS->PatchURI.c_str());
		}
	}

	// update DynPatchURLS in database
	std::string dynPatchURL;
	uint i;
	for (i = 0; i < cshard.FrontEnds.size(); ++i)
	{
		if (cshard.FrontEnds[i].Patching)
		{
			if (!dynPatchURL.empty())
				dynPatchURL += ' ';
			dynPatchURL += cshard.FrontEnds[i].PatchURI;
		}
	}

    auto shard = loginService->ShardByShardID(cshard.ShardId);
    if (!shard)
    {
        nlwarning("ShardId %d not found in the database, can't update it", cshard.ShardId);
        return;
    }

    shard->DynPatchURL = dynPatchURL;
    if (!loginService->ShardUpdate(*shard))
    {
        nlwarning("Failed to update shard %d DynPatchURL in the database", cshard.ShardId);
        return;
    }
}

static void cbWSReportNoPatch(CMessage &msgin, const std::string &serviceName, TServiceId sid)
{
	sint shardPos = findShardWithSId(sid);

	if (shardPos == -1)
	{
		nlwarning("unknown WS %d reported state of a fs", sid.get());
		return;
	}

	CShard &cshard = Shards[shardPos];

	uint i;
	for (i = 0; i < cshard.FrontEnds.size(); ++i)
	{
		cshard.FrontEnds[i].Patching = false;
	}

    auto shard = loginService->ShardByShardID(cshard.ShardId);
    if (!shard)
    {
        nlwarning("ShardId %d not found in the database, can't update it", cshard.ShardId);
        return;
    }
    shard->DynPatchURL.clear();
    if (!loginService->ShardUpdate(*shard))
    {
        nlwarning("Failed to update shard %d DynPatchURL in the database", cshard.ShardId);
        return;
    }
}

static void cbWSSetShardOpen(CMessage &msgin, const std::string &serviceName, TServiceId sid)
{
	sint shardPos = findShardWithSId(sid);

	if (shardPos == -1)
	{
		nlwarning("unknown WS %d reported shard open state", sid.get());
		return;
	}

	uint8 shardOpenState;
	msgin.serial(shardOpenState);

	CShard &cshard = Shards[shardPos];

    auto shard = loginService->ShardByShardID(cshard.ShardId);
    if (!shard)
    {
        nlwarning("ShardId %d not found in the database, can't update it", cshard.ShardId);
        return;
    }
    shard->IsOnline = shardOpenState;
    if (!loginService->ShardUpdate(*shard))
    {
        nlwarning("Failed to update shard %d open state in the database", cshard.ShardId);
        return;
    }
}

static const TUnifiedCallbackItem WSCallbackArray[] = {
	{ "CC", cbWSClientConnected },
	{ "WS_IDENT", cbWSIdentification },

	{ "REPORT_FS_STATE", cbWSReportFSState },
	{ "REPORT_NO_PATCH", cbWSReportNoPatch },
	{ "SET_SHARD_OPEN", cbWSSetShardOpen },
};

//
// Functions
//

void connectionWSInit()
{
	CUnifiedNetwork::getInstance()->addCallbackArray(WSCallbackArray, sizeof(WSCallbackArray) / sizeof(WSCallbackArray[0]));

	CUnifiedNetwork::getInstance()->setServiceUpCallback("WS", cbWSConnection);
	CUnifiedNetwork::getInstance()->setServiceDownCallback("WS", cbWSDisconnection);
}

void connectionWSUpdate()
{
}

void connectionWSRelease()
{
	nlinfo("I'm going down, clean the database");

	while (!Shards.empty())
	{
		cbWSDisconnection("", Shards[0].SId, NULL);
	}

	/*
	    // we remove all shards online from my list
	    for (uint32 i = 0; i < Shards.size (); i++)
	    {
	        TSockId from;
	        CCallbackNetBase *cnb = CUnifiedNetwork::getInstance ()->getNetBase (Shards[i].SId, from);
	        const CInetAddress &ia = cnb->hostAddress (from);

	        // shard disconnected
	        nlinfo("Set ShardId %d with IP '%s' offline in the database and set %d players to offline", Shards[i].ShardId, ia.asString ().c_str());

	        string query = "update shard set Online=Online-1, NbPlayers=NbPlayers-"+toString(Shards[i].NbPlayers)+" where ShardId="+toString(Shards[i].ShardId);
	        sint ret = mysql_query (DatabaseConnection, query.c_str ());
	        if (ret != 0)
	        {
	            nlwarning ("mysql_query (%s) failed: %s", query.c_str (),  mysql_error(DatabaseConnection));
	        }

	        // put users connected on this shard offline
	    }
	    Shards.clear ();
	*/
}
