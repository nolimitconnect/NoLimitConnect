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

#include <PktLib/GroupieId.h>
#include <PktLib/SearchParams.h>

enum EFromGuiType
{
	eFromGuiTypeNone,

	eFromGuiAnnounceHost,
	eFromGuiJoinHost,
	eFromGuiLeaveHost,
	eFromGuiUnJoinHost,
	eFromGuiJoinLastJoinedHost,
	eFromGuiSearchHost,

	eMaxFromGuiType
};

class P2PEngine;

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

class FromGuiJoinLastHostAction : public FromGuiActionBase
{
public:
	FromGuiJoinLastHostAction( P2PEngine& engine, EFromGuiType fromGuiType, HostedId& adminId, VxGUID& sessionId );
	~FromGuiJoinLastHostAction() override = default;

	void						executeAction( void ) override;

	HostedId					m_AdminId;
	VxGUID						m_SessionId;
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
