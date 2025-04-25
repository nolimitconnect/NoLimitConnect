#pragma once
//============================================================================
// Copyright (C) 2025 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QWidget> // must be declared first or linux Qt will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds

#include <GuiInterface/IDefs.h>

#include <FriendRequestMgr/FriendRequestInfo.h>
#include <CoreLib/GroupieId.h>
#include <CoreLib/VxGUID.h>

#include <vector>

class AppCommon;
class GuiUser;
class GuiFriendRequestMgr;
class FriendRequestInfo;

class GuiFriendRequest : public QWidget
{
public:
    GuiFriendRequest() = delete;
    GuiFriendRequest( AppCommon& app );
    GuiFriendRequest( AppCommon& app, std::shared_ptr<FriendRequestInfo>& requestInfo );
    GuiFriendRequest( const GuiFriendRequest& rhs );

	virtual ~GuiFriendRequest() = default;

    GuiFriendRequestMgr&       getFriendRequestMgr( void )                  { return m_FriendRequestMgr; }

    std::shared_ptr<FriendRequestInfo>& getRequestInfo( void )              { return m_RequestInfo; }
    std::shared_ptr<VxNetIdent>& getNetIdent( void )                        { return m_RequestInfo->getNetIdent(); }
    VxGUID&                     getMyOnlineId( void )                       { return m_RequestInfo->getNetIdent()->getMyOnlineId(); }
    VxGUID&                     getRequestId( void )                        { return m_RequestInfo->getRequestId(); }
    std::string                 getRequestMsg( void )                       { return m_RequestInfo->getRequestText(); }

protected:
    AppCommon&                  m_MyApp;
    GuiFriendRequestMgr&        m_FriendRequestMgr;

    std::shared_ptr<FriendRequestInfo>  m_RequestInfo;
};
