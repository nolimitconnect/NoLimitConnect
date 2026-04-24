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
#include <Plugins/FileInfo.h>
#include <P2PEngine/P2PEngine.h>

#include <CoreLib/VxDebug.h>
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
	LogModule( eLogAssets, LOG_VERBOSE, "HistoryListWidget::%s %s", __func__, m_MyApp.describeGroupieId( groupieId ).c_str() );
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
        LogModule( eLogAssets, LOG_VERBOSE, "HistoryListWidget::%s pos %d", __func__, pos0to100000 );
		assetWidget->toGuiClientAssetAction( assetAction, assetId, pos0to100000 );
	}
}

//============================================================================
void HistoryListWidget::toGuiAssetSessionHistory( AssetBaseInfo& assetInfo )
{
    if( isHistoryMatch( assetInfo ) )
	{
		LogModule( eLogAssets, LOG_VERBOSE, "HistoryListWidget::%s asset match", __func__ );
		if( !findAssetWidget( assetInfo.getAssetUniqueId() ) )
		{
			// this asset belongs in our history
			// create appropriate widget type for asset type
			AssetBaseWidget* assetWidget = createAssetWidget( &assetInfo );
			if( assetWidget )
			{
                connect( assetWidget, SIGNAL(signalShreddingAsset(AssetBaseWidget*)), this, SLOT(slotShreddingAsset(AssetBaseWidget*)) );
                connect( assetWidget, SIGNAL(signalAddLibraryAsset(AssetBaseWidget*)), this, SLOT(slotAddAssetToLibrary(AssetBaseWidget*)) );
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
				pruneHistoryToMax();
			}
		}
	}
	else
	{
		LogModule( eLogAssets, LOG_VERBOSE, "HistoryListWidget::%s asset NOT a match", __func__ );
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
void HistoryListWidget::slotAddAssetToLibrary( AssetBaseWidget * assetWidget )
{
	if( !assetWidget )
	{
		return;
	}

	AssetBaseInfo& assetInfo = assetWidget->getAssetInfo();
	if( !assetInfo.isFileAsset() )
	{
		return;
	}

	FileInfo fileInfo = assetInfo.getFileInfo();
	if( m_Engine.fromGuiSetFileIsInLibrary( fileInfo, true ) )
	{
		assetInfo.setIsInLibrary( true );
		assetWidget->showLibraryButton( false );
		m_MyApp.playSound( eSndDefButtonClick );
	}
}

//============================================================================
void HistoryListWidget::pruneHistoryToMax( void )
{
	int32_t maxHistory = VxGetMaxMessageHistory();
	if( maxHistory <= 0 )
	{
		return;
	}

	while( this->count() > maxHistory )
	{
		QListWidgetItem* oldestItem = this->item( 0 );
		AssetBaseWidget* oldestWidget = dynamic_cast<AssetBaseWidget*>( oldestItem );
		if( oldestWidget )
		{
			AssetBaseInfo& oldestAssetInfo = oldestWidget->getAssetInfo();
			if( oldestAssetInfo.isFileAsset() )
			{
				FileInfo fileInfo = oldestAssetInfo.getFileInfo();
				bool isInLibrary = m_Engine.fromGuiGetFileIsInLibrary( fileInfo );
				if( isInLibrary )
				{
					m_Engine.fromGuiAssetAction( eAssetActionRemoveFromAssetMgr, oldestAssetInfo, 0 );
				}
				else
				{
					m_Engine.fromGuiAssetAction( eAssetActionShreadFile, oldestAssetInfo, 0 );
				}
			}
			else
			{
				m_Engine.fromGuiAssetAction( eAssetActionRemoveFromAssetMgr, oldestAssetInfo, 0 );
			}
		}

		QListWidgetItem* removedItem = this->takeItem( 0 );
		if( removedItem )
		{
			delete removedItem;
		}
	}
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

//============================================================================
bool HistoryListWidget::isHistoryMatch( AssetBaseInfo& assetInfo )
{
	bool isAssetMatch = assetInfo.isHistoryMatch( m_GroupieId );
	isAssetMatch &= assetInfo.isPluginMatch( m_PluginType );

	return isAssetMatch;
}

//============================================================================
void HistoryListWidget::sendingToMember( VxGUID assetId, VxGUID memberId, QString memberName )
{
    AssetBaseWidget* assetWidget = findAssetWidget( assetId );
    if( assetWidget )
    {
        assetWidget->sendingToMember( memberId, memberName );
    }
}

//============================================================================
void HistoryListWidget::multiSendComplete( VxGUID assetId, bool allSucceeded, int successCount, int failCount )
{
    AssetBaseWidget* assetWidget = findAssetWidget( assetId );
    if( assetWidget )
    {
        assetWidget->multiSendComplete( allSucceeded, successCount, failCount );
    }
}