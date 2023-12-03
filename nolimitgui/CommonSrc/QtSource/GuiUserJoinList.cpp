//============================================================================
// Copyright (C) 2021 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QWidget> // must be declared first or Qt 6.2.4 will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds

#include "GuiUserJoinList.h"
#include "GuiUserJoin.h"
#include "GuiUser.h"

//============================================================================
GuiUserJoinList::GuiUserJoinList()
: m_UserJoinList()
{
}

//============================================================================
GuiUserJoinList::GuiUserJoinList( const GuiUserJoinList& rhs )
    : m_UserJoinList( rhs.m_UserJoinList )
{
}

//============================================================================
GuiUserJoinList& GuiUserJoinList::operator =( const GuiUserJoinList& rhs )
{
    if( this != &rhs )
    {
        m_UserJoinList          = rhs.m_UserJoinList;
    }

    return *this;
}

//============================================================================
bool GuiUserJoinList::addUserJoinIfDoesntExist( GuiUserJoin* guiUserJoin )
{
	if( doesUserJoinExist( guiUserJoin ) )
	{
		return false;
	}
	
	addUserJoin( guiUserJoin );
	return true;
}

//============================================================================
void GuiUserJoinList::addUserJoin( GuiUserJoin* guiUserJoin )
{
    m_UserJoinList.push_back( guiUserJoin );
}

//============================================================================
bool GuiUserJoinList::doesUserJoinExist( GuiUserJoin* guiUserJoin )
{
	for( auto thumb : m_UserJoinList )
	{
		if( guiUserJoin == thumb )
		{
			return true;
		}
	}

	return false;
}

//============================================================================
GuiUserJoin* GuiUserJoinList::findUserJoin( VxGUID& thumbId )
{
    for( auto iter = m_UserJoinList.begin(); iter != m_UserJoinList.end(); ++iter )
    {
        if( (*iter)->getUser()->getMyOnlineId() == thumbId )
        {
            return *iter;
        }
    }

    return nullptr;
}

//============================================================================
bool GuiUserJoinList::removeUserJoin( GuiUserJoin* guiUserJoin )
{
    if( guiUserJoin )
    {
        return removeUserJoin( guiUserJoin->getUser()->getMyOnlineId() );
    }

    return false;
}

//============================================================================
bool GuiUserJoinList::removeUserJoin( VxGUID& thumbId )
{
    bool thumbExisted = false;
    std::vector<GuiUserJoin*>::iterator iter;
    for( iter = m_UserJoinList.begin(); iter != m_UserJoinList.end(); ++iter )
    {
        if( (*iter)->getUser()->getMyOnlineId() == thumbId )
        {
            thumbExisted = true;
            m_UserJoinList.erase( iter );
            break;
        }
    }

    return thumbExisted;
}

//============================================================================
void GuiUserJoinList::clearList( void )
{
	m_UserJoinList.clear();
}

//============================================================================
void GuiUserJoinList::copyTo( GuiUserJoinList& destUserJoinList )
{
	std::vector<GuiUserJoin*>& thumbList = destUserJoinList.getUserJoinList();
	std::vector<GuiUserJoin*>::iterator iter;
	for( iter = m_UserJoinList.begin(); iter != m_UserJoinList.end(); ++iter )
	{
		thumbList.push_back( *iter );
	}
}
