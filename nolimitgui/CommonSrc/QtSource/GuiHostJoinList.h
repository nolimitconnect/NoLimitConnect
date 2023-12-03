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

class GuiHostJoin;
class VxGUID;

class GuiHostJoinList
{
public:
	GuiHostJoinList();
	~GuiHostJoinList() = default;

    //! copy constructor
    GuiHostJoinList( const GuiHostJoinList & rhs );
    //! copy operator
    GuiHostJoinList& operator =( const GuiHostJoinList & rhs );

	int								size( void )						{ return (int)m_UserJoinList.size(); }

	void							addUserJoin( GuiHostJoin* GuiHostJoin );

    bool							removeUserJoin( GuiHostJoin* GuiHostJoin );
    bool							removeUserJoin( VxGUID& thumbId );

    GuiHostJoin*					findUserJoin( VxGUID& thumbId );

	// returns false if guid already exists
	bool							addUserJoinIfDoesntExist( GuiHostJoin* GuiHostJoin );
	// return true if guid is in list
	bool							doesUserJoinExist( GuiHostJoin* GuiHostJoin );

	void							clearList( void );

	std::vector<GuiHostJoin*>&		getUserJoinList( void )					{ return m_UserJoinList; }
	void							copyTo( GuiHostJoinList& destUserJoinList );

protected:
	std::vector<GuiHostJoin*>		m_UserJoinList;
};
