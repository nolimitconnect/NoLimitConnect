#pragma once
//============================================================================
// Copyright (C) 2014 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "HostedInfo.h"

#include <CoreLib/DbBase.h>

class HostedListDb : public DbBase
{
public:
	HostedListDb();
	virtual ~HostedListDb() = default;

	RCODE						hostedListDbStartup( int dbVersion, const char* dbFileName );
	RCODE						hostedListDbShutdown( void );

	void						getAllHosteds( std::vector<HostedInfo>& hostedList );
	bool						saveHosted( HostedInfo& hostedInfo );
	void						removeClosedPortIdent( VxGUID& onlineId );
    void						removeHostedInfo( enum EHostType hostType, VxGUID& onlineId );

    bool                        updateIsFavorite( enum EHostType hostType, VxGUID& onlineId, bool isFavorite );
    bool                        updateLastConnected( enum EHostType hostType, VxGUID& onlineId, int64_t lastConnectedTime );
    bool                        updateLastJoined( enum EHostType hostType, VxGUID& onlineId, int64_t lastJoinedTime );
    bool						updateHostUrl( bool ipv6, enum  EHostType hostType, VxGUID& onlineId, std::string& hostUrl );
    bool                        updateHostTitleAndDescription( enum EHostType hostType, VxGUID& onlineId, std::string& title, std::string& description, int64_t lastDescUpdateTime, VxGUID& thumbId );


protected:
	virtual RCODE				onCreateTables( int iDbVersion );
	virtual RCODE				onDeleteTables( int iOldVersion );

    bool						doesHostInfoExist( enum EHostType hostType, VxGUID& onlineId, std::string& retOnlineHexStr );
};


