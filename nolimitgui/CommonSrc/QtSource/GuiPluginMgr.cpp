//============================================================================
// Copyright (C) 2022 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "GuiPluginMgr.h"

//============================================================================
GuiPluginMgr::GuiPluginMgr( AppCommon& app )
	: m_MyApp( app )
{
}

//============================================================================
bool GuiPluginMgr::getIsPluginVisible( EPluginType pluginType )
{
	QVector<EPluginType>::iterator iter;
	for( iter = m_VisiblePluginsList.begin(); iter != m_VisiblePluginsList.end(); ++iter )
	{
		if( pluginType == *iter )
		{
			return true;
		}
	}

	return false;
}

//============================================================================
void GuiPluginMgr::setPluginVisible( EPluginType pluginType, bool isVisible )
{
	if( pluginType == ePluginTypeInvalid )
	{
		return;
	}

	QVector<EPluginType>::iterator iter;
	for( iter = m_VisiblePluginsList.begin(); iter != m_VisiblePluginsList.end(); ++iter )
	{
		if( pluginType == *iter )
		{
			if( isVisible )
			{
				// already in list
				return;
			}
			else
			{
				// remove from list
				m_VisiblePluginsList.erase( iter );
				return;
			}
		}
	}

	if( isVisible )
	{
		m_VisiblePluginsList.push_back( pluginType );
	}
}
