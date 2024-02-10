#pragma once
//============================================================================
// Copyright (C) 2013 Brett R. Jones
//
// Code copyrighted by Brett R. Jones is under dual license similar to Ruby's license
// See file COPYING and LEGAL in root of the No Limit Connect project
//
// bjones.engineer@gmail.com
// https://nolimitconnect.com
//============================================================================

#include "ui_SessionWidget.h"

#include <GuiInterface/IDefs.h>

#include <CoreLib/AssetDefs.h>
#include <PktLib/GroupieId.h>

class GuiUser;
class InputClientBaseCallback;

class SessionWidget : public QWidget
{
	Q_OBJECT

public:
	SessionWidget( QWidget* parent = nullptr, EAssetType inputMode = eAssetTypeUnknown );
    virtual ~SessionWidget() = default;

	void						setHostAdminId( GroupieId& adminId )		{ setGroupieId( adminId ); }
	void						setGroupieId( GroupieId& groupieId );
	GroupieId&					getGroupieId( void )						{ return m_GroupieId; }

	void						setEntryMode( EAssetType inputMode );
	void						setIsPersonalRecorder( bool isPersonal );
	void						setCanSend( bool canSend );
	void						setInputClientCallback( InputClientBaseCallback* clientCallback );

	void						setAppModule( EAppModule appModule );

	void						setPluginType( EPluginType pluginType );
	EPluginType					getPluginType( void )						{ return  m_PluginType; }

	void						callbackGuiPlayMotionVideoFrame( VxGUID& feedOnlineId, QImage& vidFrame, int motion0To100000 );

	void						onActivityStop( void );

    QLabel *                    getSessionStatusLabel( void )				{ return ui.m_StatusLabel; }
    HistoryListWidget *         getSessionHistoryList( void )				{ return ui.m_HistoryList; }
    ChatEntryWidget *           getSessionChatEntry( void )					{ return ui.m_ChatEntry; }
	void						initializeHistory( void )					{ getSessionHistoryList()->initializeHistory(); }

signals:
	void						signalUserInputButtonClicked( void );

protected slots:
    void                        slotStatusMsg( QString );

protected:
	void						hideVideoCaptureInput( void );

	Ui::SessionWidgetClass		ui;
    AppCommon&                  m_MyApp;
	EAssetType					m_InputMode;
	bool						m_IsInitialized{ false };
	EPluginType					m_PluginType{ ePluginTypeInvalid };
	GroupieId					m_GroupieId;

	InputClientBaseCallback*		m_ClientCallback{ nullptr };
};
