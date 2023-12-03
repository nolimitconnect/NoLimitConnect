//============================================================================
// Copyright (C) 2010 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================


#include "FriendList.h"

//============================================================================
FriendList::FriendList()
	: m_eFriendViewType( eFriendViewEverybody )
{
}

//============================================================================
FriendList::~FriendList()
{
}

//============================================================================
void FriendList::setFriendViewType( EFriendViewType eWhichFriendsToShow )
{
	m_eFriendViewType = eWhichFriendsToShow;
}

//============================================================================
EFriendViewType FriendList::getFriendViewType( void )
{
	return m_eFriendViewType;
}

//============================================================================
//! find friend by id
GuiUser* FriendList::findFriend( VxGUID& oId )
{
	std::vector<GuiUser*>::iterator iter;
	for( iter = m_aoFriends.begin(); iter != m_aoFriends.end(); ++iter )
	{
		if( oId == (*iter)->getMyOnlineId() )
		{
			return (*iter);
		}
	}
	return 0;
}

//============================================================================
//! update friend in list
void FriendList::updateFriendList( GuiUser* guiUser, bool sessionTimeChange )
{
	std::vector<GuiUser*>::iterator iter;
	for( iter = m_aoFriends.begin(); iter != m_aoFriends.end(); ++iter )
	{
		if( guiUser->getMyOnlineId() == (*iter)->getMyOnlineId() )
		{
			if( false == sessionTimeChange )
			{
				//memcpy( (VxNetIdent*)(*iter), guiUser, sizeof( VxNetIdent) );
				onFriendUpdated( (*iter) );
				return;
			}
			else
			{
                GuiUser* removeFriend = (*iter);
				m_aoFriends.erase( iter );
				onFriendRemoved( removeFriend );
				break;
			}
		}
	}

	// not found so add
	//Friend * poFriend = new Friend();
	//memcpy( (VxNetIdent*) poFriend, guiUser, sizeof( VxNetIdent ) );
	if( ( 0 == m_aoFriends.size() ) || ( 0 == guiUser->getLastSessionTimeMs() ) )
	{
		m_aoFriends.push_back( guiUser );
	}
	else
	{
		bool wasInserted = false;
		for( iter = m_aoFriends.begin(); iter != m_aoFriends.end(); ++iter )
		{
			if( guiUser->getLastSessionTimeMs() < (*iter)->getLastSessionTimeMs() )
			{
				m_aoFriends.insert( iter, guiUser );
				wasInserted = true;
				break;
			}
		}

		if( false == wasInserted )
		{
			m_aoFriends.push_back( guiUser );
		}
	}

	onFriendAdded( guiUser );
}

//============================================================================
//! remove friend from list.. return non zero if should not be removed
RCODE FriendList::removeFriendFromList( VxGUID& oId )
{
	for( auto iter = m_aoFriends.begin(); iter != m_aoFriends.end(); ++iter )
	{
		if( oId == (*iter)->getMyOnlineId() )
		{
			onFriendRemoved( (*iter) );
			delete (*iter);
			m_aoFriends.erase( iter );
			return 0;
		}
	}

	return 0;
}
