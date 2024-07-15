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

#include <QWidget> // must be declared first or linux Qt will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds

#include <AssetMgr/AssetInfo.h>
#include <CoreLib/AssetDefs.h>
#include <PktLib/VxCommon.h>

class AppCommon;
class AssetInfo;
class ChatEntryWidget;
class GuiUser;
class MyIcons;
class InputClientBaseCallback;

class InputBaseWidget : public QWidget
{
    Q_OBJECT

public:
	InputBaseWidget( AppCommon& app, QWidget* parent = nullptr );

	AppCommon&					getMyApp( void )							{ return m_MyApp; }
	MyIcons&					getMyIcons( void );

	void						setGroupieId( GroupieId& groupieId );

	void						setIsPersonalRecorder( bool isPersonal );
    void                        setIsChatRoom( bool isChatRoom );

	void						setAppModule( EAppModule appModule )		{ m_AppModule = appModule; }
	EAppModule					getAppModule( void )						{ return  m_AppModule; }

	void						setPluginType( EPluginType pluginType )		{ m_PluginType = pluginType; }
	EPluginType					getPluginType( void )						{ return  m_PluginType; }

	void						setInputClientCallback( InputClientBaseCallback* clientCallback );
	bool						canAcceptInput( EAssetType assetType );

signals:
	void						signalChatMessage( QString chatMsg );
	void						signalElapsedRecTime( QString elapsedSec );
	void						signalInputCompleted( void );

public slots:
	void						slotChatMessage( QString chatMsg );
	void						slotElapsedTimerTimeout( void );

protected:
	bool						voiceRecord( EAssetAction action );
	bool						videoRecord( EAssetAction action );
    bool						generateFileName( EAssetType assetType, VxGUID& uniqueId );
	void						updateElapsedTime( void );

	bool						fillAssetBaseInfo( bool newAssetId );

	bool						addOptionalComment( void );

	bool						checkIfCanSend( void );

	//=== vars ===//
	AppCommon&				    m_MyApp;
	ChatEntryWidget *			m_ChatEntryWidget{ nullptr };

    AssetBaseInfo				m_AssetInfo;
	std::string					m_FileName;
	int64_t						m_TimeRecStart{ 0 };
    int64_t						m_TimeRecCurrent{ 0 };
	QTimer *					m_ElapseTimer{ nullptr };
    bool						m_IsPersonalRecorder{ false };
	EAppModule					m_AppModule{ eAppModuleInvalid };
	EPluginType					m_PluginType{ ePluginTypeInvalid };

    GroupieId                   m_GroupieId;

	InputClientBaseCallback*		m_ClientCallback{ nullptr };
};
