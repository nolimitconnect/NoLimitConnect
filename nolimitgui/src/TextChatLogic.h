#pragma once
#include <QWidget>
//============================================================================
// Copyright (C) 2016 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================


#include <PktLib/VxCommon.h>

class AppCommon;
class EngineSettings;
class P2PEngine;
class TodGameWidget;
class QFrame;
class QLabel;
class VidWidget;
class VxPushButton;

class TextChatLogic : public QWidget
{
	Q_OBJECT

public:
	TextChatLogic( AppCommon& myApp, P2PEngine& engine, EPluginType pluginType, QWidget* parent );

	void						setGuiWidgets(	VxNetIdent*	hisIdent );

signals:
	void						signalUserWentOffline( void );

protected slots:

protected:
	void						handleUserWentOffline( void );

	AppCommon& 				m_MyApp;
	P2PEngine&					m_Engine; 
	EPluginType					m_ePluginType;
	VxNetIdent*				m_HisIdent;
};
