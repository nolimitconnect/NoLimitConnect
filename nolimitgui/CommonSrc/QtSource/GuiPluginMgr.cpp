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

#include <CoreLib/VxDebug.h>

//============================================================================
GuiPluginMgr::GuiPluginMgr()
{
	connect( this, SIGNAL(signalInternalToGuiPluginStatus(EPluginType,int,int)), this, SLOT(slotInternalToGuiPluginStatus(EPluginType,int,int)), Qt::QueuedConnection );
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

//============================================================================
void GuiPluginMgr::toGuiPluginStatus( EPluginType pluginType, int statusType, int statusValue )
{
	emit signalInternalToGuiPluginStatus( pluginType, statusType, statusValue );
}

//============================================================================
void GuiPluginMgr::slotInternalToGuiPluginStatus( EPluginType pluginType, int statusType, int statusValue )
{
	if( pluginType == ePluginTypeCamServer )
	{
		setIsCamServerEnabled( statusType ? true : false );
		setCamServerClientCount( statusValue );
	}

	for( auto& client : m_CallbackClients )
	{
		client->callbackToGuiPluginStatus( pluginType, statusType, statusValue );
	}
}

//============================================================================
void GuiPluginMgr::wantPluginMgrCallbacks( GuiPluginMgrCallback* client, bool enable )
{
    if( !client )
    {
        LogMsg( LOG_ERROR, "GuiPluginMgr null client" );
        return;
    }

    bool wasFound = false;
    for( auto iter = m_CallbackClients.begin(); iter != m_CallbackClients.end(); ++iter )
    {
        if( *iter == client )
        {
            wasFound = true;
            if( !enable )
            {
                m_CallbackClients.erase( iter );
            }

            break;
        }
    }

    if( enable && !wasFound )
    {
        m_CallbackClients.push_back( client );
    }
}
