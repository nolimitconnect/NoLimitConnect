//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================


#include "HistoryListWidget.h"
#include "AppCommon.h"
#include "DialogConfirmRemoveMessage.h"

#include "AssetTextWidget.h"
#include "AssetFaceWidget.h"
#include "AssetPhotoWidget.h"
#include "AssetVoiceWidget.h"
#include "AssetVideoWidget.h"

#include <AssetMgr/AssetInfo.h>
#include <P2PEngine/P2PEngine.h>

#include <CoreLib/VxGlobals.h>

#include <QTimer>

//============================================================================
HistoryListWidget::HistoryListWidget(QWidget* parent)
: QListWidget( parent )
, m_MyApp( GetAppInstance() )
, m_Engine( GetAppInstance().getEngine() )
, m_CallbacksRequested( false )
, m_StartupTimer( new QTimer( this ) )
{
	m_StartupTimer->setInterval( 200 );
	connect( m_StartupTimer, SIGNAL(timeout()), this, SLOT(slotStartupTimeout()) );
}

//============================================================================
HistoryListWidget::~HistoryListWidget()
{
	if( m_CallbacksRequested && ( false == VxIsAppShuttingDown() ) )
	{
		wantActivityCallbacks( false );
		m_CallbacksRequested = false;
	}
}

//============================================================================
void HistoryListWidget::showEvent(QShowEvent* showEvent)
{
	QListWidget::showEvent(showEvent);
	if( ( false == m_CallbacksRequested )
		&& ( false == VxIsAppShuttingDown() ) )
	{
		m_CallbacksRequested = true;
		wantActivityCallbacks( true );
	}

	if( !m_QueryHistoryCalled )
	{
		m_StartupTimer->start();
	}
}

//============================================================================
void HistoryListWidget::hideEvent( QHideEvent* ev )
{
	if( m_CallbacksRequested )
	{
		m_CallbacksRequested = false;
		wantActivityCallbacks( false );
	}

	QListWidget::hideEvent( ev );
}

//============================================================================
void HistoryListWidget::onActivityStop( void )
{
	for(int i = 0; i < this->count(); ++i)
	{
		QListWidgetItem* item = this->item(i);
		((AssetBaseWidget *)item)->onActivityStop();
	}
}

//============================================================================
void HistoryListWidget::setGroupieId( GroupieId& groupieId )
{ 
	clear();
	LogMsg( LOG_VERBOSE, "HistoryListWidget::%s %p %s", __func__, this, m_MyApp.describeGroupieId( groupieId ).c_str() );
	m_GroupieId = groupieId; 

	wantActivityCallbacks( true );

	m_Engine.fromGuiQuerySessionHistory( m_GroupieId );
}

//============================================================================
void HistoryListWidget::initializeHistory( void )
{
	if( !m_QueryHistoryCalled && m_GroupieId.getHostedId().isValid() )
	{
		m_QueryHistoryCalled = true;
        m_Engine.fromGuiQuerySessionHistory( m_GroupieId );
	}
}

//============================================================================
void HistoryListWidget::toGuiClientAssetAction( EAssetAction assetAction, VxGUID& assetId, int pos0to100000 )
{
	AssetBaseWidget* assetWidget = findAssetWidget( assetId );
	if( assetWidget )
	{
		assetWidget->toGuiClientAssetAction( assetAction, assetId, pos0to100000 );
	}
}

//============================================================================
void HistoryListWidget::toGuiAssetSessionHistory( AssetBaseInfo& assetInfo )
{
    if( assetInfo.isHistoryMatch( m_GroupieId ) )
	{
		LogMsg( LOG_VERBOSE, "HistoryListWidget::%s %p asset match", __func__, this );
		if( !findAssetWidget( assetInfo.getAssetUniqueId() ) )
		{
			// this asset belongs in our history
			// create appropriate widget type for asset type
			AssetBaseWidget* assetWidget = createAssetWidget( &assetInfo );
			if( assetWidget )
			{
				connect( assetWidget, SIGNAL( signalShreddingAsset( AssetBaseWidget* ) ), this, SLOT( slotShreddingAsset( AssetBaseWidget* ) ) );
				int insertAtIndex = determinInsertIndex( &assetInfo );
				if( 0 <= insertAtIndex )
				{
					this->insertItem( insertAtIndex, (QListWidgetItem*)assetWidget );
				}
				else
				{
					this->addItem( (QListWidgetItem*)assetWidget );
				}

				this->setItemWidget( (QListWidgetItem*)assetWidget, (QWidget*)assetWidget );
			}
		}
	}
	else
	{
		LogMsg( LOG_VERBOSE, "HistoryListWidget::%s %p asset NOT a match", __func__, this );
	}
}

