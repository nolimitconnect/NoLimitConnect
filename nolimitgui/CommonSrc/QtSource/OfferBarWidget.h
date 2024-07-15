#pragma once
//============================================================================
// Copyright (C) 2016 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QFrame>

#include "GuiOfferCallback.h"

#include <OfferBase/OfferBaseInfo.h>

#include <QListWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui {
    class OfferBarWidgetUi;
}
QT_END_NAMESPACE

class AppCommon;
class GuiOfferMgr;
class GuiOfferSession;
class GuiUser;
class MyIcons;
class QLabel;

class OfferBarWidget : public QFrame, public GuiOfferCallback
{
	Q_OBJECT

public:
	OfferBarWidget( QWidget* parent = nullptr );
	virtual ~OfferBarWidget();

	AppCommon&					getMyApp( void )							{ return m_MyApp; }
	MyIcons&					getMyIcons( void );

	void						setOfferSessionId( VxGUID& offerSessionId ) { m_OfferSessionId = offerSessionId; }

	bool						setUser( GuiUser* guiUser );
	void						setPluginType( EPluginType pluginType );

	bool						setOfferInfo( OfferBaseInfo& offerInfo );
	OfferBaseInfo&				getOfferInfo( void )						{ return m_OfferInfo; }

protected slots:
	void						slotAcceptOfferButtonClicked( void );
	void						slotRejectOfferButtonClicked( void );
	void						slotOfferInfoButtonClicked( void );

protected:
	virtual void				showEvent( QShowEvent* ev );
	virtual void				hideEvent( QHideEvent* ev );
	virtual void				closeEvent( QCloseEvent * ev );

	virtual void				callbackGuiUpdatePluginOffer( GuiOfferSession* offerState ); 
	virtual void				callbackGuiOfferRemoved( GuiOfferSession* offerState ); 
	virtual void				callbackGuiAllOffersRemoved( void ); 


	void						initializeOfferBar( void );
	void						fillOfferBar( GuiOfferSession* offerState );
	void						updateOfferBar( GuiOfferSession* offerState );
	void						setOfferOnlineState( bool isOnline );
	void						setIsOfferAvailable( bool available );
	bool						getIsOfferAvailable( void );


	Ui::OfferBarWidgetUi&		ui;
	AppCommon&				    m_MyApp;
	GuiOfferMgr&				m_OfferMgr;
	VxGUID						m_OfferSessionId;
	GuiUser*					m_HisIdent{ nullptr };
	EPluginType					m_PluginType{ ePluginTypeInvalid };
	OfferBaseInfo				m_OfferInfo;
};
