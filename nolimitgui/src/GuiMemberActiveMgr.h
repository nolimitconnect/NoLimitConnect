#pragma once
//============================================================================
// Copyright (C) 2021 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#if defined(TARGET_OS_LINUX)
#include <QWidget> // must be declared first or linux Qt will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds
#endif // defined(TARGET_OS_LINUX)

#include <GuiInterface/IDefs.h>

#include <Membership/MemberActiveCallback.h>

#include <CoreLib/VxMutex.h>
#include <CoreLib/GroupieId.h>

#include <QObject>

#include <set>

class AppCommon;
class GuiMemberActiveCallback;


class GuiMemberActiveMgr : public QObject, public MemberActiveCallback
{
    Q_OBJECT
public:
    GuiMemberActiveMgr();
    GuiMemberActiveMgr( const GuiMemberActiveMgr& rhs ) = delete;
	virtual ~GuiMemberActiveMgr() = default;

    virtual void                onAppCommonCreated( void );

    void                        wantMemberActiveCallback( GuiMemberActiveCallback* client, bool enable );

    bool                        isMemberActive( GroupieId& groupieId );
    bool                        isMemberOfHostType( EHostType hostType, VxGUID& onlineId );
    bool                        isActiveMemberOfAny( VxGUID& onlineId );  // return true if active member of any host we have joined

    void                        getActiveMembers( HostedId& hostId, std::set<VxGUID>& memberList );
    GroupieId                   getJoinedAdminGroupieId( EHostType hostType, VxGUID& onlineId );

signals:
    void                        signalInternalMemberActive( GroupieId groupieId, bool isActive );

private slots:
    void                        slotInternalMemberActive( GroupieId groupieId, bool isActive );

protected:
    void                        callbackMemberActive( GroupieId& groupieId, bool isActive ) override;

    void                        updateMemberActive( GroupieId& groupieId, bool isActive );

    void                        announceMemberActive( GroupieId& groupieId, bool isActive );
    void                        announceMemberIsJoinedToHost( VxGUID& onlineId, EHostType host, bool isJoined );

    std::vector<GroupieId>      m_MemberList;

    std::vector<GuiMemberActiveCallback*> m_MemberClients;

};
