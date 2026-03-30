//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "AssetBaseWidget.h"

#include "AppCommon.h"
#include "GuiHelpers.h"

#include <P2PEngine/P2PEngine.h>

#include <CoreLib/VxDebug.h>
#include <CoreLib/VxGlobals.h>

#include <QProgressBar>
#include <QTimer>

//============================================================================
AssetBaseWidget::AssetBaseWidget( AppCommon& appCommon, QWidget* parent)
: QWidget( parent )
, m_MyApp( appCommon )
, m_Engine( appCommon.getEngine() )
, m_AssetInfo()
, m_AssetWidgetReadyTimer( new QTimer( this ) )
{
	m_AssetWidgetReadyTimer->setInterval( 200 );
	connect( m_AssetWidgetReadyTimer, SIGNAL(timeout()), this, SLOT(slotAssetReadyForCallbacksTimeout()) );
}

//============================================================================
AssetBaseWidget::~AssetBaseWidget()
{
	if( m_CallbacksRequested && ( false == VxIsAppShuttingDown() ) )
	{
		wantActivityCallbacks( false );
		m_CallbacksRequested = false;
	}
}

//============================================================================
MyIcons& AssetBaseWidget::getMyIcons( void )
{
	return m_MyApp.getMyIcons();
}

//============================================================================
void AssetBaseWidget::setXferBar( QProgressBar* xferProgressBar )	
{ 
	m_XferProgressBar = xferProgressBar; 
	if( 0 != m_XferProgressBar )
	{
		m_XferProgressBar->setVisible( m_ProgressBarShouldBeVisible );
		m_ProgressBarIsVisible = m_ProgressBarShouldBeVisible;
	}
}

//============================================================================
void AssetBaseWidget::showEvent(QShowEvent* showEvent)
{
	QWidget::showEvent(showEvent);
	if( !m_AssetReadyForCallbacks )
	{
		// avoid calling want callbacks while being created by a calllback
		m_AssetWidgetReadyTimer->start();
	}
	else
	{
		onAssetWidgetVisibleAndReady( true, true );
	}
}

//============================================================================
void AssetBaseWidget::hideEvent( QHideEvent* ev )
{
	onAssetWidgetVisibleAndReady( false, true );
	QWidget::hideEvent( ev );
}

//============================================================================
void AssetBaseWidget::toGuiClientAssetAction( EAssetAction assetAction, VxGUID& assetId, int pos0to100000 )
{
	if( assetId == m_AssetInfo.getAssetUniqueId() )
	{
		if( eAssetActionTxError == assetAction )
		{
			LogMsg( LOG_ERROR, "%s txError %s ", __func__, m_AssetInfo.getAssetUniqueId().toHexString().c_str() );
		}

		bool hideResendAndFail = false;
		bool needUpdate = false;
		switch( assetAction )
		{
		case eAssetActionTxProgress:
			if( eAssetSendStateTxProgress != m_AssetInfo.getAssetSendState() )
			{
				m_AssetInfo.setAssetSendState( eAssetSendStateTxProgress );
				needUpdate = true;
			}

			m_ProgressBarShouldBeVisible = true;
			if( m_AssetSendProgress != pos0to100000 )
			{
				m_AssetSendProgress = pos0to100000;
				needUpdate = true;
				if( 100 == m_AssetSendProgress )
				{
					m_ProgressBarShouldBeVisible = false;
					hideResendAndFail = true;
				}
			}

			break;

		case eAssetActionRxProgress:
			if( eAssetSendStateRxProgress != m_AssetInfo.getAssetSendState() )
			{
				m_AssetInfo.setAssetSendState( eAssetSendStateRxProgress );
				needUpdate = true;
			}

			m_ProgressBarShouldBeVisible = true;
			if( m_AssetSendProgress != pos0to100000 )
			{
				m_AssetSendProgress = pos0to100000;
				needUpdate = true;
				if( 100 == m_AssetSendProgress )
				{
					m_ProgressBarShouldBeVisible = false;
					hideResendAndFail = true;
				}
			}

			break;

		case eAssetActionTxBegin:
			m_ProgressBarShouldBeVisible = true;
			//if( eAssetSendStateTxProgress != m_AssetInfo.getAssetSendState() )
			{
				m_AssetSendProgress = 0;
				m_AssetInfo.setAssetSendState( eAssetSendStateTxProgress );
				needUpdate = true;
			}

			break;

		case eAssetActionRxBegin:
			m_ProgressBarShouldBeVisible = true;
			if( eAssetSendStateRxProgress != m_AssetInfo.getAssetSendState() )
			{
				m_AssetInfo.setAssetSendState( eAssetSendStateRxProgress );
				needUpdate = true;
			}
			break;

		case eAssetActionTxSuccess:
			m_ProgressBarShouldBeVisible = false;
			m_AssetSendProgress = 100;
			if( eAssetSendStateTxSuccess != m_AssetInfo.getAssetSendState() )
			{
				m_AssetInfo.setAssetSendState( eAssetSendStateTxSuccess );
				needUpdate = true;
				hideResendAndFail = true;
			}

			break;

		case eAssetActionRxSuccess:
			m_ProgressBarShouldBeVisible = false;
			if( eAssetSendStateRxSuccess != m_AssetInfo.getAssetSendState() )
			{
				m_AssetInfo.setAssetSendState( eAssetSendStateRxSuccess );
				needUpdate = true;
				hideResendAndFail = true;
			}

			break;

		case eAssetActionTxError:
		case eAssetActionTxCancel:
			m_ProgressBarShouldBeVisible = true;
			if( eAssetSendStateTxFail != m_AssetInfo.getAssetSendState() )
			{
				m_AssetInfo.setAssetSendState( eAssetSendStateTxFail );
				needUpdate = true;
			}
			break;

		case eAssetActionRxError:
		case eAssetActionRxCancel:
			m_ProgressBarShouldBeVisible = true;
			if( eAssetSendStateRxFail != m_AssetInfo.getAssetSendState() )
			{
				m_AssetInfo.setAssetSendState( eAssetSendStateRxFail );
				needUpdate = true;
			}
			break;

		default:
			m_ProgressBarShouldBeVisible = false;
			//LogMsg( LOG_ERROR, "Unknown Asset action %d\n", assetAction );
			break;
		}

		if( hideResendAndFail )
		{
			setResendAndFailVisible( false );
		}

		if( needUpdate )
		{
			updateFromAssetInfo();
		}
	}

	updateProgressBarVisibility();
}

