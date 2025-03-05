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

#include <memory>

class RawVideo
{
public:
	RawVideo( 	uint32_t	fourCc,
				std::shared_ptr<uint8_t>& vidData,
				uint32_t	vidDataLen, 
				int			width,
				int			height,
				int			rotation )
		: m_FourCc( fourCc ) // should always be FOURCC_RGB
		, m_VidData( vidData )
		, m_VidDataLen( vidDataLen )
		, m_Width( width )
		, m_Height( height )
		, m_Rotation( rotation )
	{
	}

	uint32_t					m_FourCc;
	std::shared_ptr<uint8_t>	m_VidData;
	uint32_t					m_VidDataLen; 
	int							m_Width;
	int							m_Height; 
	int							m_Rotation;
};
