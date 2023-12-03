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

#include "ToGuiActivityInterface.h"

#include <QListWidget>

class AppCommon;
class P2PEngine;
class VxNetIdent;
class AssetBaseWidget;
class AssetBaseInfo;

class HistoryListWidget : public QListWidget, public ToGuiActivityInterface
{
	Q_OBJECT

public:
	HistoryListWidget(QWidget* parent=nullptr);
	virtual ~HistoryListWidget();

	void						setPluginType( EPluginType pluginType ) { m_PluginType = pluginType; }
	EPluginType					getPluginType( void )					{ return  m_PluginType; }

	void						setIdents( GuiUser* myIdent, GuiUser* hisIdent );
	void						onActivityStop( void );

	void						initializeHistory( void );

public slots:
	void						slotShreddingAsset( AssetBaseWidget * assetWidget );
	void						slotStartupTimeout( void );

protected:
    void						showEvent( QShowEvent* ev ) override;
    void						hideEvent( QHideEvent* ev ) override;

    virtual void				toGuiAssetSessionHistory( AssetBaseInfo& assetInfo ) override;
    virtual void				toGuiAssetAdded( AssetBaseInfo& assetInfo ) override;
	virtual void				toGuiClientAssetAction( EAssetAction assetAction, VxGUID& assetId, int pos0to100000 ) override;

    AssetBaseWidget *			createAssetWidget( AssetBaseInfo* assetInfo );
	AssetBaseWidget*			findAssetWidget( VxGUID& assetId );

    int							determinInsertIndex( AssetBaseInfo* assetInfo );
	void						clearHistoryList( void );

	AppCommon&					m_MyApp;
	P2PEngine&					m_Engine;
	EPluginType					m_PluginType{ ePluginTypeInvalid };
	GuiUser*					m_MyIdent{ nullptr };
    GuiUser*				    m_HisIdent{ nullptr };
	bool						m_CallbacksRequested{ false };
	QTimer*						m_StartupTimer{ nullptr };
	bool						m_QueryHistoryCalled{ false };
};
