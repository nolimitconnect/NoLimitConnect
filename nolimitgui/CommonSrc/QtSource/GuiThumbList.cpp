//============================================================================
// Copyright (C) 2021 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "GuiThumbList.h"
#include "GuiThumb.h"

//============================================================================
GuiThumbList::GuiThumbList()
: m_ThumbList()
{
}

//============================================================================
GuiThumbList::GuiThumbList( const GuiThumbList& rhs )
    : m_ThumbList( rhs.m_ThumbList )
{
}

//============================================================================
GuiThumbList& GuiThumbList::operator =( const GuiThumbList& rhs )
{
    if( this != &rhs )
    {
        m_ThumbList          = rhs.m_ThumbList;
    }

    return *this;
}

//============================================================================
bool GuiThumbList::addThumbIfDoesntExist( GuiThumb* guiThumb )
{
	if( doesThumbExist( guiThumb ) )
	{
		return false;
	}
	
	addThumb( guiThumb );
	return true;
}

//============================================================================
void GuiThumbList::addThumb( GuiThumb* guiThumb )
{
    m_ThumbList.emplace_back( guiThumb );
}

//============================================================================
bool GuiThumbList::doesThumbExist( GuiThumb* guiThumb )
{
	for( auto thumb : m_ThumbList )
	{
		if( guiThumb == thumb )
		{
			return true;
		}
	}

	return false;
}

//============================================================================
GuiThumb* GuiThumbList::findThumb( VxGUID& thumbId )
{
    for( auto iter = m_ThumbList.begin(); iter != m_ThumbList.end(); ++iter )
    {
        if( (*iter)->getThumbId() == thumbId )
        {
            return *iter;
        }
    }

    return nullptr;
}

//============================================================================
bool GuiThumbList::removeThumb( GuiThumb* guiThumb )
{
    if( guiThumb )
    {
        return removeThumb( guiThumb->getThumbId() );
    }

    return false;
}

//============================================================================
bool GuiThumbList::removeThumb( VxGUID& thumbId )
{
    bool thumbExisted = false;
    std::vector<GuiThumb*>::iterator iter;
    for( iter = m_ThumbList.begin(); iter != m_ThumbList.end(); ++iter )
    {
        if( (*iter)->getThumbId() == thumbId )
        {
            thumbExisted = true;
            m_ThumbList.erase( iter );
            break;
        }
    }

    return thumbExisted;
}

//============================================================================
void GuiThumbList::clearList( void )
{
	m_ThumbList.clear();
}

//============================================================================
void GuiThumbList::copyTo( GuiThumbList& destThumbList )
{
	std::vector<GuiThumb*>& thumbList = destThumbList.getThumbList();
	std::vector<GuiThumb*>::iterator iter;
	for( iter = m_ThumbList.begin(); iter != m_ThumbList.end(); ++iter )
	{
		thumbList.push_back( *iter );
	}
}
