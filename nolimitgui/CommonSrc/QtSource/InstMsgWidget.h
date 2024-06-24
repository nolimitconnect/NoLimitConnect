#pragma once
//============================================================================
// Copyright (C) 2015 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "ui_InstMsgWidget.h"
#include <GuiInterface/IDefs.h>

class AppCommon;
class GuiUser;
class P2PEngine;
class VxNetIdent;

class InstMsgWidget : public QWidget
{
	Q_OBJECT

public:
	InstMsgWidget( QWidget* parent = nullptr );

	void						setInstMsgWidgets( EPluginType pluginType, GuiUser* hisIdent );
	void						toGuiInstMsg( QString instMsg );

signals:
	void						signalToGuiInstMsg( QString instMsg );

private slots:
	void						slotClearHistoryButtonClicked( void );
	void						slotCancelButtonClicked( void );
	void						slotSendButtonClicked( void );
	void						slotToGuiInstMsg( QString instMsg );

private:
	void						sendChatMsg( QString strChatMsg );
	void						appendHistoryMsg( QString strChatMsg );

	Ui::InstMsgWidgetClass		ui;
	AppCommon&				    m_MyApp;
	P2PEngine&					m_Engine;
	EPluginType					m_ePluginType;
    GuiUser*				    m_HisIdent;
};
