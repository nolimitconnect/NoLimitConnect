#pragma once
//============================================================================
// Copyright (C) 2015 Brett R. Jones
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

#include <QWidget> // must be declared first or linux Qt 6.2.4 will error in qmetatype.h 2167:23: array subscript value 53 is outside the bounds

#include <CoreLib/AssetDefs.h>
#include <ptop_src/ptop_engine_src/AssetMgr/AssetInfo.h>
#include <PktLib/VxCommon.h>


class AppCommon;
class AssetInfo;
class ChatEntryWidget;
class GuiUser;
class MyIcons;

class InputBaseWidget : public QWidget
{
    Q_OBJECT

public:
	InputBaseWidget( AppCommon& app, QWidget* parent = nullptr );

	AppCommon&					getMyApp( void )							{ return m_MyApp; }
	MyIcons&					getMyIcons( void );

	void						setIdents( GuiUser* myIdent, GuiUser* hisIdent );
	void						setIsPersonalRecorder( bool isPersonal );
    void                        setIsChatRoom( bool isChatRoom );

	void						setAppModule( EAppModule appModule )		{ m_AppModule = appModule; }
	EAppModule					getAppModule( void )						{ return  m_AppModule; }

	void						setPluginType( EPluginType pluginType )		{ m_PluginType = pluginType; }
	EPluginType					getPluginType( void )						{ return  m_PluginType; }

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

	//=== vars ===//
	AppCommon&				    m_MyApp;
	ChatEntryWidget *			m_ChatEntryWidget{ nullptr };
    GuiUser*				    m_MyIdent{ nullptr };
    GuiUser*				    m_HisIdent{ nullptr };
    AssetBaseInfo				m_AssetInfo;
	std::string					m_FileName;
	int64_t						m_TimeRecStart{ 0 };
    int64_t						m_TimeRecCurrent{ 0 };
	QTimer *					m_ElapseTimer{ nullptr };
    bool						m_IsPersonalRecorder{ false };
	EAppModule					m_AppModule{ eAppModuleInvalid };
	EPluginType					m_PluginType{ ePluginTypeInvalid };
};