//============================================================================
void AssetBaseWidget::updateProgressBarVisibility( void )
{
	if( 0 != m_XferProgressBar )
	{
		if( ( m_ProgressBarShouldBeVisible != m_ProgressBarIsVisible )
			&& ( 0 != m_XferProgressBar ) )
		{
			m_XferProgressBar->setVisible( m_ProgressBarShouldBeVisible );
			m_ProgressBarIsVisible = m_ProgressBarShouldBeVisible;
		}
	}
}

//============================================================================
void AssetBaseWidget::slotShredAsset( void )
{
	// don't do this.. history list will call it
	//this->m_Engine.fromGuiAssetAction( ePluginTypeInvalid, eAssetActionShreadFile, m_AssetInfo.getAssetUniqueId(), 0 );
	emit signalShreddingAsset( this );
}

//============================================================================
void AssetBaseWidget::slotResendAsset( void )
{
	GuiUser* guiUser = m_MyApp.getUserMgr().getUser( m_AssetInfo.getDestUserId() );
	if( !guiUser || !guiUser->isReachable() )
	{
		GuiHelpers::errorMsgBox( eErrMsgUserIsOffline, (QWidget*)parent() );
		return;
	}

	m_Engine.fromGuiAssetAction( eAssetActionAssetResend, m_AssetInfo, 0 );
	m_MyApp.playSound( eSndDefButtonClick );
}

//============================================================================
void AssetBaseWidget::doShowProgress( bool showProgress )
{
	m_ProgressBarShouldBeVisible = showProgress;
	updateProgressBarVisibility();
	showXferProgress( showProgress );
}

//============================================================================
void AssetBaseWidget::doSetProgress(  int xferProgress )
{
	if( 0 != m_XferProgressBar )
	{
		m_XferProgressBar->setValue( xferProgress );
	}

	setXferProgress( xferProgress );
}

//============================================================================
void AssetBaseWidget::setResendAndFailVisible( bool visible )
{
	showSendFail( visible );
	showResendButton( visible );
}

//============================================================================
void AssetBaseWidget::updateFromAssetInfo( void )
{
	switch( m_AssetInfo.getAssetSendState() )
	{
	case eAssetSendStateTxProgress:
	case eAssetSendStateRxProgress:
		showSendFail( false );
		showResendButton( false );
		doShowProgress( true );
		showShredder( false );
		doSetProgress( m_AssetSendProgress );
		break;

	case eAssetSendStateTxSuccess:
	case eAssetSendStateRxSuccess:
		showSendFail( false );
		showResendButton( false );
		doShowProgress( false );
		showShredder( true );
		break;

	case eAssetSendStateTxFail:
		showSendFail( true );
		showResendButton( true );
		doShowProgress( false );
		showShredder( true );
		break;

	case eAssetSendStateTxPermissionErr:
		showSendFail( true, true );
		showResendButton( false );
		doShowProgress( true );
		showShredder( true );
		break;

	case eAssetSendStateRxPermissionErr:
		showSendFail( true, true );
		showResendButton( false );
		doShowProgress( false );
		showShredder( true );
		break;

	case eAssetSendStateRxFail:
		showSendFail( true );
		showResendButton( false );
		doShowProgress( false );
		showShredder( true );
		break;

	case eAssetSendStateNone:
	default:
		showSendFail( false );
		showResendButton( false );
		doShowProgress( false );
		showShredder( true );
		break;

	}
}

//============================================================================
void AssetBaseWidget::slotAssetReadyForCallbacksTimeout( void )
{
	m_AssetWidgetReadyTimer->stop();
	onAssetWidgetVisibleAndReady( isVisible(), true );
}

//============================================================================
void AssetBaseWidget::onAssetWidgetVisibleAndReady( bool isVisible, bool isReady )
{
	if( isReady )
	{
		m_AssetReadyForCallbacks = true;
		if( isVisible )
		{
			if( (false == VxIsAppShuttingDown())
				&& m_AssetInfo.isValid()
				&& (false == m_CallbacksRequested) )
			{
				m_CallbacksRequested = true;
				wantActivityCallbacks( true );
			}

			updateProgressBarVisibility();
		}
		else
		{
			if( m_CallbacksRequested && (false == VxIsAppShuttingDown()) )
			{
				m_CallbacksRequested = false;
				wantActivityCallbacks( false );
			}
		}
	}
}

//============================================================================
void AssetBaseWidget::wantActivityCallbacks( bool enable )
{
	if( enable != m_ActivityCallbacksRequested )
	{
		m_ActivityCallbacksRequested = enable;
		m_MyApp.wantToGuiActivityCallbacks( this, enable );
	}	
}

