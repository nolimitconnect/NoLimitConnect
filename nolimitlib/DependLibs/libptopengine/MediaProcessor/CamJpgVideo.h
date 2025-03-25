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

#include <memory>

class CamJpgVideo
{
public:
    CamJpgVideo( std::shared_ptr<uint8_t>& vidData,
                int	vidDataLen,
                int motion = 0 )
        : m_VidData( vidData )
		, m_VidDataLen( vidDataLen )
        , m_Motion( motion )
	{
	}

	std::shared_ptr<uint8_t>	m_VidData;
    int                         m_VidDataLen;
    int							m_Motion;
};
