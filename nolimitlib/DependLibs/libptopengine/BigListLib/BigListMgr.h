#pragma once
//============================================================================
// Copyright (C) 2003 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "BigListDb.h"

#include <GuiInterface/IDefs.h>

class IToGui;
class P2PEngine;
class PktAnnList;
class PktAnnounce;
class VxNetIdent;

enum EPktAnnUpdateType
{
	ePktAnnUpdateTypeIgnored,
	ePktAnnUpdateTypeNewContact,
	ePktAnnUpdateTypeContactIsSame,
	ePktAnnUpdateTypeContactChanged,
};

const char* DescribePktAnnUpdateType( EPktAnnUpdateType pktAnnUpdateType );

class P2PEngine;

class BigListMgr : public BigListDb
{
public:
	BigListMgr() = delete;
	BigListMgr( P2PEngine& engine );
	virtual ~BigListMgr() override;

	RCODE						bigListMgrStartup( const char* pDbFileName );
	RCODE						bigListMgrShutdown( void );

	RCODE						updateBigListDatabase( BigListInfo * poInfo, const char* networkName );

	//=== add/remove functions ===//
	//! add a or update remote friend
	EPktAnnUpdateType			updatePktAnn(	PktAnnounce *	poPktAnn, 
												BigListInfo **	ppoRetInfo,
												EHostType		hostType,
												bool			useMyFriendshipFromPktAnn = false,
												bool			useHisFriendshipFromPktAnn = true );	

	bool						updateTempIdent( VxNetIdent& tempIdent );

	bool						getFriendships( VxGUID&			hisOnlineId,
												EFriendState&	retMyFriendshipToHim,
												EFriendState&	retHisFriendshipToMe );
	bool						isUserIgnored( VxGUID& hisOnlineId );

	bool						getOnlineName( VxGUID& hisOnlineId, std::string& onlineName );
	//! return true if can add friend to list
	bool						canAddFriend( void );
	//! remove from big list.. also from db if bRemoveStorage = true 
	RCODE						removeFriend( PktAnnounce * poPktAnn, bool  bRemoveStorage = true );

	//helpers
	RCODE						FillAnnList(	PktAnnList * poPktAnnList, 
												int iMaxListLen,
												int64_t s64ContactTimeLimitMs,
												bool bIncludeThisNode = false );

	void						LimitListSize( void );

	bool						queryIdent( VxGUID& onlineId, VxNetIdent& netIdent );

	void						onMyFriendshipChanged( EFriendState prevMyFriendship, VxNetIdent* netIdent );

	bool						updateMemberFriendship( VxGUID& onlineId, bool isMember = true );

	bool						updateMemberFriendship( BigListInfo* bigListInfo, bool isMember = true );

	bool				        fromGuiDeleteUser( VxGUID& onlineId );

    //! update which list person is in
    void						updateVectorList( enum EFriendState oldFriendship, BigListInfo * poInfo ) override;

protected:

	//=== vars ===//
    bool                        m_BigListMgrInitialized = false;
};



