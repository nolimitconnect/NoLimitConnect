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

class GuiThumb;
class VxGUID;

class GuiThumbList
{
public:
	GuiThumbList();
	~GuiThumbList() = default;

    //! copy constructor
    GuiThumbList( const GuiThumbList & rhs );
    //! copy operator
    GuiThumbList& operator =( const GuiThumbList & rhs );

	int							size( void )						{ return (int)m_ThumbList.size(); }

	void						addThumb( GuiThumb* guiThumb );

    bool                        removeThumb( GuiThumb* guiThumb );
    bool						removeThumb( VxGUID& thumbId );

    GuiThumb*					findThumb( VxGUID& thumbId );

	// returns false if guid already exists
	bool						addThumbIfDoesntExist( GuiThumb* guiThumb );
	// return true if guid is in list
	bool						doesThumbExist( GuiThumb* guiThumb );

	void						clearList( void );

	std::vector<GuiThumb*>&		getThumbList( void )					{ return m_ThumbList; }
	void						copyTo( GuiThumbList& destThumbList );

protected:
	std::vector<GuiThumb*>		m_ThumbList;
};
