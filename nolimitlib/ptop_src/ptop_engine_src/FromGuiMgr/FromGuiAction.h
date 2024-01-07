#pragma once
//============================================================================
// Copyright (C) 2023 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <CoreLib/VxPtopUrl.h>

#include <PktLib/GroupieId.h>
#include <PktLib/SearchParams.h>

enum EFromGuiType
{
	eFromGuiTypeNone,

	eFromGuiTypeAppStartup,
	eFromGuiTypeSetUserSpecificDir,
	eFromGuiTypeSetUserXferDir,
	eFromGuiTypeiUserLoggedOn,

	eFromGuiAnnounceHost,
	eFromGuiJoinHost,
	eFromGuiLeaveHost,
	eFromGuiUnJoinHost,

	eFromGuiSearchHost,
	eFromGuiQueryHostListFromNetworkHost,

	eMaxFromGuiType
};

class P2PEngine;
class VxNetIdent;

class FromGuiActionBase
{
public:
	FromGuiActionBase( P2PEngine& engine, EFromGuiType fromGuiType );
	virtual ~FromGuiActionBase() = default;


	virtual void				executeAction( void ) = 0;

	virtual void				onGuiActionError( void );

	std::string					describeGuiAction( void );

	P2PEngine&					m_Engine;
	EFromGuiType				m_FromGuiType{ eFromGuiTypeNone };
};

class FromGuiStartupDirectoryAction : public FromGuiActionBase
{
public:
	FromGuiStartupDirectoryAction( P2PEngine& engine, EFromGuiType fromGuiType, std::string& dir1 );
	FromGuiStartupDirectoryAction( P2PEngine& engine, EFromGuiType fromGuiType, std::string& dir1, std::string& dir2 );
	~FromGuiStartupDirectoryAction() override = default;

	void						executeAction( void ) override;

	std::string					m_Dir1;
	std::string					m_Dir2;
};

class FromGuiUserLogon : public FromGuiActionBase
{
public:
	FromGuiUserLogon( P2PEngine& engine, EFromGuiType fromGuiType, VxNetIdent* myIdent );
	~FromGuiUserLogon() override = default;

	void						executeAction( void ) override;

	VxNetIdent*					m_MyIdent{ nullptr };
};

class FromGuiHostAction : public FromGuiActionBase
{
public:
	FromGuiHostAction( P2PEngine& engine, EFromGuiType fromGuiType, HostedId& adminId, VxGUID& sessionId, std::string& hostUrlIpv4, std::string& hostUrlIpv6 );
	~FromGuiHostAction() override = default;

	void						executeAction( void ) override;

	HostedId					m_AdminId;
	VxGUID						m_SessionId;
	std::string					m_HostUrlIpv4;
	std::string					m_HostUrlIpv6;
};

class FromGuiSearchHostAction : public FromGuiActionBase
{
public:
	FromGuiSearchHostAction( P2PEngine& engine, EFromGuiType fromGuiType, EHostType hostType, SearchParams& searchParams, bool enable );
	~FromGuiSearchHostAction() override = default;

	void						executeAction( void ) override;

	EHostType					m_HostType{ eHostTypeUnknown };
	SearchParams				m_SearchParams;
	bool						m_Enable;
};

class FromGuiQueryHostListFromNetworkHostAction : public FromGuiActionBase
{
public:
	FromGuiQueryHostListFromNetworkHostAction( P2PEngine& engine, EFromGuiType fromGuiType, VxPtopUrl& netHostUrl, EHostType hostType, VxGUID& hostIdIfNullThenAll, VxGUID& searchSessionId );
	~FromGuiQueryHostListFromNetworkHostAction() override = default;

	void						executeAction( void ) override;

	VxPtopUrl					m_PtopUrl;
	EHostType					m_HostType{ eHostTypeUnknown };
	VxGUID						m_HostIdIfNullThenAll;
	VxGUID						m_SearchSessionId;
};