//============================================================================
void HistoryListWidget::toGuiAssetAdded( AssetBaseInfo& assetInfo )
{
	toGuiAssetSessionHistory( assetInfo );
}

//============================================================================
void HistoryListWidget::slotShreddingAsset( AssetBaseWidget * assetWidget )
{
	for(int i = 0; i < this->count(); ++i)
	{
		QListWidgetItem* item = this->item(i);
		if( ((AssetBaseWidget *)item) == assetWidget )
		{
			// found the widget being shredded;
            AssetBaseInfo& assetInfo = assetWidget->getAssetInfo();
            DialogConfirmRemoveMessage dlg( assetInfo, false, this );
			dlg.exec();
			EAssetAction removeAssetAction = dlg.getAssetActionResult();
			if( eAssetActionShreadFile == removeAssetAction )
			{
				m_MyApp.playSound( eSndDefPaperShredder );
				m_Engine.fromGuiAssetAction(  eAssetActionShreadFile, assetInfo, 0 );
				takeItem( i );
			}
			else if( eAssetActionRemoveFromAssetMgr  == removeAssetAction )
			{
				m_MyApp.playSound( eSndDefNeckSnap );
				m_Engine.fromGuiAssetAction( eAssetActionRemoveFromAssetMgr, assetInfo, 0 );
				takeItem( i );
			}

			break;
		}
	}
	
	update();
}

//============================================================================
int  HistoryListWidget::determinInsertIndex( AssetBaseInfo* assetInfo )
{
	int insertIdx = -1;
	for( int i = 0; i < this->count(); i++ )
	{
		AssetBaseWidget * assetWidget = (AssetBaseWidget *)this->item(i);
		if( assetInfo->getCreationTime() <= assetWidget->getAssetInfo().getCreationTime() )
		{
			insertIdx = i;
			break;
		}
	}

	return insertIdx;
}

//============================================================================
AssetBaseWidget* HistoryListWidget::createAssetWidget( AssetBaseInfo* assetInfo )
{
	AssetBaseWidget* assetWidget{ nullptr };
	switch( assetInfo->getAssetType() )
	{
	case eAssetTypePhoto:
		assetWidget = new AssetPhotoWidget( this );
		break;
	case eAssetTypeAudio:
		assetWidget = new AssetVoiceWidget( this );
		break;
	case eAssetTypeVideo:
		assetWidget = new AssetVideoWidget( this );
		break;
	case eAssetTypeChatText:
		assetWidget = new AssetTextWidget( this );
		break;
	case eAssetTypeChatFace:
		assetWidget = new AssetFaceWidget( this );
		break;

	default:
		break;
	}

	if( assetWidget )
	{
		assetWidget->setAssetInfo( *assetInfo );
	}

	return assetWidget;
}

//============================================================================
AssetBaseWidget* HistoryListWidget::findAssetWidget( VxGUID& assetId )
{
	AssetBaseWidget* assetWidget{ nullptr };
	for( int i = 0; i < this->count(); ++i )
	{
		QListWidgetItem* item = this->item( i );
		if( item )
		{
			AssetBaseWidget* curAssetWidget = dynamic_cast<AssetBaseWidget*>(item);
			if( curAssetWidget && curAssetWidget->getAssetInfo().getAssetUniqueId() == assetId )
			{
				assetWidget = curAssetWidget;
				break;
			}
		}
	}

	return assetWidget;
}

//============================================================================
void HistoryListWidget::clearHistoryList( void )
{
	for(int i = 0; i < this->count(); ++i)
	{
		QListWidgetItem* item = this->item(i);
		delete ((AssetBaseWidget *)item);
	}

	this->clear();
}

//============================================================================
void HistoryListWidget::slotStartupTimeout( void )
{
	m_StartupTimer->stop();
	if( !m_QueryHistoryCalled && m_GroupieId.getHostedId().isValid() )
	{
		m_QueryHistoryCalled = true;
		m_Engine.fromGuiQuerySessionHistory( m_GroupieId );
	}
}

//============================================================================
void HistoryListWidget::wantActivityCallbacks( bool enable )
{
	if( enable != m_ActivityCallbacksRequested )
	{
		m_ActivityCallbacksRequested = enable;
		m_MyApp.wantToGuiActivityCallbacks( this, enable );
	}	
}
