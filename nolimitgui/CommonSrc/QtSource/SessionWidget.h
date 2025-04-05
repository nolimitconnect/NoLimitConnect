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

#include <QWidget>

#include <GuiInterface/IDefs.h>

#include <CoreLib/AssetDefs.h>
#include <CoreLib/GroupieId.h>

QT_BEGIN_NAMESPACE
namespace Ui {
    class SessionWidgetClass;
}
QT_END_NAMESPACE

class AppCommon;
class ChatEntryWidget;
class GuiUser;
class HistoryListWidget;
class InputClientBaseCallback;
class QLabel;

class SessionWidget : public QWidget
{
	Q_OBJECT

public:
	SessionWidget( QWidget* parent = nullptr, EAssetType inputMode = eAssetTypeUnknown );
    virtual ~SessionWidget() = default;

	void						setHostAdminId( GroupieId& adminId )		{ setGroupieId( adminId ); }
	void						setGroupieId( GroupieId& groupieId );
	GroupieId&					getGroupieId( void )						{ return m_GroupieId; }

	// for low bandwith hosts do not allow video and audio recordings
	void						setLimitToTextAndPhotos( bool justTextAndPhotos );
	void						setEntryMode( EAssetType inputMode );
	void						setIsPersonalRecorder( bool isPersonal );
	void						setCanSend( bool canSend );
	void						setInputClientCallback( InputClientBaseCallback* clientCallback );

	void						setAppModule( EAppModule appModule );

	void						setPluginType( EPluginType pluginType );
	EPluginType					getPluginType( void )						{ return  m_PluginType; }

	void						callbackGuiPlayMotionVideoFrame( VxGUID& feedOnlineId, QImage& vidFrame, int motion0To100000 );

	void						onActivityStop( void );

    QLabel *                    getSessionStatusLabel( void );
    HistoryListWidget *         getSessionHistoryList( void );
    ChatEntryWidget *           getSessionChatEntry( void );
	void						initializeHistory( void );

signals:
	void						signalUserInputButtonClicked( void );

protected slots:
    void                        slotStatusMsg( QString );
    void                        slotCreateInviteButtonClicked( void );

protected:
	void						hideVideoCaptureInput( void );
	void						setupAdminInvite( void );

	Ui::SessionWidgetClass&		ui;
    AppCommon&                  m_MyApp;
	EAssetType					m_InputMode;
	bool						m_IsInitialized{ false };
	bool						m_JustTextAndPhotos{ false };
	EPluginType					m_PluginType{ ePluginTypeInvalid };
	GroupieId					m_GroupieId;

	InputClientBaseCallback*	m_ClientCallback{ nullptr };
};
