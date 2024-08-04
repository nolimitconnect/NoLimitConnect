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

#include <memory.h>

class RawVideo
{
public:
	RawVideo( 	uint32_t		fourCc,
		uint8_t *	vidData,
		uint32_t		vidDataLen, 
		int		width,
		int		height,
		int		rotation,
		bool	needToDelete )
		: m_FourCc( fourCc ) // should always be FOURCC_RGB
		, m_VidData( vidData )
		, m_VidDataLen( vidDataLen )
		, m_Width( width )
		, m_Height( height )
		, m_Rotation( rotation )
	{
		if( !needToDelete )
		{
			// need to make copy because will be deleted by caller
			uint8_t * newVidData = new uint8_t[ vidDataLen ];
			memcpy( newVidData, vidData, vidDataLen );
			m_VidData = newVidData;
		}
	}

	uint32_t							m_FourCc;
	uint8_t *						m_VidData;
	uint32_t							m_VidDataLen; 
	int							m_Width;
	int							m_Height; 
	int							m_Rotation;
};
