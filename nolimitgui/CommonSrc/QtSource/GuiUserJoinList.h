#pragma once
//============================================================================
// Copyright (C) 2021 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <vector>

class GuiUserJoin;
class VxGUID;

class GuiUserJoinList
{
public:
	GuiUserJoinList();
	~GuiUserJoinList() = default;

    //! copy constructor
    GuiUserJoinList( const GuiUserJoinList & rhs );
    //! copy operator
    GuiUserJoinList& operator =( const GuiUserJoinList & rhs );

	int							size( void )						{ return (int)m_UserJoinList.size(); }

	void						addUserJoin( GuiUserJoin* guiUserJoin );

    bool                        removeUserJoin( GuiUserJoin* guiUserJoin );
    bool						removeUserJoin( VxGUID& thumbId );

    GuiUserJoin*				findUserJoin( VxGUID& thumbId );

	// returns false if guid already exists
	bool						addUserJoinIfDoesntExist( GuiUserJoin* guiUserJoin );
	// return true if guid is in list
	bool						doesUserJoinExist( GuiUserJoin* guiUserJoin );

	void						clearList( void );

	std::vector<GuiUserJoin*>&	getUserJoinList( void )					{ return m_UserJoinList; }
	void						copyTo( GuiUserJoinList& destUserJoinList );

protected:
	std::vector<GuiUserJoin*>	m_UserJoinList;
};
