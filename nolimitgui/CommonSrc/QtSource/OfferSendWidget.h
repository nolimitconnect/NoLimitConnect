#pragma once
//============================================================================
// Copyright (C) 2022 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include <QWidget>

#include <OfferBase/OfferBaseInfo.h>

#include "ui_OfferSendWidget.h"
#include <QListWidgetItem>

class AppCommon;
class GuiOfferMgr;
class GuiOfferSession;
class GuiUser;
class MyIcons;
class QLabel;

class OfferSendWidget : public QWidget
{
	Q_OBJECT

public:
	OfferSendWidget( QWidget* parent = nullptr );
	virtual ~OfferSendWidget() = default;

	AppCommon&					getMyApp( void ) { return m_MyApp; }

	bool						setOfferInfo( OfferBaseInfo& offerInfo, GuiUser* guiUser = nullptr );
	OfferBaseInfo&				getOfferInfo( void )				{ return m_OfferInfo; }

	bool						setUser( GuiUser* guiUser );
	void						showIdentityWidget( bool showIdent );

	void						setPluginType( EPluginType pluginType );

	void						setOfferMessage( QString msgText );
	QString						getOfferMessage( void );

	void						setCanViewOffer( bool canView ) { m_CanViewInfo = canView; }
	bool						getCanViewOffer( void )			{ return m_CanViewInfo; }

	void						updateOfferInfo( void );
	
	void						clearOffer( void )				{ m_OfferInfo.clear(); }

signals:
	void						signalOfferSent( void );

protected slots:
	void						slotOfferViewButtonClicked( void );
	void						slotOfferSendButtonClicked( void );
	void						slotExpiresTimeChange( int comboIdx );

protected:
	bool						validateOffer( bool showErrorMsg = false );
	void						updateExpireStatus( EExpireTime expireTime );

	Ui::OfferSendWidgetUi		ui;
	AppCommon&				    m_MyApp;
	GuiUser*					m_HisIdent{ nullptr };
	EPluginType					m_PluginType{ ePluginTypeInvalid };
	OfferBaseInfo				m_OfferInfo;

	bool						m_NeedFileHash{ false };
	int							m_ExpireSeconds{ 0 };
	bool						m_CanViewInfo{ true };
};
