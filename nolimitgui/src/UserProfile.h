#pragma once
//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QString>

class UserProfile
{
public:
	UserProfile()
	{
		m_strGreeting = "Lets Communicate!";
		m_strAboutMe = "I'm Wonderful!";
		m_strUrl3 = "https://nolimitconnect.com";
		m_strPicturePath = "";
	}

    //=== vars ===//
    QString			m_strGreeting;
    QString			m_strAboutMe;
    QString			m_strPicturePath;
    QString			m_strUrl1;
    QString			m_strUrl2;
    QString			m_strUrl3;
    QString			m_strDonation;
};
