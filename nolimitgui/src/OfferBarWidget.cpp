//============================================================================
// Copyright (C) 2016 Brett R. Jones 
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license 
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "OfferBarWidget.h"

#include "ActivityMessageBox.h"
#include "AppCommon.h"
#include "GuiHelpers.h"
#include "GuiOfferMgr.h"
#include "MyIcons.h"

#include <CoreLib/VxGlobals.h>

#include <QCloseEvent>

#include "ui_OfferBarWidget.h"

//============================================================================
OfferBarWidget::OfferBarWidget( QWidget* parent )
: QFrame( parent )
, ui(*(new Ui::OfferBarWidgetUi))
, m_MyApp( GetAppInstance() )
, m_OfferMgr( m_MyApp.getOfferMgr() )
, m_HisIdent( 0 )
, m_PluginType( ePluginTypeInvalid )
{
	ui.setupUi( this );
	ui.m_FriendButton->setFixedSize( eButtonSizeMedium );
	ui.m_OfferViewButton->setFixedSize( eButtonSizeMedium );
	ui.m_AcceptButton->setFixedSize( eButtonSizeMedium );
	ui.m_RejectButton->setFixedSize( eButtonSizeMedium );
	ui.m_OfferInfoButton->setFixedSize( eButtonSizeMedium );

	ui.m_RejectButton->setIcon( eMyIconCancelNormal );
	ui.m_OfferInfoButton->setIcon( eMyIconInformation );

	connect( ui.m_FriendButton,				SIGNAL(clicked()),		this, SLOT(slotAcceptOfferButtonClicked()) );
	connect( ui.m_AcceptButton,				SIGNAL(clicked()),		this, SLOT(slotAcceptOfferButtonClicked()) );
	connect( ui.m_RejectButton,				SIGNAL(clicked()),		this, SLOT(slotRejectOfferButtonClicked()) );
	connect( ui.m_OfferInfoButton,			SIGNAL(clicked()),		this, SLOT(slotOfferInfoButtonClicked()) );
	m_OfferMgr.wantGuiOfferCallbacks( this, true );
}

//============================================================================
OfferBarWidget::~OfferBarWidget()
{
    if( false == VxIsAppShuttingDown() )
    {
		m_OfferMgr.wantGuiOfferCallbacks( this, false );
    }
}

//============================================================================
MyIcons&  OfferBarWidget::getMyIcons( void )
{
	return m_MyApp.getMyIcons();
}

//============================================================================
void OfferBarWidget::showEvent( QShowEvent* ev )
{
	QWidget::showEvent( ev );
	initializeOfferBar();
}

//============================================================================
void OfferBarWidget::hideEvent( QHideEvent* ev )
{
	QWidget::hideEvent( ev );
}

//============================================================================
void OfferBarWidget::closeEvent( QCloseEvent * ev )
{
	m_OfferMgr.wantGuiOfferCallbacks( this, false );
	ev->accept();
}

//============================================================================
void OfferBarWidget::initializeOfferBar( void )
{
	std::shared_ptr<GuiOfferSession> offerSession = m_OfferMgr.getTopGuiOfferSession();
	fillOfferBar( offerSession );
}

//============================================================================
void OfferBarWidget::fillOfferBar( std::shared_ptr<GuiOfferSession> offerSession )
{
	if( offerSession.get() )
	{
        m_OfferId       = offerSession->getOfferId();
        m_HisIdent		= offerSession->getUser();
        m_PluginType	= offerSession->getPluginType();
		updateOfferBar( offerSession );
	}
	else
	{
        m_OfferId.clearVxGUID();
		m_HisIdent = nullptr;
		m_PluginType = ePluginTypeInvalid;
	}
}

//============================================================================
void OfferBarWidget::updateOfferBar( std::shared_ptr<GuiOfferSession> offerSession )
{
	ui.m_OfferCntLabel->setText( QString("%1").arg( m_OfferMgr.getActiveOfferCount() ) );
    if( offerSession->getOfferId() == m_OfferId )
	{
		switch( offerSession->getOfferState() )
		{
		case eOfferStateRxedByUser:
			setIsOfferAvailable( true );
			break;

		default:
			setIsOfferAvailable( false );
			break;
		}

		setOfferOnlineState( offerSession->getUser()->isOnline() );
	}
}
//============================================================================
void OfferBarWidget::setOfferOnlineState( bool isOnline )
{
	if( !isOnline )
	{
		ui.m_AcceptButton->setIcon( eMyIconAcceptDisabled );
		ui.m_AcceptButton->setEnabled( false );
	}
}

//============================================================================
void OfferBarWidget::setIsOfferAvailable( bool available )
{
	ui.m_AcceptButton->setEnabled( available );
	ui.m_AcceptButton->setIcon( available ? eMyIconAcceptNormal : eMyIconAcceptDisabled );
}

//============================================================================
bool OfferBarWidget::getIsOfferAvailable( void )
{
	return ui.m_AcceptButton->isEnabled();
}

//============================================================================
void OfferBarWidget::slotAcceptOfferButtonClicked( void )
{
    m_OfferMgr.acceptOfferButtonClicked( m_PluginType, m_OfferId, m_HisIdent );
}

//============================================================================
void OfferBarWidget::slotRejectOfferButtonClicked( void )
{

    m_OfferMgr.rejectOfferButtonClicked( m_PluginType, m_OfferId, m_HisIdent );
}

//============================================================================
void OfferBarWidget::slotOfferInfoButtonClicked( void )
{
	ActivityMessageBox offerInfoMsgBox( m_MyApp, GuiHelpers::getParentPageFrame( this ) );
	offerInfoMsgBox.showOfferInfo( getOfferInfo() );
	offerInfoMsgBox.exec();
}

//============================================================================
void OfferBarWidget::callbackGuiUpdatePluginOffer( std::shared_ptr<GuiOfferSession> offerState )
{
	if( ( 0 == parentWidget() ) || parentWidget()->isVisible() )
	{
		fillOfferBar( offerState );
	}
}

//============================================================================
void OfferBarWidget::callbackGuiOfferRemoved( std::shared_ptr<GuiOfferSession> offerState )
{
    if( this->isVisible() && ( offerState->getOfferId() == m_OfferId ) )
	{
		initializeOfferBar();
	}
}

//============================================================================
void OfferBarWidget::callbackGuiAllOffersRemoved( void )
{
	this->setVisible( false );
}


//============================================================================
bool OfferBarWidget::setUser( GuiUser* guiUser )
{
	if( guiUser )
	{
		m_HisIdent = guiUser;
		return true;
	}

	return false;
}

//============================================================================
void OfferBarWidget::setPluginType( EPluginType pluginType )
{
	m_PluginType = pluginType;
}

//============================================================================
bool OfferBarWidget::setOfferInfo( OfferBaseInfo& offerInfo )
{
	m_OfferInfo = offerInfo;
	if( !m_OfferInfo.isValid() )
	{
		return false;
	}

	if( m_HisIdent && ePluginTypeInvalid != m_PluginType )
	{
		ui.m_OfferViewButton->setIcon( m_MyApp.getMyIcons().getOfferIcon( m_OfferInfo, m_PluginType ) );
        //ui.m_OfferViewButton->setEyeOverlayEnabled( true );
		return true;
	}

	return false;
}
