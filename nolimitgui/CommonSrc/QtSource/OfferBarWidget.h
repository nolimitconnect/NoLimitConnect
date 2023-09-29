#pragma once
//============================================================================
// Copyright (C) 2016 Brett R. Jones
// Issued to MIT style license by Brett R. Jones in 2017
//
// You may use, copy, modify, merge, publish, distribute, sub-license, and/or sell this software
// provided this Copyright is not modified or removed and is included all copies or substantial portions of the Software
//
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QFrame>

#include "GuiOfferCallback.h"

#include <ptop_src/ptop_engine_src/OfferBase/OfferBaseInfo.h>

#include "ui_OfferBarWidget.h"
#include <QListWidgetItem>

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

	AppCommon&					getMyApp( void ) { return m_MyApp; }
	MyIcons&					getMyIcons( void );

	void						setOfferSessionId( VxGUID& offerSessionId ) { m_OfferSessionId = offerSessionId; }

	bool						setUser( GuiUser* guiUser );
	void						setPluginType( EPluginType pluginType );
	bool						setOfferInfo( OfferBaseInfo& offerInfo );
	

protected slots:
	void						slotAcceptOfferButtonClicked( void );
	void						slotRejectOfferButtonClicked( void );

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


	Ui::OfferBarWidgetUi		ui;
	AppCommon&				    m_MyApp;
	GuiOfferMgr&			m_OfferMgr;
	VxGUID						m_OfferSessionId;
	GuiUser*					m_HisIdent{ nullptr };
	EPluginType					m_PluginType{ ePluginTypeInvalid };
	OfferBaseInfo				m_OfferInfo;
};
