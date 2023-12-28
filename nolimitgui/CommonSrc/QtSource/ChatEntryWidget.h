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

#include "ui_ChatEntryWidget.h"

#include <CoreLib/AssetDefs.h>

class VxNetIdent;

class ChatEntryWidget : public QWidget
{
    Q_OBJECT

public:
    ChatEntryWidget( QWidget* parent = nullptr, EAssetType inputMode = eAssetTypeUnknown );
	virtual ~ChatEntryWidget() = default;

	void						setGroupieId( GroupieId& groupieId );
	GroupieId					getGroupieId( void )						{ return  m_GroupieId; }

	void						setEntryMode( EAssetType inputMode );
	void						setIsPersonalRecorder( bool isPersonal );

	void						setAppModule( EAppModule appModule );
	EAppModule					getAppModule( void )						{ return  m_AppModule; }

	void						setPluginType( EPluginType pluginType );
	EPluginType					getPluginType( void )						{ return  m_PluginType; }

	void						hideVideoCaptureInput( void ); // video takes a lot of bandwidth.. disable for chat room host

	void						callbackGuiPlayMotionVideoFrame( VxGUID& feedOnlineId, QImage& vidFrame, int motion0To100000 );

	void						setCanSend( bool canSend );

	void						setInputClientCallback( InputClientCallback* clientCallback );

	bool						checkIfCanSend( void );

signals:
	void						signalUserInputButtonClicked( void );

private slots:
	void						slotAllTextButtonClicked( void );
	void						slotAllEmoteButtonClicked( void );
	void						slotAllVideoButtonClicked( void );
	void						slotAllCameraButtonClicked( void );
	void						slotAllGalleryButtonClicked( void );
	void						slotAllMicButtonPressed( void );
	void						slotAllMicButtonReleased( void );
	void						slotInputCompleted( void );

private:
	Ui::ChatEntryWidget			ui;
	EAssetType					m_InputMode{ eAssetTypeUnknown };
	GroupieId					m_GroupieId;

	EAppModule					m_AppModule{ eAppModuleInvalid };
	EPluginType					m_PluginType{ ePluginTypeInvalid };

	bool						m_IsPersonalRecorder{ false };

	InputClientCallback*		m_ClientCallback{ nullptr };
};
