#pragma once
//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <GuiInterface/IFromGui.h>
#include <CoreLib/MediaCallbackInterface.h>

class MediaClient
{
public:
	MediaClient(	EMediaInputType				mediaType, 
					MediaCallbackInterface *	callback, 
					void *						userData )
	: m_MediaInputType( mediaType )
	, m_Callback( callback )
	, m_UserData( userData )
	{
	}

	MediaClient( const MediaClient &rhs )
	{
		*this = rhs;
	}

	MediaClient&				operator =( const MediaClient &rhs )
	{
		if( this != &rhs )
		{
			m_MediaInputType	= rhs.m_MediaInputType;
			m_Callback			= rhs.m_Callback;
			m_UserData			= rhs.m_UserData;
		}

		return *this;
	}

	EMediaInputType				m_MediaInputType;
	MediaCallbackInterface *	m_Callback; 
	void *						m_UserData;
};
