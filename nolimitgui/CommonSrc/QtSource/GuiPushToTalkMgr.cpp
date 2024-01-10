//============================================================================
// Copyright (C) 2024 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#if defined(TARGET_OS_LINUX)
#include <QWidget> // must be declared first or linux Qt will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds
#endif // defined(TARGET_OS_LINUX)

#include "GuiPushToTalkMgr.h"

#include "AppCommon.h"

#include <P2PEngine/P2PEngine.h>
#include <PushToTalk/PushToTalkMgr.h>

#include <CoreLib/VxGlobals.h>

//============================================================================
GuiPushToTalkMgr::GuiPushToTalkMgr()
	: QObject()
{
}

//============================================================================
void GuiPushToTalkMgr::onSystemReady( void )
{
	connect( this, SIGNAL(signalInternalPushToTalkStatus(VxGUID,EPushToTalkStatus) ), this, SLOT(slotInternalPushToTalkStatus(VxGUID,EPushToTalkStatus)), Qt::QueuedConnection );
	GetAppInstance().getEngine().getPushToTalkMgr().wantPushToTalkCallbacks(this, true);
}

//============================================================================
void GuiPushToTalkMgr::callbackPushToTalkStatus( VxGUID& onlineId, enum EPushToTalkStatus pushToTalkStatus )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	emit signalInternalPushToTalkStatus( onlineId, pushToTalkStatus );
}

//============================================================================
void GuiPushToTalkMgr::slotInternalPushToTalkStatus( VxGUID onlineId, EPushToTalkStatus pushToTalkStatus )
{
	if( VxIsAppShuttingDown() )
	{
		return;
	}

	for( auto client : m_PushToTalkClients )
	{
		client->callbackPushToTalkStatus( onlineId, pushToTalkStatus );
	}
}

//========================================================================
void GuiPushToTalkMgr::wantGuiPushToTalkCallbacks( GuiPushToTalkCallback* clientInterface, bool wantCallbacks )
{
	for( auto iter = m_PushToTalkClients.begin(); iter != m_PushToTalkClients.end(); ++iter )
	{
		GuiPushToTalkCallback* offerInterface = (*iter);
		if( offerInterface == clientInterface )
		{
			if( wantCallbacks )
			{
				// already in list
				return;
			}
			else
			{
				// remove from list
				m_PushToTalkClients.erase( iter );
				return;
			}
		}
	}

	if( wantCallbacks )
	{
		m_PushToTalkClients.emplace_back( clientInterface );
	}
}

//============================================================================
void GuiPushToTalkMgr::setPushToTalkStatus( VxGUID& onlineId, EPushToTalkStatus pushToTalkStatus )
{
    if( onlineId.isVxGUIDValid() )
    {
        m_PushToTalkStatusMap[onlineId] = pushToTalkStatus;
    }
}

//============================================================================
EPushToTalkStatus GuiPushToTalkMgr::getPushToTalkStatus( VxGUID& onlineId )
{
	EPushToTalkStatus pushToTalkStatus{ ePushToTalkStatusNotActive };
	auto iter = m_PushToTalkStatusMap.find( onlineId );
	if( iter != m_PushToTalkStatusMap.end() )
	{
		pushToTalkStatus = iter->second;
	}

	return pushToTalkStatus;
};
