#pragma once
//============================================================================
// Copyright (C) 2010 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "GuiUser.h"

class FriendList
{
public:
	FriendList();
	virtual ~FriendList();

	void						setFriendViewType( EFriendViewType eWhichFriendsToShow );
	EFriendViewType				getFriendViewType( void );

	virtual void				onFriendAdded( GuiUser* poFriend ) {};
	virtual void				onFriendUpdated( GuiUser* poFriend ) {};
	virtual void				onFriendRemoved( GuiUser* poFriend ) {};

	//! find friend by id.. returns NULL if not found
	GuiUser*					findFriend( VxGUID& oId );

protected:
	//! update friend in list
	void						updateFriendList( GuiUser* guiUser, bool sessionTimeChange );
	//! remove friend from list.. return non zero if should not be removed
	int32_t						removeFriendFromList( VxGUID& oId );

	//=== vars ===//
	std::vector<GuiUser*>		m_aoFriends;
	EFriendViewType				m_eFriendViewType;
};